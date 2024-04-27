if not exist ".\build_vs17" mkdir build_vs17
cd .\build_vs17
cmake -G "Visual Studio 17 2022" -S .. -B .
