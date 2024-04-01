#include "Drawer.hpp"

#include <imgui.h>

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

#include "DrawerUtils/Logger.hpp"
#include "ImGuiExtensions.hpp"
#include "ImGuiFacade.hpp"

namespace AppSpace::GraphicsUtils {

using DrawerUtils::logger;
using namespace ImGuiExtensions;

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
    model_nodes_.reserve(ACTrieModel::kInitialNodesCount);
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

Drawer& Drawer::AddACTrieResetSubscriber(ACTrieResetObserver* observer) {
    actrie_reset_port_.Subscribe(observer);
    return *this;
}

void Drawer::OnNewFrame() {
    HandleNextEvent();
    Draw();
}

void Drawer::HandleNextEvent() {
    if (!events_.empty()) {
        EventType event = std::move(events_.front());
        events_.pop_front();
        switch (event.index()) {
            case kUpdateNodeEventIndex:
                HandleNodeUpdate(std::get<kUpdateNodeEventIndex>(event));
                break;
            case kFoundSubstringEventIndex:
                HandleFoundSubstring(
                    std::get<kFoundSubstringEventIndex>(event));
                break;
            case kBadInputPatternEventIndex:
                HandleBadPatternInput(
                    std::get<kBadInputPatternEventIndex>(event));
                break;
            default:
                assert(false);
                break;
        }
    }
}

void Drawer::SetupImGuiStyle() {
    ImGui::StyleColorsDark();
}

void Drawer::OnUpdatedNode(UpdatedNodeInfoPassBy updated_node_info) {
    events_.emplace_back(
        std::forward<UpdatedNodeInfoPassBy>(updated_node_info));
}

void Drawer::OnFoundSubstring(FoundSubstringInfoPassBy substring_info) {
    events_.emplace_back(std::forward<FoundSubstringInfo>(substring_info));
}

void Drawer::OnBadPatternInput(BadInputPatternInfoPassBy bad_input_info) {
    events_.emplace_back(
        std::forward<BadInputPatternInfoPassBy>(bad_input_info));
}

void Drawer::HandleNodeUpdate(const UpdatedNodeInfo& updated_node_info) {
    const auto node_index        = updated_node_info.node_index;
    const auto node_parent_index = updated_node_info.node_parent_index;
    if (node_index != ACTrieModel::kNullNodeIndex) {
        assert(node_parent_index < node_index);
        assert(node_parent_index < model_nodes_.size());
    }

    switch (updated_node_info.status) {
        case ACTrieModel::UpdatedNodeStatus::kAdded: {
            model_nodes_.emplace_back(updated_node_info.node);
            assert(node_index == model_nodes_.size() - 1);
            nodes_status_.emplace(
                node_index,
                NodeState{
                    .node_parent_index = node_parent_index,
                    .status            = NodeStatus::kAdded,
                    .parent_to_node_edge_symbol =
                        updated_node_info.parent_to_node_edge_symbol,
                });
            logger.DebugLog("Added new node");
        } break;
        case ACTrieModel::UpdatedNodeStatus::kSuffixLinksComputed: {
            auto node_iter = nodes_status_.find(node_index);
            assert(node_iter != nodes_status_.end());
            node_iter->second.status = NodeStatus::kSuffixLinksComputed;
            logger.DebugLog("Updated suffix links status for node");
        } break;
        default:
            assert(false);
            break;
    }
}

void Drawer::HandleFoundSubstring(const FoundSubstringInfo& updated_node_info) {
    printf("Substring found:\n");
}

void Drawer::HandleBadPatternInput(
    const BadInputPatternInfo& updated_node_info) {
    printf("Bad symbol '%c'\n", updated_node_info.bad_symbol);
}

void Drawer::Draw() {
    ImGui::ShowDemoWindow();

    if (disable_window_rounding_) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    }

    ImGuiWindowFlags window_begin_flags = ImGuiWindowFlags_None;
    if (no_resize_) {
        window_begin_flags |= ImGuiWindowFlags_NoResize;
    }
    if (no_decoration_) {
        window_begin_flags |= ImGuiWindowFlags_NoDecoration;
    }
    ImGui::Begin("Visualization App Main Window", nullptr, window_begin_flags);

    const ImVec2 canvas_screen_pos      = ImGui::GetCursorScreenPos();
    const ImVec2 available_size         = ImGui::GetContentRegionAvail();
    const ImVec2 end_of_available_space = canvas_screen_pos + available_size;
    const auto [available_width, available_height] = available_size;
    const ImVec2 canvas_size(
        available_width * CanvasConstants::kTreeDrawingCanvasScaleX,
        available_height * CanvasConstants::kTreeDrawingCanvasScaleY);
    const ImVec2 canvas_end_pos = canvas_screen_pos + canvas_size;
    const float tree_indent_width =
        available_width * CanvasConstants::kTreeDrawingCanvasIndentScaleX;
    const ImVec2 io_blocks_start_pos(canvas_end_pos.x + tree_indent_width,
                                     canvas_screen_pos.y);
    DrawACTrieTree(canvas_screen_pos, canvas_end_pos);
    DrawIOBlocks(io_blocks_start_pos, end_of_available_space);

    ImGui::End();
    if (disable_window_rounding_) {
        ImGui::PopStyleVar(1);
    }
}

void Drawer::DrawACTrieTree(ImVec2 canvas_screen_pos, ImVec2 canvas_end_pos) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRectFilled(canvas_screen_pos, canvas_end_pos,
                             Palette::AsImU32::kGrayColor);
    draw_list->AddRect(canvas_screen_pos, canvas_end_pos,
                       Palette::AsImU32::kWhiteColor);
}

