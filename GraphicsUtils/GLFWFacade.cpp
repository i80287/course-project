#include "GLFWFacade.hpp"

#include <cstdio>
#include <stdexcept>

namespace AppSpace::GraphicsUtils {

/// @brief Class `GLFWwindowHolder` depends on `GLFWInitializer`
///         so order of the fields initialization is very important here.
///        To mark this, we explicitly call GLFWInitializer ctor.
/// @param window_width
/// @param window_height
/// @param window_title
GLFWFacade::GLFWFacade(int window_width, int window_height,
                       const char* window_title)
    : initializer_(),
      window_holder_(window_width, window_height, window_title) {
    glfwMakeContextCurrent(GetWindow());
    glfwSwapInterval(kGLFWSwapInterval);
    if (!TryLoadGLFW()) {
        throw std::runtime_error("Failed to initialize OpenGL context");
    }
    glfwMaximizeWindow(GetWindow());
}

GLFWFacade::WindowSize GLFWFacade::GetWindowSize() const noexcept {
    int display_w = 0;
    int display_h = 0;
    glfwGetFramebufferSize(window_holder_.GetWindow(), &display_w, &display_h);
    return {.width = display_w, .height = display_h};
}

bool GLFWFacade::ShouldClose() const noexcept {
    auto window = GetWindow();
    assert(window != nullptr);
    // glfwWindowShouldClose returns int in C-style
    //  but it is actually bool.
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
    const auto [display_w, display_h] = GetWindowSize();
    assert(glViewport);
    glViewport(0, 0, display_w, display_h);

    assert(glClearColor);
    glClearColor(ScreenParams::kScreenClearColorRed,
                 ScreenParams::kScreenClearColorGreen,
                 ScreenParams::kScreenClearColorBlue,
                 ScreenParams::kScreenClearColorAlpha);
    assert(glClear);
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

GLFWFacade::GLFWwindowHolder::GLFWwindowHolder(int width, int height,
                                               const char* title)
    : window_(glfwCreateWindow(width, height, title, nullptr, nullptr)) {
    if (window_ == nullptr) {
        throw std::runtime_error("Unable to create window");
    }
}

GLFWFacade::GLFWwindowHolder::~GLFWwindowHolder() {
    glfwDestroyWindow(window_);
}

bool GLFWFacade::TryLoadGLFW() const noexcept {
    // This type of loading should be used according
    //  to the documentation on
    //  https://www.khronos.org/opengl/wiki/OpenGL_Loading_Library
    return gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
}

}  // namespace AppSpace::GraphicsUtils
