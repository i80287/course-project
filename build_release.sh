#! /bin/sh

mkdir -p release
cd ./release || exit
cmake -S .. -B . -DCMAKE_BUILD_TYPE=Release
make
