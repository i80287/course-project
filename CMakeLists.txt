cmake_minimum_required(VERSION 3.5)

project(vis_actrie_app VERSION 0.1.0 LANGUAGES C CXX)

set(EXTERNAL_DEPENDENCIES_DIR "${PROJECT_SOURCE_DIR}/external")

set(GLAD_DIR "${EXTERNAL_DEPENDENCIES_DIR}/glad")
set(GLAD_INCLUDE_DIR "${GLAD_DIR}/include")

set(GLFW_DIR "${EXTERNAL_DEPENDENCIES_DIR}/glfw")
set(GLFW_INCLUDE_DIR "${GLFW_DIR}/include")

set(IMGUI_DIR "${EXTERNAL_DEPENDENCIES_DIR}/imgui")
set(IMGUI_INCLUDE_DIR "${IMGUI_DIR}")
set(IMGUI_BACKENDS_INCLUDE_DIR "${IMGUI_DIR}/backends")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS ON)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(HEADER_FILES
    "${PROJECT_SOURCE_DIR}/ACTrie.hpp"
    "${PROJECT_SOURCE_DIR}/App.hpp"
    "${PROJECT_SOURCE_DIR}/Drawer.hpp"
    "${PROJECT_SOURCE_DIR}/glad_fix.hpp"
    "${PROJECT_SOURCE_DIR}/app_config.hpp"
    "${PROJECT_SOURCE_DIR}/SendTypeHelper.hpp")

add_library(glad "${GLAD_DIR}/src/glad.c" "${GLAD_INCLUDE_DIR}/glad/glad.h")
target_include_directories(glad PUBLIC include/)

add_subdirectory(external/glfw)
# add_library(glfw STATIC IMPORTED)

add_executable(vis_actrie_app
    main.cpp
    App.cpp
    ACTrie.cpp
    Drawer.cpp
    Observable.cpp
    "${IMGUI_DIR}/imgui.cpp"
    "${IMGUI_DIR}/imgui_demo.cpp"
    "${IMGUI_DIR}/imgui_draw.cpp"
    "${IMGUI_DIR}/imgui_tables.cpp"
    "${IMGUI_DIR}/imgui_widgets.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_glfw.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
    ${HEADER_FILES}
)

include_directories(${PROJECT_SOURCE_DIR}
    ${GLAD_INCLUDE_DIR}
	${GLFW_INCLUDE_DIR}
	${IMGUI_INCLUDE_DIR}
    ${IMGUI_BACKENDS_INCLUDE_DIR}
)

target_link_libraries(vis_actrie_app glad glfw)

set_property(TARGET vis_actrie_app PROPERTY CXX_STANDARD 20)

message(STATUS "CMAKE_C_FLAGS: ${CMAKE_C_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")

    elseif (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "GNU")
        
    endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(vis_actrie_app PRIVATE 
        -Wall
        -Wextra
        -Wpedantic
        -Wunused
        -pedantic-errors
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
    )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

endif()
