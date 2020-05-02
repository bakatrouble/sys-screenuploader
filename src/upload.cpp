#include <iostream>
#include <cstdio>
#include <netdb.h>
#include <filesystem>
#include <curl/curl.h>
#include "config.hpp"
#include "logger.hpp"

namespace fs = filesystem;

struct upload_info {
    FILE *f;
    size_t sizeLeft;
};

static size_t _uploadReadFunction(void *ptr, size_t size, size_t nmemb, void *data) {
    auto *ui = (struct upload_info*)data;
    size_t maxBytes = size * nmemb;
    if (maxBytes < 1)
        return 0;

    if (ui->sizeLeft) {
        size_t bytes = min(ui->sizeLeft, maxBytes);
        size_t bytesRead = fread(ptr, (size_t)1, bytes, ui->f);
        ui->sizeLeft -= bytesRead;
        return bytes;
    }

    return 0;
}

bool sendFileToServer(string &path, size_t size) {
    string tid = path.substr(path.length() - 36, 32);
    Logger::get().debug() << "Title ID: " << tid << endl;
    if (!Config::get().uploadAllowed(tid, path.back() == '4')) {
        Logger::get().info() << "Not uploading" << endl;
        return true;
    }

    fs::path fpath(path);

    FILE *f = fopen(path.c_str(), "rb");

    if (f == nullptr) {
        Logger::get().error() << "fopen() failed" << endl;
        return false;
    }

    struct upload_info ui = { f, size };

    CURL *curl = curl_easy_init();
    if (curl) {
        stringstream url;
        url << Config::get().getUrl(tid) << "?filename=" << fpath.filename().string() << Config::get().getUrlParams() ;
        Logger::get().debug() << "Upload URL: " << url.str() << endl;
        curl_easy_setopt(curl, CURLOPT_URL, url.str().c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        struct curl_slist *chunk = nullptr;
        chunk = curl_slist_append(chunk, "Accept: application/json");
        chunk = curl_slist_append(chunk, "Content-Type: application/octet-stream");
        chunk = curl_slist_append(chunk, "Content-Transfer-Encoding: binary");
        chunk = curl_slist_append(chunk, "Connection: close");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (curl_off_t)size);
        curl_easy_setopt(curl, CURLOPT_READDATA, &ui);
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, _uploadReadFunction);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 0x2000L);
        curl_easy_setopt(curl, CURLOPT_UPLOAD_BUFFERSIZE, 0x2000L);
        CURLcode res = curl_easy_perform(curl);
        fclose(f);

        if (res == CURLE_OK) {
            long responseCode;
            double requestSize;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
            curl_easy_getinfo(curl, CURLINFO_SIZE_UPLOAD, &requestSize);
            Logger::get().debug() << requestSize << " bytes sent, response code: " << responseCode << endl;
            curl_slist_free_all(chunk);
            curl_easy_cleanup(curl);
            return responseCode == 200;
        } else {
            Logger::get().error() << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
            curl_slist_free_all(chunk);
            curl_easy_cleanup(curl);
            return false;
        }
    }
    Logger::get().error() << "curl_easy_init() failed" << endl;
    return false;
}
