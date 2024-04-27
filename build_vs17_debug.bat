if not exist ".\debug_vs17" mkdir debug_vs17
cd .\debug_vs17
cmake -G "Visual Studio 17 2022" -S .. -B . -DCMAKE_BUILD_TYPE=Debug
