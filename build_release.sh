#! /bin/sh

mkdir -p build_release_unix
cd ./build_release_unix || exit
cmake -S .. -B . -DCMAKE_BUILD_TYPE=Release
make --jobs "$(nproc)"
