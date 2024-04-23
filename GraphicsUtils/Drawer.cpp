#include "Drawer.hpp"

#include <imgui.h>

#include <cassert>
#include <cctype>
#include <climits>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <numbers>
#include <string>
#include <unordered_set>
#include <vector>

#include "DrawerUtils/Logger.hpp"
#include "ImGuiExtensions.hpp"

namespace AppSpace::GraphicsUtils {

using DrawerUtils::logger;
using namespace ImGuiExtensions;

namespace {
// The technique for std::visit taken from the example on
//  https://en.cppreference.com/w/cpp/utility/variant/visit
// It is used to pass different lambdas to std::visit.
template <class... Ts>
struct overloaded : public Ts... {
    using Ts::operator()...;
};
// explicit deduction guide
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;
}  // namespace

Drawer::Drawer(ImVec2 window_size)
    : updated_node_in_port_([this](UpdatedNodeInfoPassBy updated_node_info) {
          OnUpdatedNode(std::forward<UpdatedNodeInfoPassBy>(updated_node_info));
      }),
      found_substring_in_port_([this](FoundSubstringInfoPassBy substring_info) {
          OnFoundSubstring(
              std::forward<FoundSubstringInfoPassBy>(substring_info));
      }),
      bad_input_in_port_([this](BadInputPatternInfoPassBy bad_input_info) {
          OnBadPatternInput(
              std::forward<BadInputPatternInfoPassBy>(bad_input_info));
      }),
      passing_through_in_port_([this](PassingThroughInfoPassBy passing_info) {
          OnPassingThrough(
              std::forward<PassingThroughInfoPassBy>(passing_info));
      }) {
    SetupImGuiStyle();
    ImGui::SetNextWindowSize(window_size, ImGuiCond_FirstUseEver);
    nodes_.reserve(ACTrieModel::kInitialNodesCount);
}

Drawer::UpdatedNodeObserver* Drawer::GetUpdatedNodeObserverPort() noexcept {
    return &updated_node_in_port_;
}

Drawer::FoundSubstringObserver* Drawer::GetFoundStringObserverPort() noexcept {
    return &found_substring_in_port_;
}

Drawer::BadInputPatternObserver*
Drawer::GetBadInputPatternObserverPort() noexcept {
    return &bad_input_in_port_;
}

Drawer::PassingThroughObserver*
Drawer::GetPassingThroughObserverPort() noexcept {
    return &passing_through_in_port_;
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
    if (events_.empty()) {
        return;
    }

    if (!show_speed_manager_.ShouldHandleNextEvent()) {
        return;
    }

    std::visit(overloaded{
                   [this](const CopiedUpdatedNodeInfo& updated_node_info) {
                       HandleNodeUpdate(updated_node_info);
                   },
                   [this](CopiedFoundSubstringInfo&& found_substring_info) {
                       HandleFoundSubstring(std::move(found_substring_info));
                   },
                   [this](BadInputPatternInfoPassBy bad_input_info) {
                       HandleBadPatternInput(
                           std::forward<BadInputPatternInfo>(bad_input_info));
                   },
                   [this](PassingThroughInfoPassBy passing_through_info) {
                       HandlePassingThrough(std::forward<PassingThroughInfo>(
                           passing_through_info));
                   },
               },
               std::move(events_.front()));
    events_.pop_front();
}

void Drawer::SetupImGuiStyle() {
    ImGui::StyleColorsDark();
}

void Drawer::OnUpdatedNode(UpdatedNodeInfoPassBy updated_node_info) {
    events_.emplace_back(CopiedUpdatedNodeInfo{
        .node_index        = updated_node_info.node_index,
        .parent_node_index = updated_node_info.parent_node_index,
        .node              = ACTrieModel::ACTNode(updated_node_info.node.get()),
        .status            = updated_node_info.status,
        .parent_to_node_edge_symbol =
            updated_node_info.parent_to_node_edge_symbol,
    });
}

void Drawer::OnFoundSubstring(FoundSubstringInfoPassBy substring_info) {
    events_.emplace_back(CopiedFoundSubstringInfo{
        .found_substring       = std::string(substring_info.found_substring),
        .substring_start_index = substring_info.substring_start_index,
        .current_vertex_index  = substring_info.current_vertex_index});
}

void Drawer::OnBadPatternInput(BadInputPatternInfoPassBy bad_input_info) {
    events_.emplace_back(
        std::forward<BadInputPatternInfoPassBy>(bad_input_info));
}

void Drawer::OnPassingThrough(PassingThroughInfoPassBy passing_info) {
    events_.emplace_back(std::forward<PassingThroughInfoPassBy>(passing_info));
}

void Drawer::HandleNodeUpdate(const CopiedUpdatedNodeInfo& updated_node_info) {
    switch (updated_node_info.status) {
        case ACTrieModel::UpdatedNodeStatus::kAdded:
            HandleAddedNode(updated_node_info);
            break;
        case ACTrieModel::UpdatedNodeStatus::kSuffixLinksComputed:
            HandleNodeComputedLinks(updated_node_info);
            break;
        default:
            assert(false);
            break;
    }
}

void Drawer::HandleAddedNode(const CopiedUpdatedNodeInfo& updated_node_info) {
    const auto node_index        = updated_node_info.node_index;
    const auto parent_node_index = updated_node_info.parent_node_index;
    if (node_index != ACTrieModel::kNullNodeIndex) {
        assert(parent_node_index < node_index);
        assert(parent_node_index < nodes_.size());
    }

    char parent_to_node_edge_symbol =
        updated_node_info.parent_to_node_edge_symbol;
    // We copy node because
    // "std::move of the expression of the trivially-copyable type
    // 'ACTrieModel::ACTNode' has no effect;"
    nodes_.emplace_back(NodeState{
        .node                       = updated_node_info.node,
        .parent_index               = parent_node_index,
        .parent_to_node_edge_symbol = parent_to_node_edge_symbol,
    });
    assert(node_index == nodes_.size() - 1);

    switch (node_index) {
        case ACTrieModel::kNullNodeIndex:
        case ACTrieModel::kFakePreRootIndex:
        case ACTrieModel::kRootIndex:
            break;
        default:
            NodeState& parent_node_state = nodes_[parent_node_index];
            parent_node_state
                .node[ACTrieModel::SymbolToIndex(parent_to_node_edge_symbol)] =
                node_index;
            break;
    }
    RecalculateAllNodesPositions(nodes_);
    logger.DebugLog("Added new node");
}

void Drawer::HandleNodeComputedLinks(
    const CopiedUpdatedNodeInfo& updated_node_info) {
    const auto node_index        = updated_node_info.node_index;
    const auto parent_node_index = updated_node_info.parent_node_index;
    assert(node_index != ACTrieModel::kNullNodeIndex);
    assert(node_index < nodes_.size());
    assert(parent_node_index < node_index);
    assert(parent_node_index < nodes_.size());
    const auto& updated_node            = updated_node_info.node;
    nodes_[node_index].node.suffix_link = updated_node.suffix_link;
    nodes_[node_index].node.compressed_suffix_link =
        updated_node.compressed_suffix_link;
    nodes_[node_index].node.word_index = updated_node.word_index;
    logger.DebugLog("Updated suffix links status for node");
}

void Drawer::HandleFoundSubstring(
    CopiedFoundSubstringInfo&& found_substring_info) {
    logger.DebugLog("Found substring: ", found_substring_info.found_substring);

    std::string str = std::move(found_substring_info.found_substring);
    // Start index will never be greater than 2^32,
    //  but std::to_string(uint32_t) is noexcept.
    std::string pos_str = std::to_string(
        std::uint32_t(found_substring_info.substring_start_index));
    constexpr std::string_view text = " at position ";
    str.reserve(str.size() + text.size() + pos_str.size());
    str += text;
    str += pos_str;
    found_words_.emplace_back(std::move(str));
    found_word_node_index_ = found_substring_info.current_vertex_index;
    is_scroll_found_words_to_bottom_ = true;
}

void Drawer::HandleBadPatternInput(BadInputPatternInfoPassBy bad_symbol_info) {
    logger.DebugLog("Bad symbol: '", "'", bad_symbol_info.bad_symbol);
    is_bad_symbol_found_ = true;
    bad_symbol_position_ = bad_symbol_info.symbol_index;
    bad_symbol_          = bad_symbol_info.bad_symbol;
}

void Drawer::HandlePassingThrough(PassingThroughInfo passing_info) {
    logger.DebugLog("Passing through node ", passing_info);
    passing_through_node_index_ = passing_info;
    found_word_node_index_      = ACTrieModel::kNullNodeIndex;
}

void Drawer::Draw() {
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
        std::max(available_width * CanvasParams::kTreeDrawingCanvasScaleX,
                 1.0f),
        std::max(available_height * CanvasParams::kTreeDrawingCanvasScaleY,
                 1.0f));
    const ImVec2 canvas_end_pos = canvas_screen_pos + canvas_size;
    const float tree_indent_width =
        available_width * CanvasParams::kTreeDrawingCanvasIndentScaleX;
    const float text_input_height =
        available_height * CanvasParams::kTextInputHeightScaleY;
    const float text_input_indent =
        available_height * CanvasParams::kTextInputIndentScaleY;
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
    const auto [available_width, available_height] =
        canvas_end_pos - canvas_screen_pos;
    const ImVec2 canvas_start_pos = ImVec2(
        canvas_screen_pos.x,
        canvas_screen_pos.y +
            available_height * CanvasParams::kCanvasButtonsIndentHeightScaleY);

