#include <vector>
#include <cstring>
#include <string>
#include <algorithm>
#include <fstream>

#include <switch.h>
#include <emummc_cfg.h>

#include "utils.hpp"

Result smcGetEmummcConfig(emummc_mmc_t mmc_id, emummc_config_t *out_cfg, void *out_paths) {
    SecmonArgs args;
    args.X[0] = 0xF0000404;
    args.X[1] = mmc_id;
    args.X[2] = (u64) out_paths;
    Result rc = svcCallSecureMonitor(&args);
    if (rc == 0) {
        if (args.X[0] != 0) {
            rc = (26u | ((u32) args.X[0] << 9u));
        }
        if (rc == 0) {
            memcpy(out_cfg, &args.X[1], sizeof(*out_cfg));
        }
    }
    return rc;
}

string getAlbumPath() {
    string out = "Nintendo/Album";
    static struct {
        char storage_path[0x7F + 1];
        char nintendo_path[0x7F + 1];
    } __attribute__((aligned(0x1000))) paths;

    emummc_config_t config;

    int x = smcGetEmummcConfig(EMUMMC_MMC_NAND, &config, &paths);
    if (x != 0) return out;
    if (config.base_cfg.type == 0) return out;
    out = paths.nintendo_path;
    out += "/Album";
    return out;
}

string getLastAlbumItem() {
    vector<string> years, months, days, files;
    string albumPath = getAlbumPath();
    if (!fs::is_directory(albumPath)) return "no album directory";

    for (auto &entry : fs::directory_iterator(albumPath))
        if (entry.is_directory())
            years.push_back(entry.path());
    if (years.empty()) return "no years";
    sort(years.begin(), years.end());

    for (auto &entry : fs::directory_iterator(years.back()))
        if (entry.is_directory())
            months.push_back(entry.path());
    if (months.empty()) return "no months";
    sort(months.begin(), months.end());

    for (auto &entry : fs::directory_iterator(months.back()))
        if (entry.is_directory())
            days.push_back(entry.path());
    if (days.empty()) return "no days";
    sort(days.begin(), days.end());

    for (auto &entry : fs::directory_iterator(days.back()))
        if (entry.is_regular_file())
            files.push_back(entry.path());
    if (files.empty()) return "no files";
    sort(files.begin(), files.end());

    return files.back();
}

size_t filesize(string path) {
    streampos begin, end;
    ifstream f(path, ios::binary);
    begin = f.tellg();
    f.seekg(0, ios::end);
    end = f.tellg();
    f.close();
    return end - begin;
}
