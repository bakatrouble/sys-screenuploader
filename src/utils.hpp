#pragma once

#include <filesystem>
#include "config.hpp"

using namespace std;
namespace fs = filesystem;


string getLastAlbumItem(Config &conf);
size_t filesize(string &path);
string url_encode(const string &value);
