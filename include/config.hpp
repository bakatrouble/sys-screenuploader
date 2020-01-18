#pragma once

#include <string>
#include <switch.h>

using namespace std;

class Config {
public:
    string destination_id = "undefined";
    string host = "screenuploader.bakatrouble.me";
    long port = 80;

    static Config load();
};
