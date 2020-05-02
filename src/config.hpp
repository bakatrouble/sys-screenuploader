#pragma once

#include <string>
#include <switch.h>
#include <map>

using namespace std;


const string URLplaceholder = "<destid>";
const string defaultUrl = "https://screenuploader.bakatrouble.me/upload/" + URLplaceholder + "/";

class Config {
public:
    static Config& get() {
        static Config instance;
        return instance;
    }

    bool refresh();

    string getUrl(string &tid);
    string getUrlParams();
    bool uploadAllowed(string &tid, bool isMovie);
    bool keepLogs();

    bool error;

private:
    string m_url;
    string m_defaultDestID;
    bool m_uploadScreenshots;
    bool m_uploadMovies;
    bool m_keepLogs;
    map<string, string> m_titleSettings;
    map<string, bool> m_titleScreenshots;
    map<string, bool> m_titleMovies;
    map<string, string> m_urlParams;
};
