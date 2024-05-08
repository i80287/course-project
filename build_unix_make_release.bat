if not exist ".\build_release" mkdir build_release
cd .\build_release
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DVIS_ACTRIE_APP_STATIC_LINK_GCC_STD_WINPTHREAD=ON -S .. -B .
make --jobs 4
