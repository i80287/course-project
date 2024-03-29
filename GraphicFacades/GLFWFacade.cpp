#include "GLFWFacade.hpp"

#include <cstdio>
#include <stdexcept>

namespace AppSpace::GraphicFacades {

GLFWFacade::WindowManager::WindowManager(int width, int height,
                                         const char* title)
    : window_(glfwCreateWindow(width, height, title, nullptr, nullptr)) {
    if (window_ == nullptr) {
        throw std::runtime_error("Unable to create window");
    }
}

GLFWFacade::WindowManager::~WindowManager() {
    glfwDestroyWindow(window_);
}

void GLFWFacade::GLFWInitializer::GlfwErrorCallback(
    int error, const char* description) noexcept {
    // Here we use low level fprintf instead of std::cerr
    //  because this function can be called from the C library
    //  and thus we are not allowed to throw exceptions.
    std::fprintf(stderr, "Glfw Error %d: %s\n", error,
                 description ? description : "unknown error");
}

GLFWFacade::GLFWInitializer::GLFWInitializer() {
    glfwSetErrorCallback(GlfwErrorCallback);
    if (!glfwInit()) {
        throw std::runtime_error("Unable to init glfw");
    }
}

GLFWFacade::GLFWInitializer::~GLFWInitializer() {
    glfwTerminate();
}

GLFWFacade::GLFWFacade(int window_width, int window_height,
                       const char* window_title)
    : initializer_(),
      window_manager_(window_width, window_height, window_title) {
    glfwMakeContextCurrent(GetWindow());
    constexpr int kSwapInterval = 1;
    glfwSwapInterval(kSwapInterval);
    if (!TryLoadGLFW()) {
        throw std::runtime_error("Failed to initialize OpenGL context");
    }
}

bool GLFWFacade::TryLoadGLFW() const noexcept {
    // This type of loading should be used according
    //  to the documentation on
    //  https://www.khronos.org/opengl/wiki/OpenGL_Loading_Library
    return gladLoadGLLoader(
               reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) &&
           glViewport != nullptr && glClearColor != nullptr &&
           glClear != nullptr;
}

}  // namespace AppSpace::GraphicFacades
