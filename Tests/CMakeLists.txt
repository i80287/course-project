cmake_minimum_required(VERSION 3.5)

project(actrie_tests VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(actrie_tests
    main.cpp
    tests.cpp
    ../App/ACTrie.cpp
)

include_directories(${PROJECT_SOURCE_DIR})

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")

    elseif (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "GNU")

    elseif (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "AppleClang")

    endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(actrie_tests PRIVATE
        -Wall
        -Wextra
        -Wlogical-op
        -Wcast-qual
        -Wpedantic
        -Wshift-overflow=2
        -Wduplicated-cond
        -Wunused
        -Wconversion
        -Wunsafe-loop-optimizations
        -Wshadow
        -Wnull-dereference
        -Wundef
        -Wwrite-strings
        -Wsign-conversion
        -Wmissing-noreturn
        -Wunreachable-code
        -Wcast-align
        -Warray-bounds=2
        -Wformat=2
    )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

endif()
