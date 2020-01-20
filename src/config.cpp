#include <iostream>
#include <inih.h>
#include "config.hpp"

using namespace std;

Config Config::load() {
    Config conf;
    conf.refresh();
    return conf;
}

bool Config::refresh() {
    INIReader reader("sdmc:/config/sys-screenuploader/config.ini");

    if (reader.ParseError() != 0) {
        cout << "Config parse error " << reader.ParseError() << endl;
        return false;
    }

    string destid = reader.Get("server", "destination_id", "undefined");
    string url = reader.Get("server", "url", "https://screenuploader.bakatrouble.me/upload/" + URLplaceholder + "/");
    if (url.find(URLplaceholder) != string::npos) {
        url.replace(url.find(URLplaceholder), URLplaceholder.length(), destid);
    }
    m_url = url;

    string albumPath = reader.Get("directories", "album_path", "");
    if (albumPath.length() > 0)
        m_albumPath = albumPath;

    return true;
}

string Config::getUrl() {
    return m_url;
}

string Config::getAlbumPath() {
    return m_albumPath;
}
