if not exist ".\build_vs16" mkdir build_vs16
cd .\build_vs16
cmake -G "Visual Studio 16 2019" -S .. -B .
