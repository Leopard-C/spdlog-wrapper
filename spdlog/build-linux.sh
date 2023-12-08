#!/bin/bash

set -e

script_abs=$(readlink -f "$0")
script_dir=$(dirname $script_abs)

build_dir=$script_dir/build
out_lib_dir=$script_dir/../spdlog-wrapper/lib/linux

if [ ! -e $build_dir ]; then mkdir $build_dir; fi
if [ ! -e $out_lib_dir ]; then mkdir -p $out_lib_dir; fi

cd build
cmake ..
make -j

cp libspdlog.a $out_lib_dir

echo "Success!"
