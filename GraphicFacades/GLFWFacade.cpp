#include "GLFWFacade.hpp"

#include <cstdio>
#include <stdexcept>

namespace AppSpace::GraphicFacades {

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

bool GLFWFacade::ShouldClose() const noexcept {
    auto window = GetWindow();
    assert(window != nullptr);
    return static_cast<bool>(glfwWindowShouldClose(window));
}

void GLFWFacade::PollEvents() const noexcept {
    assert(GetWindow() != nullptr);
    glfwPollEvents();
}

void GLFWFacade::UpdateWindowContext() noexcept {
    auto window = GetWindow();
    assert(window != nullptr);
    glfwMakeContextCurrent(window);
    glfwSwapBuffers(window);
}

void GLFWFacade::ClearWindow() const noexcept {
    int display_w = 0;
    int display_h = 0;
    glfwGetFramebufferSize(window_manager_.GetWindow(), &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);

    constexpr float kScreenClearColorRed   = 0.45f;
    constexpr float kScreenClearColorGreen = 0.55f;
    constexpr float kScreenClearColorBlue  = 0.60f;
    constexpr float kScreenClearColorAlpha = 1.00f;
    glClearColor(kScreenClearColorRed, kScreenClearColorGreen,
                 kScreenClearColorBlue, kScreenClearColorAlpha);
    glClear(GL_COLOR_BUFFER_BIT);
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
