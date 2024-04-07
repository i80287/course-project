CMAKE_MINIMUM_REQUIRED(VERSION 3.5)
PROJECT(imgui LANGUAGES CXX)

SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(IMGUI_DIR "${PROJECT_SOURCE_DIR}/imgui")

ADD_LIBRARY(imgui
    "${IMGUI_DIR}/imgui.cpp"
    "${IMGUI_DIR}/imgui_demo.cpp"
    "${IMGUI_DIR}/imgui_draw.cpp"
    "${IMGUI_DIR}/imgui_tables.cpp"
    "${IMGUI_DIR}/imgui_widgets.cpp"
)

TARGET_INCLUDE_DIRECTORIES(imgui PUBLIC ${IMGUI_DIR})

message(STATUS "Compiling ImGui")
