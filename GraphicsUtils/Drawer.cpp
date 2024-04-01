#include "Drawer.hpp"

#include <imgui.h>

#include <cassert>
#include <cctype>
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

Drawer& Drawer::AddACTrieBuildSubscriber(ACTrieBuildObserver* observer) {
    actrie_build_port_.Subscribe(observer);
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

void Drawer::HandleFoundSubstring(
    const FoundSubstringInfo& found_substring_info) {
    logger.DebugLog("Found substring: ", found_substring_info.found_substring);
    found_words_.emplace_back(std::move(found_substring_info.found_substring));
}

void Drawer::HandleBadPatternInput(const BadInputPatternInfo& bad_symbol_info) {
    is_bad_symbol_found_ = true;
    bad_symbol_position_ = bad_symbol_info.symbol_index;
    bad_symbol_          = bad_symbol_info.bad_symbol;
    logger.DebugLog("Bad symbol: '", "'", bad_symbol_info.bad_symbol);
}

void Drawer::Draw() {
    ImGui::ShowDemoWindow();

    if (is_window_rounding_disabled_) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    }

    ImGuiWindowFlags window_begin_flags = ImGuiWindowFlags_None;
    if (is_no_resize_) {
        window_begin_flags |= ImGuiWindowFlags_NoResize;
    }
    if (is_no_decoration_) {
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
    const float text_input_height =
        available_height * CanvasConstants::kTextInputHeightScaleY;
    const float text_input_indent =
        available_height * CanvasConstants::kTextInputIndentScaleY;
    const ImVec2 text_input_start_pos(canvas_end_pos.x + tree_indent_width,
                                      canvas_screen_pos.y);
    const ImVec2 text_input_end_pos(end_of_available_space.x,
                                    text_input_start_pos.y + text_input_height);
    const ImVec2 io_blocks_start_pos(
        canvas_end_pos.x + tree_indent_width,
        canvas_screen_pos.y + text_input_height + text_input_indent);
    const ImVec2 io_blocks_end_pos = end_of_available_space;

    DrawACTrieTree(canvas_screen_pos, canvas_end_pos);
    DrawTextInputBlock(text_input_start_pos, text_input_end_pos);
    DrawIOBlocks(io_blocks_start_pos, io_blocks_end_pos);

    if (is_bad_symbol_found_ || bad_symbol_modal_opened_) {
        DrawBadSymbolWindow();
    }

    ImGui::End();
    if (is_window_rounding_disabled_) {
        ImGui::PopStyleVar(1);
    }
    if (is_clear_button_pressed_) {
        ClearStateAndNotify();
        is_clear_button_pressed_ = false;
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
                          ImVec2 io_blocks_end_pos) {
    const float blocks_indent =
        io_blocks_end_pos.x * CanvasConstants::kIOBlocksIndentScaleX;
    const float block_width =
        (io_blocks_end_pos.x - io_blocks_start_pos.x - blocks_indent) /
        CanvasConstants::kNumberOfIOBlocks;

    const ImVec2 pattern_block_start_pos = io_blocks_start_pos;
    const ImVec2 pattern_block_end_pos(io_blocks_start_pos.x + block_width,
                                       io_blocks_end_pos.y);
    const ImVec2 found_words_block_start_pos(
        pattern_block_start_pos.x + block_width + blocks_indent,
        pattern_block_start_pos.y);
    const ImVec2 found_words_block_end_pos = io_blocks_end_pos;
    DrawPatternInputBlock(pattern_block_start_pos, pattern_block_end_pos);
    DrawFoundWordsBlock(found_words_block_start_pos, found_words_block_end_pos);
}

void Drawer::DrawPatternInputBlock(ImVec2 block_start_pos,
                                   ImVec2 block_end_pos) {
    ImGui::SetCursorScreenPos(block_start_pos);
    const ImVec2 io_block_size = block_end_pos - block_start_pos;
    ImGui::BeginChild("PatternInputBlock", io_block_size);

    ImGui::TextUnformatted("Added patterns:");
    ImGui::SameLine();
    is_clear_button_pressed_ |= ImGui::SmallButton("Clear AC trie");
    ImGui::SameLine();
    const bool copy_to_clipboard = ImGui::SmallButton("Copy");
    const float footer_height_to_reserve =
        ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    const ImVec2 scrolling_region_size(
        io_block_size.x, io_block_size.y - footer_height_to_reserve * 2);
    ImGui::BeginChild("ScrollingRegion", scrolling_region_size,
                      ImGuiChildFlags_Border,
                      ImGuiWindowFlags_HorizontalScrollbar);

    if (ImGui::BeginPopupContextWindow()) {
        is_clear_button_pressed_ |= ImGui::Selectable("Clear AC trie");
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
    if (is_scroll_to_bottom_ ||
        (is_auto_scroll_ && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
        ImGui::SetScrollHereY(1.0f);
    is_scroll_to_bottom_ = false;

    // ImGui::PopStyleVar();
    // ImGui::Separator();
    ImGui::EndChild();
    if (!is_inputing_text_) {
        bool reclaim_focus =
            AddPatternInput(scrolling_region_size.x *
                            CanvasConstants::kPatternInputFieldWidthScaleX);
        ImGui::SetItemDefaultFocus();
        // Auto-focus on window apparition
        if (reclaim_focus) {
            ImGui::SetKeyboardFocusHere(-1);
        }
    }

    ImGui::EndChild();
}

void Drawer::DrawFoundWordsBlock(ImVec2 block_start_pos, ImVec2 block_end_pos) {
    ImGui::SetCursorScreenPos(block_start_pos);

    const ImVec2 io_block_size = block_end_pos - block_start_pos;
    const float footer_height_to_reserve =
        ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    const ImVec2 scrolling_region_size(
        io_block_size.x, io_block_size.y - footer_height_to_reserve * 2);

    ImGui::BeginChild("FoundWordsBlock", io_block_size);

    ImGui::TextUnformatted("Found words:");
    ImGui::BeginChild("FoundWordsScrollingRegion", scrolling_region_size,
                      ImGuiChildFlags_Border,
                      ImGuiWindowFlags_HorizontalScrollbar);

    const ImVec4 color = Palette::AsImColor::kGreenColor;
    ImGui::PushStyleColor(ImGuiCol_Text, color);
    for (const std::string& item : found_words_) {
        ImGui::TextUnformatted(item.data(), item.data() + item.size());
    }
    ImGui::PopStyleColor();

    ImGui::EndChild();
    ImGui::EndChild();
}

void Drawer::DrawTextInputBlock(ImVec2 block_start_pos, ImVec2 block_end_pos) {
    ImGui::SetCursorScreenPos(block_start_pos);
    if (is_inputing_text_) {
        const float text_input_box_width =
            (block_end_pos.x - block_start_pos.x) *
            CanvasConstants::kTextInputBoxWithScaleX;
        bool reclaim_focus = AddTextInput(text_input_box_width);
        ImGui::SetItemDefaultFocus();
        // Auto-focus on window apparition
        if (reclaim_focus) {
            ImGui::SetKeyboardFocusHere(-1);
        }
    } else {
        const ImVec2 button_size = (block_end_pos - block_start_pos) /
                                   CanvasConstants::kButtonDecreaseScale;
        is_inputing_text_ = ImGui::Button("Build AC trie", button_size);
        if (is_inputing_text_) {
            actrie_build_port_.Notify();
            logger.DebugLog("User built AC trie");
        }
    }
}

void Drawer::DrawBadSymbolWindow() {
    constexpr const char* kBadSymbolModalName = "Incorrect input";
    ImGui::OpenPopup(kBadSymbolModalName);
    ImGui::BeginPopupModal(kBadSymbolModalName);
    bad_symbol_modal_opened_ = true;
    if (is_bad_symbol_found_) {
        bad_pattern_ = patterns_input_history_.MoveAndPopLast();
    }
    ImGui::Text(
        "Incorrect symbol '%c' passed in the pattern %s at position %zu",
        bad_symbol_, bad_pattern_.c_str(), bad_symbol_position_);
    if (ImGui::Button("Close")) {
        bad_symbol_modal_opened_ = false;
        ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
    is_bad_symbol_found_ = false;
}

bool Drawer::AddPatternInput(float text_input_width) {
    constexpr ImGuiInputTextFlags input_text_flags =
        ImGuiInputTextFlags_EnterReturnsTrue |
        ImGuiInputTextFlags_EscapeClearsAll |
        ImGuiInputTextFlags_CallbackHistory;

    ImGui::PushItemWidth(text_input_width);
    bool user_added_text = ImGui::InputText(
        "Pattern input", pattern_input_buffer_.data(),
        pattern_input_buffer_.size(), input_text_flags,
        [](ImGuiInputTextCallbackData* data) {
            assert(data);
            Drawer* this_drawer = static_cast<Drawer*>(data->UserData);
            assert(this_drawer);
            this_drawer->PatternInputImGuiCallback(*data);
            return 0;
        },
        reinterpret_cast<void*>(this));
    if (user_added_text) {
        std::string_view pattern = TrimSpaces(pattern_input_buffer_.data());
        if (!pattern.empty()) {
            patterns_input_history_.AddString(pattern);
            user_pattern_input_port_.Notify(pattern);
            logger.DebugLog("User added string: ", pattern);
        }
        ClearPatternInputBuffer();
    }
    ImGui::PopItemWidth();
    return user_added_text;
}

bool Drawer::AddTextInput(float text_input_width) {
    constexpr ImGuiInputTextFlags input_text_flags =
        ImGuiInputTextFlags_EnterReturnsTrue |
        ImGuiInputTextFlags_EscapeClearsAll |
        ImGuiInputTextFlags_CallbackHistory;

    ImGui::PushItemWidth(text_input_width);
    bool user_added_text = ImGui::InputText(
        "Text input", text_input_buffer_.data(), text_input_buffer_.size(),
        input_text_flags,
        [](ImGuiInputTextCallbackData* data) {
            assert(data);
            Drawer* this_drawer = static_cast<Drawer*>(data->UserData);
            assert(this_drawer);
            this_drawer->TextInputImGuiCallback(*data);
            return 0;
        },
        reinterpret_cast<void*>(this));
    if (user_added_text) {
        std::string_view input_text = TrimSpaces(text_input_buffer_.data());
        if (!input_text.empty()) {
            texts_input_history_.AddString(input_text);
            user_text_input_port_.Notify(input_text);
            logger.DebugLog("User requested a search in text: ", input_text);
        }
        ClearTextInputBuffer();
    }
    ImGui::PopItemWidth();
    return user_added_text;
}

void Drawer::PatternInputImGuiCallback(ImGuiInputTextCallbackData& data) {
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
    logger.DebugLog("Updated pattern input box");
}

void Drawer::TextInputImGuiCallback(ImGuiInputTextCallbackData& data) {
    logger.DebugLog("TEST");
    if (data.EventFlag != ImGuiInputTextFlags_CallbackHistory ||
        texts_input_history_.Empty()) {
        return;
    }

    std::string_view text;
    switch (data.EventKey) {
        case ImGuiKey_UpArrow:
            text = texts_input_history_.MoveToPreviousInputAndRead();
            break;
        case ImGuiKey_DownArrow:
            text = texts_input_history_.MoveToNextInputAndRead();
            break;
        default:
            return;
    }
    data.DeleteChars(0, data.BufTextLen);
    data.InsertChars(0, text.begin(), text.end());
    logger.DebugLog("Updated text input box");
}

void Drawer::ClearStateAndNotify() {
    is_inputing_text_        = false;
    is_clear_button_pressed_ = false;
    is_bad_symbol_found_     = false;
    events_.clear();
    model_nodes_.clear();
    nodes_status_.clear();
    patterns_input_history_.Clear();
    texts_input_history_.Clear();
    found_words_.clear();
    ClearPatternInputBuffer();
    actrie_reset_port_.Notify();
    logger.DebugLog("Cleared actrie");
}

void Drawer::ClearPatternInputBuffer() noexcept {
    pattern_input_buffer_[0] = '\0';
}

void Drawer::ClearTextInputBuffer() noexcept {
    text_input_buffer_[0] = '\0';
}

std::string_view Drawer::TrimSpaces(std::string_view str) noexcept {
    std::size_t l = 0;
    while (l < str.size() && std::isspace(static_cast<std::uint8_t>(str[l]))) {
        l++;
    }
    std::size_t r = str.size();
    while (r > l && std::isspace(static_cast<std::uint8_t>(str[r - 1]))) {
        r--;
    }
    return {str.begin() + l, r - l};
}

}  // namespace AppSpace::GraphicsUtils
