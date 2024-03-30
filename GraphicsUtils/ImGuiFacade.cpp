#include "ImGuiFacade.hpp"

#include <stdexcept>

namespace AppSpace::GraphicsUtils {

ImGuiFacade::ImGuiFacade(int window_width, int window_height,
                         const char* window_title)
    : glfw_facade_(window_width, window_height, window_title),
      context_manager_(),
      imgui_binder_(glfw_facade_) {}

ImGuiFacade::ImGuiContextManager::ImGuiContextManager() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    EnableKeyboardControls();
}

ImGuiFacade::ImGuiContextManager::~ImGuiContextManager() {
    ImGui::DestroyContext();
}

void ImGuiFacade::ImGuiContextManager::EnableKeyboardControls() {
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
}

ImGuiFacade::ImGuiBinder::ImGuiBinder(const GLFWFacade& glfw_facade) {
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
