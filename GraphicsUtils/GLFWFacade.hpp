#pragma once

#include <cassert>

#include "GladFix.hpp"

namespace AppSpace::GraphicsUtils {

class GLFWFacade final {
public:
    GLFWFacade(int window_width, int window_height, const char* window_title);
    constexpr GLFWwindow* GetWindow() const noexcept;
    bool ShouldClose() const noexcept;
    void PollEvents() const noexcept;
    void UpdateWindowContext() noexcept;
    void ClearWindow() const noexcept;

private:
    static constexpr int kGLFWSwapInterval = 1;

    class GLFWInitializer final {
    public:
        GLFWInitializer();
        ~GLFWInitializer();
        GLFWInitializer(const GLFWInitializer&)            = delete;
        GLFWInitializer(GLFWInitializer&&)                 = delete;
        GLFWInitializer& operator=(const GLFWInitializer&) = delete;
        GLFWInitializer& operator=(GLFWInitializer&&)      = delete;

    private:
        static void GlfwErrorCallback(int error,
                                      const char* description) noexcept;
    };

    class GLFWwindowHolder final {
    public:
        GLFWwindowHolder(int width, int height, const char* title);
        ~GLFWwindowHolder();
        GLFWwindowHolder(const GLFWwindowHolder&)            = delete;
        GLFWwindowHolder(GLFWwindowHolder&&)                 = delete;
        GLFWwindowHolder& operator=(const GLFWwindowHolder&) = delete;
        GLFWwindowHolder& operator=(GLFWwindowHolder&&)      = delete;

        constexpr GLFWwindow* GetWindow() const noexcept {
            return window_;
        }

    private:
        GLFWwindow* window_;
    };

    bool TryLoadGLFW() const noexcept;

    /// @brief Class `GLFWwindowHolder` depends on class `GLFWInitializer`
    //          so order of the fields is very important here.
    GLFWInitializer initializer_;
    GLFWwindowHolder window_holder_;
};

constexpr GLFWwindow* GLFWFacade::GetWindow() const noexcept {
    return window_holder_.GetWindow();
}

}  // namespace AppSpace::GraphicsUtils
