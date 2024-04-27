#! /bin/sh

mkdir -p build_debug
cd ./build_debug || exit
cmake -S .. -B . -DCMAKE_BUILD_TYPE=Debug
make --jobs "$(nproc)"
