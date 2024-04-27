if not exist ".\debug_vs15" mkdir debug_vs15
cd .\debug_vs15
cmake -G "Visual Studio 16 2019" -S .. -B . -DCMAKE_BUILD_TYPE=Debug
