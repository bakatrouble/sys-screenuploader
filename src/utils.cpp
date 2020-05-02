#include <vector>
#include <cstring>
#include <string>
#include <algorithm>
#include <fstream>
#include <iostream>

#include "utils.hpp"

string getAlbumPath() {
    return "img:/";
}

bool isDigitsOnly(const string &str) {
    return str.find_first_not_of("0123456789") == string::npos;
}

string getLastAlbumItem() {
    vector<string> years, months, days, files;
    string albumPath = getAlbumPath();
    if (!fs::is_directory(albumPath)) return "<No album directory: " + albumPath + ">";

    for (auto &entry : fs::directory_iterator(albumPath))
        if (entry.is_directory() && isDigitsOnly(entry.path().filename()) && entry.path().filename().string().length() == 4)
            years.push_back(entry.path());
    if (years.empty()) return "<No years in " + albumPath + ">";
    sort(years.begin(), years.end());

    for (auto &entry : fs::directory_iterator(years.back()))
        if (entry.is_directory() && isDigitsOnly(entry.path().filename()) && entry.path().filename().string().length() == 2)
            months.push_back(entry.path());
    if (months.empty()) return "<No months in " + years.back() + ">";
    sort(months.begin(), months.end());

    for (auto &entry : fs::directory_iterator(months.back()))
        if (entry.is_directory() && isDigitsOnly(entry.path().filename()) && entry.path().filename().string().length() == 2)
            days.push_back(entry.path());
    if (days.empty()) return "<No days in " + months.back() + ">";
    sort(days.begin(), days.end());

    for (auto &entry : fs::directory_iterator(days.back()))
        if (entry.is_regular_file())
            files.push_back(entry.path());
    if (files.empty()) return "<No screenshots in " + days.back() + ">";
    sort(files.begin(), files.end());

    return files.back();
}

size_t filesize(string &path) {
    streampos begin, end;
    ifstream f(path, ios::binary);
    begin = f.tellg();
    f.seekg(0, ios::end);
    end = f.tellg();
    f.close();
    return end - begin;
}

string url_encode(const string &value) {
    ostringstream escaped;
    escaped.fill('0');
    escaped << hex;

    for (char c : value) {
        // Keep alphanumeric and other accepted characters intact
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
            continue;
        }

        // Any other characters are percent-encoded
        escaped << uppercase;
        escaped << '%' << setw(2) << int((unsigned char) c);
        escaped << nouppercase;
    }

    return escaped.str();
}
