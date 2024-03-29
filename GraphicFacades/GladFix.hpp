#pragma once

/**
 * It is a helper header file that prevents clang-format
 * from reodering the two includes described below.
 * (if they are written sequentially, they will be reodered by the clang-format)
 */

/**
 * About OpenGL function loaders: modern OpenGL doesn't have
 * a standard header file and requires individual function pointers
 * to be loaded manually. Helper libraries are often used for this purpose.
 * Here we use glad.
 */
#include <glad/glad.h>

/**
 * Then we include glfw3.h after our OpenGL definitions
 */
#include <GLFW/glfw3.h>
