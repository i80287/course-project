if not exist ".\release" mkdir release
cd .\release
cmake -G "Unix Makefiles" -S .. -B . -DCMAKE_BUILD_TYPE=Release
make
