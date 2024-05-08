#! /bin/sh

mkdir -p build_release_unix
cd ./build_release_unix || exit
cmake -DCMAKE_BUILD_TYPE=Release -S .. -B .
make --jobs "$(nproc)"