void Drawer::DrawIOBlocks(ImVec2 io_blocks_start_pos,
                          ImVec2 end_of_available_space) {
    float parts_width =
        (end_of_available_space.x - io_blocks_start_pos.x -
        end_of_available_space.x * CanvasConstants::kIOBlocksIndentScaleX) / 2;
    ImGui::SetCursorScreenPos(io_blocks_start_pos);
    const ImVec2 pattern_block_end_pos(io_blocks_start_pos.x + parts_width,
                                       end_of_available_space.y);
    DrawPatternInput(io_blocks_start_pos, pattern_block_end_pos);
}

void Drawer::DrawPatternInput(ImVec2 io_block_start_pos,
                              ImVec2 io_block_end_pos) {
    const ImVec2 io_block_size = io_block_end_pos - io_block_start_pos;
    ImGui::BeginChild("PatternInputBar", io_block_size);

    if (ImGui::SmallButton("Clear data structure")) {
        ClearStateAndNotify();
    }
    ImGui::SameLine();
    const bool copy_to_clipboard = ImGui::SmallButton("Copy");
    const float footer_height_to_reserve =
        ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    const ImVec2 scrolling_region_size(
        io_block_size.x, io_block_size.y - footer_height_to_reserve * 2);
    ImGui::BeginChild(
        "ScrollingRegion", scrolling_region_size,
        ImGuiChildFlags_Border,
        ImGuiWindowFlags_HorizontalScrollbar);

    if (ImGui::BeginPopupContextWindow()) {
        if (ImGui::Selectable("Clear data structure")) {
            ClearStateAndNotify();
        }
        ImGui::EndPopup();
    }

    // ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
    //                     ImVec2(4, 1));  // Tighten spacing

    if (copy_to_clipboard) {
        ImGui::LogToClipboard();
    }
    for (const std::string& item : patterns_input_history_) {
        ImVec4 color = Palette::AsImColor::kRedColor;
        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::TextUnformatted(item.data(), item.data() + item.size());
        ImGui::PopStyleColor();
    }
    if (copy_to_clipboard) {
        ImGui::LogFinish();
    }

    // Keep up at the bottom of the scroll region if we were already at the
    // bottom at the beginning of the frame. Using a scrollbar or
    // mouse-wheel will take away from the bottom edge.
    if (scroll_to_bottom_ ||
        (auto_scroll_ && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
        ImGui::SetScrollHereY(1.0f);
    scroll_to_bottom_ = false;

    // ImGui::PopStyleVar();
    // ImGui::Separator();
    ImGui::EndChild();
    bool reclaim_focus = AddPatternTextInput(scrolling_region_size.x);
    ImGui::SetItemDefaultFocus();
    // Auto-focus on window apparition
    if (reclaim_focus) {
        ImGui::SetKeyboardFocusHere(-1);
    }

    ImGui::EndChild();
}

void Drawer::DrawTextInput(ImVec2 io_blocks_start_pos,
                           float footer_height_to_reserve) {
    assert(false);
}

bool Drawer::AddPatternTextInput(float text_input_width) {
    constexpr ImGuiInputTextFlags input_text_flags =
        ImGuiInputTextFlags_EnterReturnsTrue |
        ImGuiInputTextFlags_EscapeClearsAll |
        ImGuiInputTextFlags_CallbackCompletion |
        ImGuiInputTextFlags_CallbackHistory;

    ImGui::PushItemWidth(text_input_width);
    bool user_added_text = ImGui::InputText(
        "Pattern input", pattern_input_buffer_.data(),
        pattern_input_buffer_.size(), input_text_flags,
        [](ImGuiInputTextCallbackData* data) {
            assert(data);
            Drawer* this_drawer = static_cast<Drawer*>(data->UserData);
            assert(this_drawer);
            this_drawer->TextEditCallback(*data);
            return 0;
        },
        reinterpret_cast<void*>(this));
    if (user_added_text) {
        std::string_view pattern =
            patterns_input_history_.TrimSpaces(pattern_input_buffer_.data());
        if (!pattern.empty()) {
            patterns_input_history_.AddString(pattern);
            user_pattern_input_port_.Notify(pattern);
        }
        ClearPatternInputBuffer();
    }
    ImGui::PopItemWidth();
    return user_added_text;
}

void Drawer::TextEditCallback(ImGuiInputTextCallbackData& data) {
    if (data.EventFlag != ImGuiInputTextFlags_CallbackHistory ||
        patterns_input_history_.Empty()) {
        return;
    }

    std::string_view text;
    switch (data.EventKey) {
        case ImGuiKey_UpArrow:
            text = patterns_input_history_.MoveToPreviousInputAndRead();
            break;
        case ImGuiKey_DownArrow:
            text = patterns_input_history_.MoveToNextInputAndRead();
            break;
        default:
            return;
    }
    data.DeleteChars(0, data.BufTextLen);
    data.InsertChars(0, text.begin(), text.end());
}

void Drawer::ClearStateAndNotify() {
    events_.clear();
    model_nodes_.clear();
    nodes_status_.clear();
    pattern_input_buffer_.fill('\0');
    patterns_input_history_.Clear();
    ClearPatternInputBuffer();
    actrie_reset_port_.Notify();
    logger.DebugLog("Cleared actrie");
}

void Drawer::ClearPatternInputBuffer() noexcept {
    pattern_input_buffer_[0] = '\0';
}

}  // namespace AppSpace::GraphicsUtils
