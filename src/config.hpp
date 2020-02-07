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
    bool uploadAllowed(string &tid, bool isMovie);

protected:
    string m_url = "https://screenuploader.bakatrouble.me/upload/" + URLplaceholder + "/";
    string m_defaultDestID;
    bool m_uploadScreenshots;
    bool m_uploadMovies;
    map<string, string> m_titleSettings;
    map<string, bool> m_titleScreenshots;
    map<string, bool> m_titleMovies;
};
