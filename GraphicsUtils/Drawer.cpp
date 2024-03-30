#include "Drawer.hpp"

#include <imgui.h>

#include <cassert>
#include <cstdio>
#include <vector>

#include "ImGuiExtensions.hpp"
#include "ImGuiFacade.hpp"

namespace AppSpace {

Drawer::Drawer()
    : updated_node_port_([this](UpdatedNodeInfoPassBy updated_node_info) {
          OnUpdatedNode(std::forward<UpdatedNodeInfoPassBy>(updated_node_info));
      }),
      found_string_port_([this](FoundSubstringInfoPassBy substring_info) {
          OnFoundSubstring(
              std::forward<FoundSubstringInfoPassBy>(substring_info));
      }),
      bad_input_port_([this](BadInputPatternInfoPassBy bad_input_info) {
          OnBadPatternInput(
              std::forward<BadInputPatternInfoPassBy>(bad_input_info));
      }) {
    SetupImGuiStyle();
}

Drawer::UpdatedNodeObserver* Drawer::GetUpdatedNodeObserverPort() noexcept {
    return &updated_node_port_;
}

Drawer::FoundSubstringObserver* Drawer::GetFoundStringObserverPort() noexcept {
    return &found_string_port_;
}

Drawer::BadInputPatternObserver*
Drawer::GetBadInputPatternObserverPort() noexcept {
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

void Drawer::OnNewFrame() {
    if (!events_.empty()) {
        // We copy event instead of moving because
        //  clang-tidy suggests so.
        //  [std::move of the expression of the trivially-copyable type]
        //  [performance-move-const-arg]
        EventType event = events_.front();
        events_.pop();
        switch (event.index()) {
            case kUpdateNodeEventIndex:
                std::get<kUpdateNodeEventIndex>(event).node.get();
                break;
            case kFoundSubstringEventIndex:
                std::get<kUpdateNodeEventIndex>(event).node.get();
                break;
            case kBadInputPatternEventIndex:
                std::get<kUpdateNodeEventIndex>(event).node.get();
                break;
        }
    }

    Draw();
}

void Drawer::SetupImGuiStyle() {
    ImGui::StyleColorsDark();
}

void Drawer::OnUpdatedNode(UpdatedNodeInfoPassBy updated_node_info) {
    events_.emplace(std::forward<UpdatedNodeInfoPassBy>(updated_node_info));
}

void Drawer::OnFoundSubstring(FoundSubstringInfoPassBy substring_info) {
    events_.emplace(std::forward<FoundSubstringInfo>(substring_info));
}

void Drawer::OnBadPatternInput(BadInputPatternInfoPassBy bad_input_info) {
    events_.emplace(std::forward<BadInputPatternInfoPassBy>(bad_input_info));
}

void Drawer::Draw() {
    using namespace ImGuiExtensions;

    ImGui::ShowDemoWindow();

    // ImGui::Begin("wnd");
    // ImDrawList* draw_list = ImGui::GetWindowDrawList();

    // // TODO: this should be used for node drawing
    // // static std::vector<ImVec2> points;

    // // Count of the nodes without fake nodes
    // // size_t real_nodes_count = actrie_nodes_->size() -
    // // ModelType::kRootIndex; ImGui::Text("AC Trie with %zu nodes",
    // // real_nodes_count);

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

    // ImVec2 canvas_pos     = ImGui::GetCursorScreenPos();
    // ImVec2 canvas_size    = ImGui::GetContentRegionAvail();
    // canvas_size.x         = std::max(canvas_size.x, 50.0f);
    // canvas_size.y         = std::max(canvas_size.y, 50.0f);
    // ImVec2 canvas_end_pos = ImVecAdd(canvas_pos, canvas_size);

    // constexpr ImU32 kGrayColor  = IM_COL32(50, 50, 50, 255);
    // constexpr ImU32 kRedColor   = IM_COL32(250, 60, 50, 255);
    // constexpr ImU32 kWhiteColor = IM_COL32(255, 255, 255, 255);
    // draw_list->AddRectFilled(canvas_pos, canvas_end_pos, kGrayColor);
    // draw_list->AddRect(canvas_pos, canvas_end_pos, kWhiteColor);

    // // // const auto& root_node =
    // // // actrie_nodes_->operator[](ModelType::kRootIndex);

    // // ImVec2 circle_center = ImVecMiddle(canvas_pos, canvas_end_pos);
    // // // TODO: replace magic numbers with constants
    // // draw_list->AddCircle(circle_center, 36, IM_COL32(120, 80, 40,
    // 255),
    // // 0, 3);
    // // // std::string word_index_str =
    // // std::to_string(root_node.IsTerminal() ?
    // // // root_node.word_index : 0); draw_list->AddText(circle_center,
    // // // kRedColor, word_index_str.data(),
    // // //                    word_index_str.data() +
    // // word_index_str.size());

    // ImGui::End();
}

}  // namespace AppSpace
