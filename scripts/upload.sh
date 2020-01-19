#!/usr/bin/env bash

cd cmake-build-debug
ftp -inv 192.168.1.176 5000 << EOF
cd /atmosphere/contents/42000062616b6101
delete exefs.nsp
put screenuploader.nsp
rename screenuploader.nsp exefs.nsp
EOF
