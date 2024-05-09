#! /bin/sh

mkdir -p build_release
cd ./build_release || exit
cmake -DCMAKE_BUILD_TYPE=Release -S .. -B .
make --jobs "$(nproc)"
