#! /bin/sh

mkdir -p tests_build
cd ./tests_build || exit
cmake -S .. -B . -DCMAKE_BUILD_TYPE=Release
make
./actrie_tests
