if not exist ".\build_vs17_clang" mkdir build_vs17_clang
cd .\build_vs17_clang
cmake -G "Visual Studio 17 2022" -T ClangCL -S .. -B .
