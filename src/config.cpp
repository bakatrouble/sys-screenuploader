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

    string destid = reader.Get("server", "destination_id", "undefined");
    string url = reader.Get("server", "url", "https://screenuploader.bakatrouble.me/upload/" + URLplaceholder + "/");
    if (url.find(URLplaceholder) != string::npos) {
        url.replace(url.find(URLplaceholder), URLplaceholder.length(), destid);
    }

    Config conf = { url };
    return conf;
}
