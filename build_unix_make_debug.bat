if not exist ".\build_debug" mkdir build_debug
cd .\build_debug
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DVIS_ACTRIE_APP_STATIC_LINK_GCC_STD_WINPTHREAD=ON -S .. -B .
make --jobs 4
