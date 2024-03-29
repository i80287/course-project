#pragma once

#include <cassert>

#include "GladFix.hpp"

namespace AppSpace::GraphicFacades {

class GLFWFacade final {
public:
    GLFWFacade(int window_width         = kDefaultWindowWidth,
               int window_height        = kDefaultWindowHeight,
               const char* window_title = kDefaultWindowTitle);

    constexpr GLFWwindow* GetWindow() const noexcept;
    bool ShouldClose() const noexcept;
    void PollEvents() const noexcept;
    void UpdateWindowContext() noexcept;
    void ClearWindow() const noexcept;

private:
    static constexpr int kDefaultWindowWidth         = 1280;
    static constexpr int kDefaultWindowHeight        = 720;
    static constexpr const char* kDefaultWindowTitle = "visualization app";

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

    class WindowManager final {
    public:
        WindowManager(int width, int height, const char* title);
        ~WindowManager();
        WindowManager(const WindowManager&)            = delete;
        WindowManager(WindowManager&&)                 = delete;
        WindowManager& operator=(const WindowManager&) = delete;
        WindowManager& operator=(WindowManager&&)      = delete;

        constexpr GLFWwindow* GetWindow() const noexcept {
            return window_;
        }

    private:
        GLFWwindow* window_;
    };

    bool TryLoadGLFW() const noexcept;

    GLFWInitializer initializer_;
    WindowManager window_manager_;
};

constexpr GLFWwindow* GLFWFacade::GetWindow() const noexcept {
    return window_manager_.GetWindow();
}

}  // namespace AppSpace::GraphicFacades
