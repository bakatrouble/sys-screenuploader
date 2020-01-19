#pragma once
#include <switch.h>
#include <string>

using namespace std;

typedef struct trySend_Thread {
    Thread thread;
    string path;
    size_t fs;
    bool finished = false;
} trySend_Thread;

//bool sendFileToServer(string &path, size_t size);
void trySend(void *arg);
