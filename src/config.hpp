#pragma once

#include <string>
#include <switch.h>

using namespace std;


const string URLplaceholder = "<destid>";

class Config {
public:
    string url = "https://screenuploader.bakatrouble.me/upload/" + URLplaceholder + "/";

    static Config load();
};
