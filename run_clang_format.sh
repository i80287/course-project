#! /bin/sh

clang-format -i ./*.cpp ./App/*.cpp ./App/*.hpp ./GraphicsUtils/*.cpp ./GraphicsUtils/*.hpp -style=file:./.clang-format -fallback-style=Google
