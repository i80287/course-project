#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "GLFWFacade.hpp"

namespace AppSpace::GraphicFacades {

class ImGuiFacade final {
public:
    ImGuiFacade();
    ImGuiFacade(const ImGuiFacade&)            = delete;
    ImGuiFacade(ImGuiFacade&&)                 = delete;
    ImGuiFacade& operator=(const ImGuiFacade&) = delete;
    ImGuiFacade& operator=(ImGuiFacade&&)      = delete;

    bool ShouldClose() noexcept {
        return static_cast<bool>(
            glfwWindowShouldClose(glfw_facade_.GetWindow()));
    }

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

private:
    class ImGuiContextManager {
    public:
        ImGuiContextManager();
        ~ImGuiContextManager();
        ImGuiContextManager(const ImGuiContextManager&) = delete;
        ImGuiContextManager(ImGuiContextManager&&)      = delete;
        ImGuiContextManager& operator=(const ImGuiContextManager&) =
            delete;
        ImGuiContextManager& operator=(ImGuiContextManager&&) = delete;
    };

    class ImGuiBinder {
    public:
        ImGuiBinder(GLFWFacade& glfw_facade);
        ~ImGuiBinder();
        ImGuiBinder(const ImGuiBinder&)            = delete;
        ImGuiBinder(ImGuiBinder&&)                 = delete;
        ImGuiBinder& operator=(const ImGuiBinder&) = delete;
        ImGuiBinder& operator=(ImGuiBinder&&)      = delete;

        void NewFrame() {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
        }

        void RenderDrawData() {
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }
    };

    GLFWFacade glfw_facade_;
    ImGuiContextManager context_manager_;
    ImGuiBinder imgui_binder_;
};

}  // namespace AppSpace::GraphicFacades
