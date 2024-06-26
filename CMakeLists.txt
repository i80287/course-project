cmake_minimum_required(VERSION 3.5)

project(vis_actrie_app VERSION 0.1.0 LANGUAGES C CXX)

option(VIS_ACTRIE_APP_STATIC_LINK_GCC_STD_WINPTHREAD "Statically link C and C++ standart libraries and Win pthread when using GCC on Windows" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(IMGUI_BACKENDS_DIR "${PROJECT_SOURCE_DIR}/external/imgui/backends")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(external) # For ImGui
add_subdirectory(external/glad)
add_subdirectory(external/glfw)

add_executable(vis_actrie_app
    main.cpp
    App/App.cpp
    App/ACTrie.cpp
    App/ACTrieController.cpp
    App/React.cpp
    GraphicsUtils/Drawer.cpp
    GraphicsUtils/DrawerUtils/StringHistoryManager.cpp
    GraphicsUtils/DrawerUtils/Logger.cpp
    GraphicsUtils/GLFWFacade.cpp
    GraphicsUtils/ImGuiFacade.cpp
    "${IMGUI_BACKENDS_DIR}/imgui_impl_glfw.cpp"
    "${IMGUI_BACKENDS_DIR}/imgui_impl_opengl3.cpp"
)

include_directories(${IMGUI_BACKENDS_DIR})

target_link_libraries(vis_actrie_app glad glfw imgui)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")

    elseif (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "GNU")
        if (CMAKE_BUILD_TYPE STREQUAL "Debug")
            target_compile_definitions(vis_actrie_app PRIVATE
                _LIBCPP_ENABLE_ASSERTIONS=1
            )
        endif()

    elseif (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "AppleClang")

    endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_definitions(vis_actrie_app PRIVATE
            _GLIBCXX_DEBUG
            _GLIBCXX_DEBUG_PEDANTIC
        )
    endif()
    target_compile_options(vis_actrie_app PRIVATE
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

    if (VIS_ACTRIE_APP_STATIC_LINK_GCC_STD_WINPTHREAD)
        target_link_options(vis_actrie_app PRIVATE -static)
    endif()

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

endif()

get_target_property(APP_COMPILATION_OPTIONS vis_actrie_app COMPILE_OPTIONS)
get_target_property(APP_LINK_OPTIONS vis_actrie_app LINK_OPTIONS)
message(STATUS "Additional app compilation options: ${APP_COMPILATION_OPTIONS}")
message(STATUS "Additional app link options: ${APP_LINK_OPTIONS}")
