#!/usr/bin/env bash

rm -rf release
mkdir release
cd release
mkdir -p config/sys-screenuploader
mkdir -p atmosphere/titles/42000062616B6101/flags
cp ../config.ini config/sys-screenuploader/config.ini
cp ../toolbox.json atmosphere/titles/42000062616B6101/toolbox.json
cp ../cmake-build-release/screenuploader.nsp atmosphere/titles/42000062616B6101/exefs.nsp
touch atmosphere/titles/42000062616B6101/flags/boot2.flag
zip -r sys-screenuploader.zip *
