#include <iostream>
#include <inih.h>
#include "config.hpp"

using namespace std;

Config Config::load() {
    INIReader reader("sdmc:/config/sys-screenuploader/config.ini");

    if (reader.ParseError() != 0) {
        cout << "Config parse error " << reader.ParseError() << endl;
        return Config();
    }

    Config conf = {
        reader.Get("server", "host", "undefined"),
        reader.GetInteger("server", "port", 0)
    };
    return conf;
}
