#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <concepts>

#include "GLFWFacade.hpp"

namespace AppSpace::GraphicsUtils {

class ImGuiFacade final {
public:
    static constexpr int kDefaultWindowWidth         = 1920;
    static constexpr int kDefaultWindowHeight        = 1080;
    static constexpr const char* kDefaultWindowTitle = "visualization app";

    ImGuiFacade(int window_width         = kDefaultWindowWidth,
                int window_height        = kDefaultWindowHeight,
                const char* window_title = kDefaultWindowTitle);
    ImGuiFacade(const ImGuiFacade&)            = delete;
    ImGuiFacade& operator=(const ImGuiFacade&) = delete;
    ImGuiFacade(ImGuiFacade&&)                 = delete;
    ImGuiFacade& operator=(ImGuiFacade&&)      = delete;
    /// @brief This function will be called on every iteration of the runtime
    /// loop.
    //
    //  This function is intentionally made with template argument
    //   in order to help compiler inline it.
    //  We do so instead of std::function<void()> because this is
    //   the hot-path of the program.
    /// @param new_frame_code
    void StartRuntimeLoop(auto new_frame_code) {
        while (!glfw_facade_.ShouldClose()) {
            RenderRuntimeLoopIteration(new_frame_code);
        }
    }

private:
    void RenderRuntimeLoopIteration(auto new_frame_code) {
        glfw_facade_.PollEvents();
        imgui_binder_.NewFrame();
        ImGui::NewFrame();
        new_frame_code();
        ImGui::Render();
        glfw_facade_.ClearWindow();
        imgui_binder_.RenderDrawData();
        glfw_facade_.UpdateWindowContext();
    }

    class ImGuiContextManager final {
    public:
        ImGuiContextManager();
        ~ImGuiContextManager();
        static void EnableKeyboardControls();
    };

    class ImGuiBinder final {
    public:
        ImGuiBinder(const GLFWFacade& glfw_facade);
        ~ImGuiBinder();
        static void NewFrame();
        static void RenderDrawData();
    };

    /// @brief Class `ImGuiBinder` depends on class `ImGuiContextManager`
    ///         which depends on class `GLFWFacade`, so order of the
    ///         fields is very important here.
    GLFWFacade glfw_facade_;
    ImGuiContextManager context_manager_;
    ImGuiBinder imgui_binder_;
};

}  // namespace AppSpace::GraphicsUtils
