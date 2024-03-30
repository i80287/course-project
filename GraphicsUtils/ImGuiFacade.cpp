#include "ImGuiFacade.hpp"

#include <stdexcept>

namespace AppSpace::GraphicsUtils {

ImGuiFacade::ImGuiFacade()
    : glfw_facade_{}, context_manager_{}, imgui_binder_(glfw_facade_) {}

ImGuiFacade::ImGuiContextManager::ImGuiContextManager() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // Enable Keyboard Controls
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
}

ImGuiFacade::ImGuiContextManager::~ImGuiContextManager() {
    ImGui::DestroyContext();
}

ImGuiFacade::ImGuiBinder::ImGuiBinder(GLFWFacade& glfw_facade) {
    // Setup Platform/Renderer bindings
    if (!ImGui_ImplGlfw_InitForOpenGL(glfw_facade.GetWindow(), true) ||
        !ImGui_ImplOpenGL3_Init()) {
        throw std::runtime_error(
            "Unable to bind imgui context to glfw opengl3 context");
    }
}

ImGuiFacade::ImGuiBinder::~ImGuiBinder() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
}

}  // namespace AppSpace::GraphicsUtils