    ImGui::PushItemWidth(available_width *
                         CanvasParams::kControllersWidthScaleX);
    ImGui::SliderInt("Speed", &show_speed_manager_.GetSpeedRef(),
                     DrawerShowSpeedManager::kMinSpeedUnit,
                     DrawerShowSpeedManager::kMaxSpeedUnit);
    if (is_inputting_text_) {
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, TreeParams::kSuffixLinkColor);
        ImGui::Checkbox("Show suffix links to root", &show_root_suffix_links_);
        ImGui::PopStyleColor();
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text,
                              TreeParams::kCompressedSuffixLinkColor);
        ImGui::Checkbox("Show compressed suffix links to root",
                        &show_root_compressed_suffix_links_);
        ImGui::PopStyleColor();
    }

    const bool changed = ImGui::SliderFloat("Font scale", &font_scale_,
                                            FontParams::kMinFontScale,
                                            FontParams::kMaxFontScale);
    if (changed) {
        ImGui::SetWindowFontScale(font_scale_);
    }
    ImGui::PopItemWidth();

    ImDrawList& draw_list = *ImGui::GetWindowDrawList();
    draw_list.AddRectFilled(canvas_start_pos, canvas_end_pos,
                            Palette::AsImU32::kGrayColor);
    draw_list.AddRect(canvas_start_pos, canvas_end_pos,
                      Palette::AsImU32::kWhiteColor);

    ImGui::InvisibleButton(
        "canvas", canvas_end_pos - canvas_start_pos,
        ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
    const bool is_active = ImGui::IsItemActive();
    if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right, 0)) {
        const ImGuiIO& io = ImGui::GetIO();
        scroll_trie_canvas_coordinates_.x += io.MouseDelta.x;
        scroll_trie_canvas_coordinates_.y += io.MouseDelta.y;
    }

    const ImVec2 canvas_move_vector =
        canvas_start_pos + scroll_trie_canvas_coordinates_;

    for (VertexIndex node_index = ACTrieModel::kRootIndex;
         node_index < nodes_.size(); node_index++) {
        const NodeState& node_state = nodes_[node_index];
        assert(node_state.coordinates != TreeParams::kNodeInvalidCoordinates);
        const ImVec2 node_center = node_state.coordinates + canvas_move_vector;

        if (!NodeFitsInCanvas(node_center, canvas_start_pos, canvas_end_pos)) {
            continue;
        }

        DrawEdgesBetweenNodeAndChildren(draw_list, node_index,
                                        canvas_move_vector, canvas_start_pos,
                                        canvas_end_pos);
        DrawSuffixLinksForNode(draw_list, node_index, canvas_move_vector,
                               canvas_start_pos, canvas_end_pos);
    }

    for (VertexIndex node_index = ACTrieModel::kRootIndex;
         node_index < nodes_.size(); node_index++) {
        const NodeState& node_state = nodes_[node_index];
        assert(node_state.coordinates != TreeParams::kNodeInvalidCoordinates);
        const ImVec2 node_center = node_state.coordinates + canvas_move_vector;

        if (!NodeFitsInCanvas(node_center, canvas_start_pos, canvas_end_pos)) {
            continue;
        }

        draw_list.AddCircleFilled(node_center, TreeParams::kNodeRadius,
                                  Palette::AsImU32::kDarkGreenColor);
        draw_list.AddCircle(node_center, TreeParams::kNodeRadius,
                            Palette::AsImU32::kBlackColor, 0,
                            TreeParams::kNodeThickness);
        if (node_state.node.IsTerminal()) {
            DrawTerminalNodeSign(draw_list, node_center);
        }
    }

    if (passing_through_node_index_ == ACTrieModel::kNullNodeIndex) {
        return;
    }

    assert(passing_through_node_index_ < nodes_.size());
    const NodeState& passing_through_node_state =
        nodes_[passing_through_node_index_];
    assert(passing_through_node_state.coordinates !=
           TreeParams::kNodeInvalidCoordinates);
    const ImVec2 passing_through_node_center =
        passing_through_node_state.coordinates + canvas_move_vector;
    if (NodeFitsInCanvas(passing_through_node_center, canvas_start_pos,
                         canvas_end_pos)) {
        draw_list.AddCircle(
            passing_through_node_center,
            TreeParams::kNodeRadius * TreeParams::kPassingThroughRadiusScale,
            Palette::AsImU32::kYellowColor);
    }

    if (found_word_node_index_ == ACTrieModel::kNullNodeIndex) {
        return;
    }

    assert(found_word_node_index_ < nodes_.size());
    const NodeState& found_word_node_state = nodes_[found_word_node_index_];
    assert(found_word_node_state.coordinates !=
           TreeParams::kNodeInvalidCoordinates);
    const ImVec2 found_word_node_center =
        found_word_node_state.coordinates + canvas_move_vector;
    if (NodeFitsInCanvas(found_word_node_center, canvas_start_pos,
                         canvas_end_pos)) {
        draw_list.AddCircleFilled(
            found_word_node_center,
            TreeParams::kNodeRadius * TreeParams::kFoundSubstringRadiusScale,
            Palette::AsImU32::kOrangeColor);
    }
}

