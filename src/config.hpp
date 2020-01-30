#pragma once

#include <string>
#include <switch.h>

using namespace std;


const string URLplaceholder = "<destid>";

class Config {
public:
    static Config load();
    bool refresh();

    string getUrl();

protected:
    string m_url = "https://screenuploader.bakatrouble.me/upload/" + URLplaceholder + "/";
};
