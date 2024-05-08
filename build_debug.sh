#! /bin/sh

mkdir -p build_debug
cd ./build_debug || exit
cmake -DCMAKE_BUILD_TYPE=Debug -S .. -B .
make --jobs "$(nproc)"
