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
        reader.Get("server", "destination_id", "undefined"),
        reader.Get("server", "host", "screenuploader.bakatrouble.me"),
        reader.GetInteger("server", "port", 80),
    };
    return conf;
}
