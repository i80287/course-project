if not exist ".\debug" mkdir debug
cd .\debug
cmake -S .. -B . -DCMAKE_BUILD_TYPE=Debug
make
