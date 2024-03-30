#! /bin/sh

clang-tidy ./*.cpp ./App/*.cpp ./GraphicsUtils/*.cpp ./GraphicsUtils/DrawerUtils/*.cpp --config-file=.clang-tidy -- -std=c++20 -I./external/glad/include -I./external/glfw/include -I./external/imgui -I./external/imgui/backends
