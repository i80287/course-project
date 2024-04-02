if not exist ".\tests_build" mkdir tests_build
cd .\tests_build
cmake -G "Unix Makefiles" -S .. -B . -DCMAKE_BUILD_TYPE=Release
make
.\actrie_tests
