#!/bin/bash

set -e

if [ ! -e ./build ]; then
    mkdir build
fi

cd build
cmake ..
make -j
cp libspdlog.a ../../spdlog-wrapper/lib/linux
echo "Success!"
