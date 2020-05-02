#include <iostream>
#include <inih.h>
#include <set>
#include "config.hpp"
#include "utils.hpp"
#include "logger.hpp"

using namespace std;

bool Config::refresh() {
    INIReader reader("sdmc:/config/sys-screenuploader/config.ini");

    if (reader.ParseError() != 0) {
        Logger::get().error() << "Config parse error " << reader.ParseError() << endl;
        error = true;
        return false;
    }

    m_defaultDestID = reader.Get("server", "destination_id", "undefined");
    m_uploadScreenshots = reader.GetBoolean("server", "upload_screenshots", true);
    m_uploadMovies = reader.GetBoolean("server", "upload_movies", true);
    m_keepLogs = reader.GetBoolean("server", "keep_logs", false);

    if (reader.Sections().count("destinations") > 0) {
        map<string, string> destinations;
        for (auto &destName : reader.Fields("destinations")) {
            destinations[destName] = reader.Get("destinations", destName, m_defaultDestID);
        }

        if (reader.Sections().count("title_settings") > 0) {
            for (auto &tid : reader.Fields("title_settings")) {
                string destName = reader.Get("title_settings", tid, ";");  // ";" is guaranteed to not be in a field name?
                m_titleSettings[tid] = !destinations[destName].empty() ? destinations[destName] : m_defaultDestID;
            }
        }

        string defaultDestName = reader.Get("server", "default_destination", ";");
        if (!destinations[defaultDestName].empty()) {
            m_defaultDestID = destinations[defaultDestName];
        }
    }

    if (reader.Sections().count("title_screenshots") > 0) {
        for (auto &tid : reader.Fields("title_screenshots")) {
            m_titleScreenshots[tid] = reader.GetBoolean("title_screenshots", tid, true);
        }
    }

    if (reader.Sections().count("title_movies") > 0) {
        for (auto &tid : reader.Fields("title_movies")) {
            m_titleMovies[tid] = reader.GetBoolean("title_movies", tid, true);
        }
    }

    m_url = reader.Get("server", "url", defaultUrl);

    if (reader.Sections().count("url_params") > 0) {
        for (auto &key : reader.Fields("url_params")) {
            m_urlParams[key] = reader.Get("url_params", key, "");
        }
    }

    return true;
}

string Config::getUrl(string &tid) {
    string destID,
           url = m_url;
    if (!m_titleSettings[tid].empty())
        destID = m_titleSettings[tid];
    else
        destID = m_defaultDestID;
    if (url.find(URLplaceholder) != string::npos) {
        url.replace(url.find(URLplaceholder), URLplaceholder.length(), destID);
    }
    return url;
}

string Config::getUrlParams() {
    if (m_urlParams.empty())
        return "";
    stringstream ss;
    for (auto &urlParam : m_urlParams) {
        ss << "&" << url_encode(urlParam.first) << "=" << url_encode(urlParam.second);
    }
    return ss.str();
}

bool Config::uploadAllowed(string &tid, bool isMovie) {
    if (isMovie) {
        if (m_titleMovies.count(tid) > 0)
            return m_titleMovies[tid];
        return m_uploadMovies;
    } else {
        if (m_titleScreenshots.count(tid) > 0)
            return m_titleScreenshots[tid];
        return m_uploadScreenshots;
    }
}

bool Config::keepLogs() {
    return m_keepLogs;
}
