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

protected:
    string m_url = "https://screenuploader.bakatrouble.me/upload/" + URLplaceholder + "/";
    string m_defaultDestID;
    map<string, string> m_titleSettings;
};
