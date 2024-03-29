#include "Drawer.hpp"

#include <imgui.h>

#include <cassert>
#include <cstdio>
#include <vector>

#include "ImGuiExtensions.hpp"

namespace AppSpace {

Drawer::Drawer()
    : window_(1280, 720),
      found_strings_port_(
          [this](FoundSubstringData) {

          },
          [this](FoundSubstringData) {
              // TODO:
          },
          [this](FoundSubstringData) {
              // TODO:
          }),
      bad_input_port_(
          [this](BadInputStringData) {

          },
          [this](BadInputStringData) {

          },
          [this](BadInputStringData) {

          }) {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // ImGuiIO& io = ImGui::GetIO();
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable
    // Keyboard Controls io.ConfigFlags |=
    // ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window_.Get(), true);
    ImGui_ImplOpenGL3_Init();

    // Setup ImGui style
    ImGui::StyleColorsDark();
}

Drawer::~Drawer() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

bool Drawer::ShouldClose() const noexcept {
    return bool(glfwWindowShouldClose(window_.Get()));
}

Drawer::FoundSubstringObserver*
Drawer::GetFoundStringsObserverPort() noexcept {
    return &found_strings_port_;
}

Drawer::BadInputObserver* Drawer::GetBadInputObserverPort() noexcept {
    return &bad_input_port_;
}

void Drawer::Render() {
    constexpr ImVec4 kClearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Poll and handle events (inputs, window resize, etc.)
    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    UpdateState();

    GLFWwindow* window = window_.Get();
    int display_w = 0, display_h = 0;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(kClearColor.x, kClearColor.y, kClearColor.z,
                 kClearColor.w);
    glClear(GL_COLOR_BUFFER_BIT);

    // Render dear imgui into screen
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwMakeContextCurrent(window);
    glfwSwapBuffers(window);
}

void Drawer::UpdateState() {
    // ImGui::Begin("wnd");

    // ImDrawList* draw_list = ImGui::GetWindowDrawList();

    // // TODO: this should be used for node drawing
    // // static std::vector<ImVec2> points;

    // // Count of the nodes without fake nodes
    // size_t real_nodes_count = actrie_nodes_->size() -
    // ModelType::kRootIndex; ImGui::Text("AC Trie with %zu nodes",
    // real_nodes_count);

    // // if (ImGui::Button("Clear")) {
    // //     points.clear();
    // // }
    // // if (points.size() >= 2) {
    // //     ImGui::SameLine();
    // //     if (ImGui::Button("Undo")) {
    // //         points.pop_back();
    // //         points.pop_back();
    // //     }
    // // }

    // ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
    // ImVec2 canvas_size = ImGui::GetContentRegionAvail();
    // if (canvas_size.x < 50.0f)
    //     canvas_size.x = 50.0f;
    // if (canvas_size.y < 50.0f)
    //     canvas_size.y = 50.0f;
    // ImVec2 canvas_end_pos = ImVecAdd(canvas_pos, canvas_size);

    // constexpr ImU32 kGrayColor = IM_COL32(50, 50, 50, 255);
    // constexpr ImU32 kRedColor = IM_COL32(250, 60, 50, 255);
    // constexpr ImU32 kWhiteColor = IM_COL32(255, 255, 255, 255);
    // draw_list->AddRectFilled(canvas_pos, canvas_end_pos, kGrayColor);
    // draw_list->AddRect(canvas_pos, canvas_end_pos, kWhiteColor);

    // const auto& root_node =
    // actrie_nodes_->operator[](ModelType::kRootIndex);

    // ImVec2 circle_center = ImVecMiddle(canvas_pos, canvas_end_pos);
    // // TODO: replace magic numbers with constants
    // draw_list->AddCircle(circle_center, 36, IM_COL32(120, 80, 40, 255),
    // 0, 3); std::string word_index_str =
    //     std::to_string(root_node.IsTerminal() ? root_node.word_index :
    //     0);
    // draw_list->AddText(circle_center, kRedColor, word_index_str.data(),
    //                    word_index_str.data() + word_index_str.size());

    // ImGui::End();
}

}  // namespace AppSpace
