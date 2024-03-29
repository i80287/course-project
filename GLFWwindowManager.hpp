#pragma once

#include <cstdio>
#include <stdexcept>

#include "glad_fix.hpp"

namespace AppSpace {

class GLFWwindowManager final {
public:
    GLFWwindowManager(int width, int height, const char* title = "app") {
        glfwSetErrorCallback(GlfwErrorCallback);
        if (!glfwInit()) {
            throw std::runtime_error("Unable to init glfw");
        }
        window_ = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (window_ == nullptr) {
            throw std::runtime_error("Unable to create window");
        }
        if (TryLoadGLFW()) {
            this->~GLFWwindowManager();
            throw std::runtime_error("Unable to context to OpenGL");
        }
    }

    ~GLFWwindowManager() {
        if (window_)
            glfwDestroyWindow(window_);
        glfwTerminate();
    }

    constexpr GLFWwindow* Get() const noexcept {
        return window_;
    }

private:
    static void GlfwErrorCallback(int error,
                                  const char* description) noexcept {
        // Here we use low level fprintf instead of std::cerr
        //  because this function can be called from the C library
        //  and thus we are not allowed to throw exceptions.
        std::fprintf(stderr, "Glfw Error %d: %s\n", error,
                     description ? description : "unknown error");
    }

    bool TryLoadGLFW() const noexcept {
        glfwMakeContextCurrent(window_);
        glfwSwapInterval(1);  // Enable vsync
        return gladLoadGL() && glViewport != nullptr &&
               glClearColor != nullptr && glClear != nullptr;
    }

    GLFWwindow* window_ = nullptr;
};

}  // namespace AppSpace
