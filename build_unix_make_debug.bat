if not exist ".\build_debug" mkdir build_debug
cd .\build_debug
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -S .. -B .
make
