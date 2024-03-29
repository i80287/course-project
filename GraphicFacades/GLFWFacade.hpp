#pragma once

#include <cassert>

#include "GladFix.hpp"

namespace AppSpace::GraphicFacades {

class GLFWFacade final {
public:
    GLFWFacade(int window_width         = kDefaultWindowWidth,
               int window_height        = kDefaultWindowHeight,
               const char* window_title = kDefaultWindowTitle);

    constexpr GLFWwindow* GetWindow() noexcept {
        return window_manager_.GetWindow();
    }

    bool ShouldClose() noexcept {
        auto window = GetWindow();
        assert(window != nullptr);
        return static_cast<bool>(glfwWindowShouldClose(window));
    }

    void PollEvents() noexcept {
        assert(GetWindow() != nullptr);
        // Poll and handle events (inputs, window resize, etc.)
        glfwPollEvents();
    }

    void UpdateWindowContext() noexcept {
        auto window = GetWindow();
        assert(window != nullptr);
        glfwMakeContextCurrent(window);
        glfwSwapBuffers(window);
    }

    void ClearWindow() noexcept {
        int display_w = 0;
        int display_h = 0;
        glfwGetFramebufferSize(window_manager_.GetWindow(), &display_w,
                               &display_h);
        glViewport(0, 0, display_w, display_h);

        constexpr float kScreenClearColorRed   = 0.45f;
        constexpr float kScreenClearColorGreen = 0.55f;
        constexpr float kScreenClearColorBlue  = 0.60f;
        constexpr float kScreenClearColorAlpha = 1.00f;
        glClearColor(kScreenClearColorRed, kScreenClearColorGreen,
                     kScreenClearColorBlue, kScreenClearColorAlpha);
        glClear(GL_COLOR_BUFFER_BIT);
    }

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

        constexpr GLFWwindow* GetWindow() noexcept {
            return window_;
        }

    private:
        GLFWwindow* window_;
    };

    bool TryLoadGLFW() const noexcept;

    GLFWInitializer initializer_;
    WindowManager window_manager_;
};

}  // namespace AppSpace::GraphicFacades
