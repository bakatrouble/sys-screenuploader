#pragma once

#include <string>
#include <switch.h>
#include <map>

using namespace std;


const string URLplaceholder = "<destid>";

class Config {
public:
    static Config load();
    bool refresh();

    string getUrl(string &tid);
    string getUrlParams();
    bool uploadAllowed(string &tid, bool isMovie);
    bool keepLogs();

protected:
    string m_url = "https://screenuploader.bakatrouble.me/upload/" + URLplaceholder + "/";
    string m_defaultDestID;
    bool m_uploadScreenshots;
    bool m_uploadMovies;
    bool m_keepLogs;
    map<string, string> m_titleSettings;
    map<string, bool> m_titleScreenshots;
    map<string, bool> m_titleMovies;
    map<string, string> m_urlParams;
};
