if not exist ".\debug_unix" mkdir debug_unix
cd .\debug_unix
cmake -G "Unix Makefiles" -S .. -B . -DCMAKE_BUILD_TYPE=Debug
make
