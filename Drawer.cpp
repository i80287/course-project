#include "Drawer.hpp"

#include <imgui.h>

#include <cassert>
#include <cstdio>
#include <vector>

#include "ImGuiExtensions.hpp"

namespace AppSpace {

static void GlfwErrorCallback(int error, const char* description) noexcept {
    fprintf(stderr, "Glfw Error %d: %s\n", error,
            description ? description : "unknown error");
}

Drawer::Drawer()
    : port_(
          [this](SubscribeDataType data) {
              this->actrie_nodes_ = data.first;
              this->actrie_words_lengths_ = data.second;
          },
          [this](NotifyDataType data) {
              // TODO:
          },
          [this](UnsubscribeDataType data) {
              // TODO:
          }) {
    // Setup window
    glfwSetErrorCallback(GlfwErrorCallback);
    if (!glfwInit()) {
        OnError("Unable to init glfw");
        return;
    }

    GLFWwindow* window = glfwCreateWindow(
        1280, 720, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
    if (window == nullptr) {
        OnError("Unable to create window");
        return;
    }

    window_ = window;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // Enable vsync

    // tie window contex to glad's
    if (!gladLoadGL() || glViewport == nullptr || glClearColor == nullptr ||
        glClear == nullptr) {
        OnError("Unable to context to OpenGL");
        return;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // ImGuiIO& io = ImGui::GetIO();
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable
    // Keyboard Controls io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; //
    // Enable Gamepad Controls

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    // Setup ImGui style
    ImGui::StyleColorsDark();
}

Drawer::~Drawer() {
    // Cleanup
    try {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    } catch (...) {
    }
    if (window_)
        glfwDestroyWindow(window_);
    glfwTerminate();
}

bool Drawer::ShouldClose() const noexcept {
    return bool(glfwWindowShouldClose(window_));
}

Drawer::ObserverType* Drawer::GetObserverPort() noexcept { return &port_; }

void Drawer::Render() {
    constexpr ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Poll and handle events (inputs, window resize, etc.)
    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    UpdateState();

    GLFWwindow* window = window_;
    int display_w = 0, display_h = 0;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    // Render dear imgui into screen
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwMakeContextCurrent(window);
    glfwSwapBuffers(window);
}

void Drawer::UpdateState() {
    ImGui::Begin("wnd");

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    // TODO: this should be used for node drawing
    // static std::vector<ImVec2> points;

    // Count of the nodes without fake nodes
    size_t real_nodes_count = actrie_nodes_->size() - ModelType::kRootIndex;
    ImGui::Text("AC Trie with %zu nodes", real_nodes_count);

    // if (ImGui::Button("Clear")) {
    //     points.clear();
    // }
    // if (points.size() >= 2) {
    //     ImGui::SameLine();
    //     if (ImGui::Button("Undo")) {
    //         points.pop_back();
    //         points.pop_back();
    //     }
    // }

    ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
    ImVec2 canvas_size = ImGui::GetContentRegionAvail();
    if (canvas_size.x < 50.0f)
        canvas_size.x = 50.0f;
    if (canvas_size.y < 50.0f)
        canvas_size.y = 50.0f;
    ImVec2 canvas_end_pos = ImVecAdd(canvas_pos, canvas_size);

    constexpr ImU32 kGrayColor = IM_COL32(50, 50, 50, 255);
    constexpr ImU32 kRedColor = IM_COL32(250, 60, 50, 255);
    constexpr ImU32 kWhiteColor = IM_COL32(255, 255, 255, 255);
    draw_list->AddRectFilled(canvas_pos, canvas_end_pos, kGrayColor);
    draw_list->AddRect(canvas_pos, canvas_end_pos, kWhiteColor);

    const auto& root_node = actrie_nodes_->operator[](ModelType::kRootIndex);

    ImVec2 circle_center = ImVecMiddle(canvas_pos, canvas_end_pos);
    // TODO: replace magic numbers with constants
    draw_list->AddCircle(circle_center, 36, IM_COL32(120, 80, 40, 255), 0, 3);
    std::string word_index_str =
        std::to_string(root_node.IsTerminal() ? root_node.word_index : 0);
    draw_list->AddText(circle_center, kRedColor, word_index_str.data(),
                       word_index_str.data() + word_index_str.size());

    ImGui::End();
}

void Drawer::OnError(std::string_view msg) noexcept {
    puts(msg.data());
}

}  // namespace AppSpace