void Drawer::DrawIOBlocks(ImVec2 io_blocks_start_pos,
                          ImVec2 io_blocks_end_pos) {
    const float blocks_indent =
        io_blocks_end_pos.x * CanvasParams::kIOBlocksIndentScaleX;
    const float block_width =
        (io_blocks_end_pos.x - io_blocks_start_pos.x - blocks_indent) /
        CanvasParams::kNumberOfIOBlocks;

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

    if (copy_to_clipboard) {
        ImGui::LogToClipboard();
    }
    const ImVec4 color = Palette::AsImColor::kBrightRedColor;
    ImGui::PushStyleColor(ImGuiCol_Text, color);
    for (const std::string& item : patterns_input_history_) {
        ImGui::TextUnformatted(item.data(), item.data() + item.size());
    }
    ImGui::PopStyleColor();
    if (copy_to_clipboard) {
        ImGui::LogFinish();
    }

    // Keep up at the bottom of the scroll region if we were already at the
    // bottom at the beginning of the frame. Using a scrollbar or
    // mouse-wheel will take away from the bottom edge.
    if (is_scroll_patterns_to_bottom_ ||
        (is_patterns_auto_scroll_ &&
         ImGui::GetScrollY() >= ImGui::GetScrollMaxY())) {
        ImGui::SetScrollHereY(1.0f);
        is_scroll_patterns_to_bottom_ = false;
    }

    ImGui::EndChild();
    if (!is_inputting_text_) {
        bool reclaim_focus =
            AddPatternInput(scrolling_region_size.x *
                            CanvasParams::kPatternInputFieldWidthScaleX);
        ImGui::SetItemDefaultFocus();
        // Autofocus on window apparition
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

    if (is_scroll_found_words_to_bottom_ ||
        (is_found_words_auto_scroll_ &&
         ImGui::GetScrollY() >= ImGui::GetScrollMaxY())) {
        ImGui::SetScrollHereY(1.0f);
        is_scroll_found_words_to_bottom_ = false;
    }

    ImGui::EndChild();
    ImGui::EndChild();
}

void Drawer::DrawTextInputBlock(ImVec2 block_start_pos, ImVec2 block_end_pos) {
    ImGui::SetCursorScreenPos(block_start_pos);
    if (is_inputting_text_) {
        const float text_input_box_width =
            (block_end_pos.x - block_start_pos.x) *
            CanvasParams::kTextInputBoxWithScaleX;
        bool reclaim_focus = AddTextInput(text_input_box_width);
        ImGui::SetItemDefaultFocus();
        // Autofocus on window apparition
        if (reclaim_focus) {
            ImGui::SetKeyboardFocusHere(-1);
        }
    } else {
        const ImVec2 button_size = (block_end_pos - block_start_pos) /
                                   CanvasParams::kButtonDecreaseScale;
        is_inputting_text_ = ImGui::Button("Build AC trie", button_size);
        if (is_inputting_text_) {
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
        bad_pattern_         = patterns_input_history_.PopAndGetLast();
        is_bad_symbol_found_ = false;
    }
    ImGui::Text(
        "Incorrect symbol '%c' passed in the pattern %s at position %zu",
        bad_symbol_, bad_pattern_.c_str(), bad_symbol_position_);
    if (ImGui::Button("Close")) {
        bad_symbol_modal_opened_ = false;
        ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
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
            // ImGui passes data->UserData as void*
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
            is_scroll_patterns_to_bottom_ = true;
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
            // ImGui passes data->UserData as void*
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
    passing_through_node_index_        = ACTrieModel::kNullNodeIndex;
    found_word_node_index_             = ACTrieModel::kNullNodeIndex;
    is_no_resize_                      = false;
    is_no_decoration_                  = false;
    is_window_rounding_disabled_       = false;
    is_scroll_patterns_to_bottom_      = false;
    is_patterns_auto_scroll_           = true;
    is_scroll_found_words_to_bottom_   = false;
    is_found_words_auto_scroll_        = true;
    is_inputting_text_                 = false;
    is_clear_button_pressed_           = false;
    show_root_suffix_links_            = false;
    show_root_compressed_suffix_links_ = false;
    is_bad_symbol_found_               = false;
    bad_symbol_modal_opened_           = false;
    bad_symbol_                        = '\0';
    bad_pattern_.clear();
    bad_symbol_position_ = 0;
    events_.clear();
    nodes_.clear();
    patterns_input_history_.Clear();
    texts_input_history_.Clear();
    found_words_.clear();
    ClearPatternInputBuffer();
    ClearTextInputBuffer();
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

    auto is_space = [](char c) noexcept {
        return static_cast<bool>(std::isspace(static_cast<std::uint8_t>(c)));
    };

    while (l < str.size() && is_space(str[l])) {
        l++;
    }
    std::size_t r = str.size();
    while (r > l && is_space(str[r - 1])) {
        r--;
    }
    return {str.begin() + l, r - l};
}

void Drawer::RecalculateAllNodesPositions(std::vector<NodeState>& nodes) {
    if (ACTrieModel::kRootIndex >= nodes.size()) {
        return;
    }

    for (NodeState& node_state : nodes) {
        node_state.coordinates = TreeParams::kNodeInvalidCoordinates;
        node_state.leftest_child_x_coordinate =
            TreeParams::kDefaultLeftestChildXCoordinate;
        node_state.rightest_child_y_coordinate =
            TreeParams::kDefaultRightestChildXCoordinate;
    }

    std::vector<VertexIndex> leaf_node_indexes;
    leaf_node_indexes.reserve(nodes.size() / 2);
    FindSortedLeafNodesAndComputeYCoords(ACTrieModel::kRootIndex, 0, nodes,
                                         leaf_node_indexes);

    float leaf_nodes_x_coord = 0;
    for (VertexIndex leaf_node_index : leaf_node_indexes) {
        leaf_nodes_x_coord += TreeParams::kNodeOffsetX;
        leaf_nodes_x_coord += TreeParams::kNodeRadius;
        assert(leaf_node_index < nodes.size());
        nodes[leaf_node_index].coordinates.x = leaf_nodes_x_coord;
        leaf_nodes_x_coord += TreeParams::kNodeRadius;
        leaf_nodes_x_coord += TreeParams::kNodeOffsetX;
        leaf_nodes_x_coord += TreeParams::kDeltaXBetweenNodes;
    }

    AlignNodesAboveLeafNodes(nodes, std::move(leaf_node_indexes));
}

void Drawer::AlignNodesAboveLeafNodes(
    std::vector<NodeState>& nodes,
    std::vector<VertexIndex>&& leaf_node_indexes) {
    assert(std::unordered_set<VertexIndex>(leaf_node_indexes.cbegin(),
                                           leaf_node_indexes.cend())
               .size() == leaf_node_indexes.size());

    std::vector<VertexIndex> nodes_on_current_layer =
        std::move(leaf_node_indexes);
    std::vector<VertexIndex> nodes_on_layer_above;
    nodes_on_layer_above.reserve(nodes_on_current_layer.size());

    while (!nodes_on_current_layer.empty()) {
        FillAboveLayerCoordinatesUsingCurrentLayer(nodes, nodes_on_layer_above,
                                                   nodes_on_current_layer);
        nodes_on_current_layer.swap(nodes_on_layer_above);
        nodes_on_layer_above.clear();
    }
}

void Drawer::FillAboveLayerCoordinatesUsingCurrentLayer(
    std::vector<NodeState>& nodes,
    std::vector<VertexIndex>& nodes_on_layer_above,
    const std::vector<VertexIndex>& nodes_on_current_layer) noexcept {
    // At least nodes_on_current_layer.size() space should be reserved
    //  in AlignNodesAboveLeafNodes.
    assert(nodes_on_layer_above.capacity() >= nodes_on_current_layer.size());

    for (std::size_t i = 0; i < nodes_on_current_layer.size();) {
        const VertexIndex left_child_index = nodes_on_current_layer[i];
        assert(left_child_index < nodes.size());
        const VertexIndex parent_index = nodes[left_child_index].parent_index;
        const float y_coord            = nodes[left_child_index].coordinates.y;

        auto j = i + 1;
        auto node_has_same_parent =
            [&nodes, parent_index](VertexIndex node_index) noexcept {
                return nodes[node_index].parent_index == parent_index;
            };
        while (j < nodes_on_current_layer.size() &&
               node_has_same_parent(nodes_on_current_layer[j])) {
            assert(nodes[nodes_on_current_layer[j]].coordinates.y == y_coord);
            j++;
        }

        const VertexIndex right_child_index = nodes_on_current_layer[j - 1];
        assert(right_child_index < nodes.size());
        assert(nodes[right_child_index].parent_index == parent_index);

        NodeState& parent_node = nodes[parent_index];
        parent_node.coordinates.y =
            y_coord - TreeParams::kDeltaYBetweenNodeCenters;

        const float left_child_x  = nodes[left_child_index].coordinates.x;
        const float right_child_x = nodes[right_child_index].coordinates.x;
        assert(left_child_x != TreeParams::kNodeInvalidCoordinates.x);
        assert(right_child_x != TreeParams::kNodeInvalidCoordinates.x);
        assert(left_child_index == right_child_index ||
               left_child_x < right_child_x);

        parent_node.leftest_child_x_coordinate =
            std::min(parent_node.leftest_child_x_coordinate, left_child_x);
        parent_node.rightest_child_y_coordinate =
            std::max(parent_node.rightest_child_y_coordinate, right_child_x);

        if (parent_index != ACTrieModel::kFakePreRootIndex) {
            nodes_on_layer_above.push_back(parent_index);
        } else {
            assert(left_child_index == right_child_index);
            assert(left_child_index == ACTrieModel::kRootIndex);
        }

        assert(i < j);
        i = j;
    }

    for (VertexIndex node_index : nodes_on_layer_above) {
        assert(nodes[node_index].coordinates.y !=
               TreeParams::kNodeInvalidCoordinates.y);
        nodes[node_index].coordinates.x =
            (nodes[node_index].leftest_child_x_coordinate +
             nodes[node_index].rightest_child_y_coordinate) /
            2;
    }
}

void Drawer::FindSortedLeafNodesAndComputeYCoords(
    VertexIndex start_node, uint32_t dfs_depth, std::vector<NodeState>& nodes,
    std::vector<VertexIndex>& leaf_node_indexes) {
    bool has_at_least_one_child = false;
    assert(start_node < nodes.size());
    for (VertexIndex child_node_index : nodes[start_node].node.edges) {
        if (child_node_index == ACTrieModel::kNullNodeIndex) {
            continue;
        }
        has_at_least_one_child = true;
        FindSortedLeafNodesAndComputeYCoords(child_node_index, dfs_depth + 1,
                                             nodes, leaf_node_indexes);
    }

    if (!has_at_least_one_child) {
        nodes[start_node].coordinates.y = static_cast<float>(dfs_depth) *
                                          TreeParams::kDeltaYBetweenNodeCenters;
        leaf_node_indexes.push_back(start_node);
    }
}

void Drawer::DrawTerminalNodeSign(ImDrawList& draw_list, ImVec2 node_center) {
    constexpr float len =
        TreeParams::kNodeRadius / std::numbers::sqrt2_v<float>;
    const ImVec2 left_upper_point  = node_center - len;
    const ImVec2 right_lower_point = left_upper_point + len * 2;
    const ImVec2 left_lower_point =
        ImVec2(left_upper_point.x, right_lower_point.y);
    const ImVec2 right_upper_point =
        ImVec2(right_lower_point.x, left_upper_point.y);
    draw_list.AddLine(left_upper_point, right_lower_point,
                      Palette::AsImU32::kBlackColor);
    draw_list.AddLine(left_lower_point, right_upper_point,
                      Palette::AsImU32::kBlackColor);
}

void Drawer::DrawEdgesBetweenNodeAndChildren(ImDrawList& draw_list,
                                             VertexIndex node_index,
                                             ImVec2 canvas_move_vector,
                                             ImVec2 canvas_start_pos,
                                             ImVec2 canvas_end_pos) const {
    assert(node_index < nodes_.size());
    const NodeState& node_state = nodes_[node_index];
    const ImVec2 node_center    = node_state.coordinates + canvas_move_vector;
    for (VertexIndex child_index : node_state.node.edges) {
        if (child_index == ACTrieModel::kNullNodeIndex) {
            continue;
        }
        assert(child_index < nodes_.size());
        const NodeState& child_node_state = nodes_[child_index];
        const ImVec2 child_center =
            child_node_state.coordinates + canvas_move_vector;
        if (!NodeFitsInCanvas(child_center, canvas_start_pos, canvas_end_pos)) {
            continue;
        }
        DrawEdge(draw_list, node_center, child_center,
                 child_node_state.parent_to_node_edge_symbol);
    }
}

void Drawer::DrawEdge(ImDrawList& draw_list, ImVec2 node_center,
                      ImVec2 child_center, char edge_symbol,
                      ImU32 edge_color) const {
    const ImVec2 vec_between_circle_centers = child_center - node_center;
    const float vec_len =
        std::hypot(vec_between_circle_centers.x, vec_between_circle_centers.y);
    assert(vec_len > TreeParams::kNodeRadius);
    const ImVec2 inner_radius_vec =
        vec_between_circle_centers / vec_len * TreeParams::kNodeRadius;
    const ImVec2 edge_start_pos = node_center + inner_radius_vec;
    const ImVec2 edge_end_pos   = child_center - inner_radius_vec;
    draw_list.AddLine(edge_start_pos, edge_end_pos, edge_color,
                      TreeParams::kEdgeThickness);

    if (edge_symbol != '\0') {
        const char edge_text[]              = {edge_symbol, '\0'};
        const std::string_view edge_text_sv = edge_text;
        const ImVec2 edge_center_pos =
            ImVecMiddle(edge_start_pos, edge_end_pos);
        const float edge_rectangle_height = edge_end_pos.y - edge_start_pos.y;
        const ImVec2 text_pos =
            ImVec2(edge_center_pos.x,
                   edge_center_pos.y - edge_rectangle_height *
                                           TreeParams::kEdgeTextPositionScaleY);
        draw_list.AddText(text_pos, Palette::AsImU32::kGreenColor,
                          edge_text_sv.begin(), edge_text_sv.end());
    }
}

void Drawer::DrawSuffixLinksForNode(ImDrawList& draw_list,
                                    VertexIndex node_index,
                                    ImVec2 canvas_move_vector,
                                    ImVec2 canvas_start_pos,
                                    ImVec2 canvas_end_pos) const {
    assert(node_index < nodes_.size());
    switch (node_index) {
        case ACTrieModel::kNullNodeIndex:
        case ACTrieModel::kFakePreRootIndex:
            assert(false);
            break;
        case ACTrieModel::kRootIndex:
            return;
        default:
            break;
    }
    const NodeState& node_state      = nodes_[node_index];
    const ACTrieModel::ACTNode& node = node_state.node;
    const VertexIndex suffix_link    = node.suffix_link;
    if (suffix_link == ACTrieModel::kNullNodeIndex) {
        return;
    }
    const VertexIndex compressed_suffix_link = node.compressed_suffix_link;
    assert(compressed_suffix_link != ACTrieModel::kNullNodeIndex);

    const ImVec2 node_center = node_state.coordinates + canvas_move_vector;

    if (suffix_link != ACTrieModel::kRootIndex || show_root_suffix_links_) {
        assert(suffix_link < nodes_.size());
        const ImVec2 sl_node_center =
            nodes_[suffix_link].coordinates + canvas_move_vector;
        if (NodeFitsInCanvas(sl_node_center, canvas_start_pos,
                             canvas_end_pos)) {
            DrawSuffixLink(draw_list, node_center, sl_node_center,
                           TreeParams::kSuffixLinkCenterMoveVector,
                           TreeParams::kSuffixLinkColor);
        }
    }

    if (compressed_suffix_link != ACTrieModel::kRootIndex ||
        show_root_compressed_suffix_links_) {
        assert(compressed_suffix_link < nodes_.size());
        const ImVec2 csl_node_center =
            nodes_[compressed_suffix_link].coordinates + canvas_move_vector;
        if (NodeFitsInCanvas(csl_node_center, canvas_start_pos,
                             canvas_end_pos)) {
            DrawSuffixLink(draw_list, node_center, csl_node_center,
                           TreeParams::kCompressedSuffixLinkCenterMoveVector,
                           TreeParams::kCompressedSuffixLinkColor);
        }
    }
}

void Drawer::DrawSuffixLink(ImDrawList& draw_list, ImVec2 node_center,
                            ImVec2 suf_link_node_center,
                            ImVec2 suf_link_move_vector,
                            ImU32 suf_link_color) const {
    draw_list.AddLine(node_center + suf_link_move_vector,
                      suf_link_node_center + suf_link_move_vector,
                      suf_link_color, TreeParams::kEdgeThickness);
}

bool Drawer::NodeFitsInCanvas(ImVec2 node_center, ImVec2 canvas_start_pos,
                              ImVec2 canvas_end_pos) noexcept {
    assert(canvas_start_pos.x < canvas_end_pos.x);
    assert(canvas_start_pos.y < canvas_end_pos.y);
    return canvas_start_pos.x <= node_center.x - TreeParams::kNodeRadius &&
           node_center.x + TreeParams::kNodeRadius <= canvas_end_pos.x &&
           canvas_start_pos.y <= node_center.y - TreeParams::kNodeRadius &&
           node_center.y + TreeParams::kNodeRadius <= canvas_end_pos.y;
}

}  // namespace AppSpace::GraphicsUtils
