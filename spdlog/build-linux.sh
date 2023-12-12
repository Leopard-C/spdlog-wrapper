#!/bin/bash

set -e

PrintUsage()
{
    echo "usage:"
    echo "  ./build-linux.sh [toolchain-config]"
    echo ""
    echo "for example:"
    echo "  ./build-linux.sh"
    echo "  ./build-linux.sh default"
    echo "  ./build-linux.sh aarch64"
}

script_abs=$(readlink -f "$0")
script_dir=$(dirname $script_abs)

toolchain_config=default

if [ $# -eq 1 ]; then
    toolchain_config=$1
elif [ $# -gt 1 ]; then
    PrintUsage
    exit 1
fi

toolchain_file="$script_dir/cmake/toolchain.$toolchain_config.cmake"
if [ ! -e "$toolchain_file" ]; then
    echo "ERROR: not found file '$toolchain_file'"
    PrintUsage
    exit 2
fi

build_dir=$script_dir/build
out_lib_dir=$script_dir/../spdlog-wrapper/lib/linux

if [ ! -e $build_dir ]; then mkdir $build_dir; fi
if [ ! -e $out_lib_dir ]; then mkdir -p $out_lib_dir; fi

cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=$toolchain_file
make -j

cp libspdlog.a $out_lib_dir

echo "Success!"
