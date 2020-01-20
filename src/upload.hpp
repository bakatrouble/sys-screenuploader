#pragma once
#include <switch.h>
#include <string>
#include "config.hpp"

using namespace std;

bool sendFileToServer(Config &conf, string &path, size_t size);
