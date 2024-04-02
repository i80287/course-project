#! /bin/sh

mkdir -p debug
cd ./debug || exit
cmake -S .. -B . -DCMAKE_BUILD_TYPE=Debug
make
