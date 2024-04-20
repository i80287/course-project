if not exist ".\debug" mkdir debug
cd .\debug
cmake -G "Unix Makefiles" -S .. -B . -DCMAKE_BUILD_TYPE=Debug
make
