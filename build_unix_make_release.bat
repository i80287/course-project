if not exist ".\build_release" mkdir build_release
cd .\build_release
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -S .. -B .
make
