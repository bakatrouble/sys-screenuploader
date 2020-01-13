#pragma once

#include <string>
#include <switch.h>

using namespace std;

class Config {
public:
    string host = "undefined";
    long port = 0;

    static Config load();
};
