#include <netinet/in.h>
#include <switch.h>
#include <fstream>
#include <dirent.h>
#include <iostream>
#include "upload.hpp"
#include "utils.hpp"

using namespace std;

#define INNER_HEAP_SIZE 0x50000

extern "C" {
    extern u32 __start__;

    u32 __nx_applet_type = AppletType_None;

    size_t nx_inner_heap_size = INNER_HEAP_SIZE;
    char nx_inner_heap[INNER_HEAP_SIZE];

    void __libnx_init_time(void);
    void __libnx_initheap(void);
    void __appInit(void);
    void __appExit(void);

    // we override libnx internals to do a minimal init
    void __libnx_initheap(void) {
        void*  addr = nx_inner_heap;
        size_t size = nx_inner_heap_size;

        extern char *fake_heap_start;
        extern char *fake_heap_end;

        // setup newlib fake heap
        fake_heap_start = (char*)addr;
        fake_heap_end = (char*)addr + size;
    }

    void __appInit(void) {
        Result rc;
        rc = smInitialize();
        if (R_FAILED(rc))
            fatalThrow(rc);
        rc = setsysInitialize();
        if (R_SUCCEEDED(rc)) {
            SetSysFirmwareVersion fw;
            rc = setsysGetFirmwareVersion(&fw);
            if (R_SUCCEEDED(rc))
                hosversionSet(MAKEHOSVERSION(fw.major, fw.minor, fw.micro));
            else
                fatalThrow(rc);
            setsysExit();
        }
        else
            fatalThrow(rc);
        rc = pmdmntInitialize();
        if (R_FAILED(rc))
            fatalThrow(rc);
        rc = nsInitialize();
        if (R_FAILED(rc))
            fatalThrow(rc);

        SocketInitConfig sockConf = {
            .bsdsockets_version = 1,

            .tcp_tx_buf_size = 0x800,
            .tcp_rx_buf_size = 0x1000,
            .tcp_tx_buf_max_size = 0x2EE0,
            .tcp_rx_buf_max_size = 0x2EE0,

            .udp_tx_buf_size = 0x0,
            .udp_rx_buf_size = 0x0,

            .sb_efficiency = 4,
        };
        rc = socketInitialize(&sockConf);
        if (R_FAILED(rc))
            fatalThrow(rc);
        rc = pminfoInitialize();
        if (R_FAILED(rc))
            fatalThrow(rc);

        rc = fsInitialize();
        if (R_FAILED(rc))
            fatalThrow(rc);

        fsdevMountSdmc();
    }

    void __appExit(void) {
        fsdevUnmountAll();
        fsExit();
        pminfoExit();
        pmdmntExit();
        nsExit();
        socketExit();
        smExit();
    }
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
int main(int argc, char **argv) {
    mkdir("sdmc:/config", 0700);
    mkdir("sdmc:/config/sys-screenuploader", 0700);
    ofstream log("sdmc:/config/sys-screenuploader/screenuploader.log", ios::app);
    cout.rdbuf(log.rdbuf());
    cerr.rdbuf(log.rdbuf());

    cout << "=============================" << endl << endl << endl;
    cout << "ScreenUploader is starting..." << endl;

    string tmpItem, lastItem = getLastAlbumItem();
    cout << "Current last item: " << lastItem << endl;

    size_t fs;
    while (true) {
        tmpItem = getLastAlbumItem();
        if (lastItem != tmpItem) {
            fs = filesize(tmpItem);
            if (fs > 0) {
                cout << "=============================" << endl;
                cout << "New item found: " << tmpItem << endl;
                cout << "Filesize: " << fs << endl;
                if (sendFileToServer(tmpItem, fs))
                    lastItem = tmpItem;
            }
        }

		svcSleepThread(1e+9);
    }
}
#pragma clang diagnostic pop
