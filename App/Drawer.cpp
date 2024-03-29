#include "Drawer.hpp"

#include <imgui.h>

#include <cassert>
#include <cstdio>
#include <vector>

#include "ImGuiExtensions.hpp"

namespace AppSpace {

Drawer::Drawer()
    : found_strings_port_([this](FoundSubstringSentBy substring_info) {
          OnFoundSubstring(substring_info);
      }),
      bad_input_port_([this](BadInputSentBy bad_input_info) {
          OnBadPatternInput(bad_input_info);
      }) {
    // Setup ImGui style
    SetupImGuiStyle();
}

void Drawer::SetupImGuiStyle() {
    ImGui::StyleColorsDark();
}

Drawer::FoundSubstringObserver* Drawer::GetFoundStringsObserverPort() noexcept {
    return &found_strings_port_;
}

Drawer::BadInputObserver* Drawer::GetBadInputObserverPort() noexcept {
    return &bad_input_port_;
}

Drawer& Drawer::AddPatternSubscriber(PatternObserver* observer) {
    user_pattern_input_port_.Subscribe(observer);
    return *this;
}

Drawer& Drawer::AddTextSubscriber(TextObserver* observer) {
    user_text_input_port_.Subscribe(observer);
    return *this;
}

void Drawer::Draw() {
    ImGui::Begin("wnd");

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    // TODO: this should be used for node drawing
    // static std::vector<ImVec2> points;

    // Count of the nodes without fake nodes
    // size_t real_nodes_count = actrie_nodes_->size() -
    // ModelType::kRootIndex; ImGui::Text("AC Trie with %zu nodes",
    // real_nodes_count);

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

    ImVec2 canvas_pos  = ImGui::GetCursorScreenPos();
    ImVec2 canvas_size = ImGui::GetContentRegionAvail();
    if (canvas_size.x < 50.0f)
        canvas_size.x = 50.0f;
    if (canvas_size.y < 50.0f)
        canvas_size.y = 50.0f;
    ImVec2 canvas_end_pos = ImVecAdd(canvas_pos, canvas_size);

    constexpr ImU32 kGrayColor  = IM_COL32(50, 50, 50, 255);
    constexpr ImU32 kRedColor   = IM_COL32(250, 60, 50, 255);
    constexpr ImU32 kWhiteColor = IM_COL32(255, 255, 255, 255);
    draw_list->AddRectFilled(canvas_pos, canvas_end_pos, kGrayColor);
    draw_list->AddRect(canvas_pos, canvas_end_pos, kWhiteColor);

    // const auto& root_node =
    // actrie_nodes_->operator[](ModelType::kRootIndex);

    ImVec2 circle_center = ImVecMiddle(canvas_pos, canvas_end_pos);
    // TODO: replace magic numbers with constants
    draw_list->AddCircle(circle_center, 36, IM_COL32(120, 80, 40, 255), 0, 3);
    // std::string word_index_str = std::to_string(root_node.IsTerminal() ?
    // root_node.word_index : 0); draw_list->AddText(circle_center,
    // kRedColor, word_index_str.data(),
    //                    word_index_str.data() + word_index_str.size());

    ImGui::End();
}

void Drawer::OnFoundSubstring(FoundSubstringSentBy substring_info) {}

void Drawer::OnBadPatternInput(BadInputSentBy bad_input_info) {}

}  // namespace AppSpace
