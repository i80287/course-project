#pragma once

#include <imgui.h>

#include <deque>
#include <limits>
#include <variant>

#include "../App/ACTrie.hpp"
#include "../App/Observer.hpp"
#include "DrawerShowSpeedManager.hpp"
#include "DrawerUtils/StringHistoryManager.hpp"
#include "Palette.hpp"

namespace AppSpace::GraphicsUtils {

class Drawer final {
    using ACTrieModel = ACTrieDS::ACTrie;
    using VertexIndex = ACTrieModel::VertexIndex;
    using Pattern     = ACTrieModel::Pattern;
    using Text        = ACTrieModel::Text;

    using PatternObserver     = Observer<Pattern>;
    using TextObserver        = Observer<Text>;
    using ACTrieResetObserver = Observer<void, void>;
    using ACTrieBuildObserver = Observer<void, void>;

    using UpdatedNodeInfo           = ACTrieModel::UpdatedNodeInfo;
    using FoundSubstringInfo        = ACTrieModel::FoundSubstringInfo;
    using BadInputPatternInfo       = ACTrieModel::BadInputPatternInfo;
    using PassingThroughInfo        = ACTrieModel::PassingThroughInfo;
    using UpdatedNodeInfoPassBy     = ACTrieModel::UpdatedNodeInfoPassBy;
    using FoundSubstringInfoPassBy  = ACTrieModel::FoundSubstringInfoPassBy;
    using BadInputPatternInfoPassBy = ACTrieModel::BadInputPatternInfoPassBy;
    using PassingThroughInfoPassBy  = ACTrieModel::PassingThroughInfoPassBy;
    using UpdatedNodeObserver       = ACTrieModel::UpdatedNodeObserver;
    using FoundSubstringObserver    = ACTrieModel::FoundSubstringObserver;
    using BadInputPatternObserver   = ACTrieModel::BadInputPatternObserver;
    using PassingThroughObserver    = ACTrieModel::PassingThroughObserver;

public:
    Drawer(ImVec2 window_size);

    UpdatedNodeObserver* GetUpdatedNodeObserverPort() noexcept;
    FoundSubstringObserver* GetFoundStringObserverPort() noexcept;
    BadInputPatternObserver* GetBadInputPatternObserverPort() noexcept;
    PassingThroughObserver* GetPassingThroughObserverPort() noexcept;
    Drawer& AddPatternSubscriber(PatternObserver* observer);
    Drawer& AddTextSubscriber(TextObserver* observer);
    Drawer& AddACTrieResetSubscriber(ACTrieResetObserver* observer);
    Drawer& AddACTrieBuildSubscriber(ACTrieBuildObserver* observer);
    void OnNewFrame();

private:
    struct CanvasParams final {
        static constexpr float kControllersWidthScaleX          = 1.0f / 4;
        static constexpr float kTreeDrawingCanvasScaleX         = 0.6f;
        static constexpr float kTreeDrawingCanvasScaleY         = 1.0f;
        static constexpr float kTreeDrawingCanvasIndentScaleX   = 0.005f;
        static constexpr float kPatternInputFieldWidthScaleX    = 0.5f;
        static constexpr float kCanvasButtonsIndentHeightScaleY = 0.05f;
        static constexpr float kIOBlocksIndentScaleX            = 0.005f;
        static constexpr float kTextInputHeightScaleY =
            kCanvasButtonsIndentHeightScaleY;
        static constexpr float kTextInputIndentScaleY     = 0.005f;
        static constexpr float kButtonDecreaseScale       = 2.0f;
        static constexpr float kTextInputBoxWithScaleX    = 0.7f;
        static constexpr int kNumberOfIOBlocks            = 2;
        static constexpr ImVec2 kInitialScrollingPosition = ImVec2(50, 50);

        static_assert(kTreeDrawingCanvasScaleX +
                              kTreeDrawingCanvasIndentScaleX +
                              kIOBlocksIndentScaleX <
                          1.0f,
                      "");
        static_assert(kInitialScrollingPosition.x > 0 &&
                          kInitialScrollingPosition.y > 0,
                      "");
        static_assert(kControllersWidthScaleX * 3 <= 1,
                      "no place for 3 controllers");
    };
    struct TreeParams final {
        static constexpr ImVec2 kNodeInvalidCoordinates = ImVec2(-1, -1);
        static constexpr float kDefaultLeftestChildXCoordinate =
            std::numeric_limits<float>::max();
        static constexpr float kDefaultRightestChildXCoordinate =
            std::numeric_limits<float>::min();
        static constexpr ImVec2 kRootNodeCoordinates      = ImVec2(0, 0);
        static constexpr float kNodeRadius                = 15.0f;
        static constexpr float kPassingThroughRadiusScale = 0.8f;
        static constexpr float kFoundSubstringRadiusScale = 0.5f;
        static constexpr float kNodeDiameter              = kNodeRadius * 2;
        static constexpr float kDeltaYBetweenNodeCenters  = 50.0f;
        static constexpr float kDeltaXBetweenNodes        = 5.0f;
        static constexpr float kNodeOffsetX               = 2.0f;
        static constexpr float kEdgeThickness             = 2.0f;
        static constexpr float kNodeThickness             = 1.5f;
        static constexpr float kEdgeTextPositionScaleY    = 0.2f;
        static constexpr float kSuffixLinkXOffsetScale    = -0.2f;
        static constexpr float kCompressedSuffixLinkXOffsetScale = 0.2f;

        static constexpr ImU32 kSuffixLinkColor = Palette::AsImU32::kBlueColor;
        static constexpr ImU32 kCompressedSuffixLinkColor =
            Palette::AsImU32::kGreenColor;

        static constexpr ImVec2 kSuffixLinkCenterMoveVector = ImVec2(
            TreeParams::kNodeRadius * TreeParams::kSuffixLinkXOffsetScale, 0);
        static constexpr ImVec2 kCompressedSuffixLinkCenterMoveVector =
            ImVec2(TreeParams::kNodeRadius *
                       TreeParams::kCompressedSuffixLinkXOffsetScale,
                   0);
        static_assert(-1 < kSuffixLinkXOffsetScale &&
                          kSuffixLinkXOffsetScale < 1,
                      "");
        static_assert(-1 < kCompressedSuffixLinkXOffsetScale &&
                          kCompressedSuffixLinkXOffsetScale < 1,
                      "");
        static_assert(kSuffixLinkXOffsetScale *
                              kCompressedSuffixLinkXOffsetScale <
                          0,
                      "scales must have opposite signs");
        static_assert(0 < kEdgeTextPositionScaleY &&
                          kEdgeTextPositionScaleY < 0.5,
                      "");
        static_assert(kDeltaYBetweenNodeCenters > kNodeDiameter, "");
        static_assert(0 < kPassingThroughRadiusScale &&
                          kPassingThroughRadiusScale < 1,
                      "");
        static_assert(0 < kFoundSubstringRadiusScale &&
                          kFoundSubstringRadiusScale < 1,
                      "");
        static_assert(kFoundSubstringRadiusScale != kPassingThroughRadiusScale,
                      "");
    };
    struct NodeState final {
        ACTrieModel::ACTNode node;
        VertexIndex parent_index;
        char parent_to_node_edge_symbol;
        ImVec2 coordinates = TreeParams::kNodeInvalidCoordinates;
        float leftest_child_x_coordinate =
            TreeParams::kDefaultLeftestChildXCoordinate;
        float rightest_child_y_coordinate =
            TreeParams::kDefaultRightestChildXCoordinate;
    };
    // same as ACTrie::UpdatedNodeInfo but with copied node.
    struct CopiedUpdatedNodeInfo final {
        VertexIndex node_index;
        VertexIndex parent_node_index;
        ACTrieModel::ACTNode node;
        ACTrieModel::UpdatedNodeStatus status;
        char parent_to_node_edge_symbol;
    };
    // same as ACTrie::FoundSubstringInfo but with string_view
    //  copied into string.
    struct CopiedFoundSubstringInfo final {
        std::string found_substring;
        std::size_t substring_start_index;
        VertexIndex current_vertex_index;
    };
    struct FontParams final {
        static constexpr float kMinFontScale = 0.8f;
        static constexpr float kMaxFontScale = 1.6f;
    };
    using EventType =
        std::variant<CopiedUpdatedNodeInfo, CopiedFoundSubstringInfo,
                     BadInputPatternInfo, PassingThroughInfo>;

    void HandleNextEvent();
    void SetupImGuiStyle();
    void OnUpdatedNode(UpdatedNodeInfoPassBy updated_node_info);
    void OnFoundSubstring(FoundSubstringInfoPassBy substring_info);
    void OnBadPatternInput(BadInputPatternInfoPassBy bad_input_info);
    void OnPassingThrough(PassingThroughInfoPassBy passing_info);
    void HandleNodeUpdate(const CopiedUpdatedNodeInfo& updated_node_info);
    void HandleAddedNode(const CopiedUpdatedNodeInfo& updated_node_info);
    void HandleNodeComputedLinks(
        const CopiedUpdatedNodeInfo& updated_node_info);
    void HandleFoundSubstring(CopiedFoundSubstringInfo&& found_substring_info);
    void HandleBadPatternInput(BadInputPatternInfoPassBy updated_node_info);
    void HandlePassingThrough(PassingThroughInfo passing_info);
    void Draw();
    void DrawACTrieTree(ImVec2 canvas_screen_pos, ImVec2 canvas_end_pos);
    void DrawIOBlocks(ImVec2 io_blocks_start_pos, ImVec2 io_blocks_end_pos);
    void DrawPatternInputBlock(ImVec2 block_start_pos, ImVec2 block_end_pos);
    void DrawFoundWordsBlock(ImVec2 block_start_pos, ImVec2 block_end_pos);
    void DrawTextInputBlock(ImVec2 block_start_pos, ImVec2 block_end_pos);
    void DrawBadSymbolWindow();
    bool AddPatternInput(float text_input_width);
    bool AddTextInput(float text_input_width);
    void PatternInputImGuiCallback(ImGuiInputTextCallbackData& data);
    void TextInputImGuiCallback(ImGuiInputTextCallbackData& data);
    void ClearStateAndNotify();
    void ClearPatternInputBuffer() noexcept;
    void ClearTextInputBuffer() noexcept;
    static std::string_view TrimSpaces(std::string_view str) noexcept;
    void RecalculateNodePosition(VertexIndex node);
    void VerifyXCoordsOfSiblingsOfNode(VertexIndex node_index,
                                       float left_x_coord, float right_x_coord);
    static void RecalculateAllNodesPositions(std::vector<NodeState>& nodes);
    static void FindSortedLeafNodesAndComputeYCoords(
        VertexIndex start_node, std::uint32_t dfs_depth,
        std::vector<NodeState>& nodes,
        std::vector<VertexIndex>& leaf_node_indexes);
    static void AlignNodesAboveLeafNodes(
        std::vector<NodeState>& nodes,
        std::vector<VertexIndex>&& leaf_node_indexes);
    static void FillAboveLayerCoordinatesUsingCurrentLayer(
        std::vector<NodeState>& nodes,
        std::vector<VertexIndex>& nodes_on_layer_above,
        const std::vector<VertexIndex>& nodes_on_current_layer) noexcept;
    void DrawTerminalNodeSign(ImDrawList& draw_list, ImVec2 node_center);
    void DrawEdgesBetweenNodeAndChildren(ImDrawList& draw_list,
                                         VertexIndex node_index,
                                         ImVec2 canvas_move_vector,
                                         ImVec2 canvas_start_pos,
                                         ImVec2 canvas_end_pos) const;
    void DrawEdge(ImDrawList& draw_list, ImVec2 node_center,
                  ImVec2 child_center, char edge_symbol,
                  ImU32 edge_color = Palette::AsImU32::kBrightRedColor) const;
    void DrawSuffixLinksForNode(ImDrawList& draw_list, VertexIndex node_index,
                                ImVec2 canvas_move_vector,
                                ImVec2 canvas_start_pos,
                                ImVec2 canvas_end_pos) const;
    void DrawSuffixLink(ImDrawList& draw_list, ImVec2 node_center,
                        ImVec2 suf_link_node_center,
                        ImVec2 suf_link_move_vector,
                        ImU32 suf_link_color) const;
    static bool NodeFitsInCanvas(ImVec2 node_center, ImVec2 canvas_start_pos,
                                 ImVec2 canvas_end_pos) noexcept;

    static constexpr std::size_t kPatternInputBufferSize = 64;
    static constexpr std::size_t kTextInputBufferSize    = 2048;

    UpdatedNodeObserver updated_node_in_port_;
    FoundSubstringObserver found_substring_in_port_;
    BadInputPatternObserver bad_input_in_port_;
    PassingThroughObserver passing_through_in_port_;
    Observable<Pattern> user_pattern_input_port_;
    Observable<Text> user_text_input_port_;
    Observable<void, void> actrie_reset_port_;
    Observable<void, void> actrie_build_port_;

    std::deque<EventType> events_;
    std::vector<NodeState> nodes_;
    DrawerShowSpeedManager show_speed_manager_;
    DrawerUtils::StringHistoryManager patterns_input_history_;
    DrawerUtils::StringHistoryManager texts_input_history_;
    std::vector<std::string> found_words_;
    ImVec2 scroll_trie_canvas_coordinates_ =
        CanvasParams::kInitialScrollingPosition;
    VertexIndex passing_through_node_index_ = ACTrieModel::kNullNodeIndex;
    VertexIndex found_word_node_index_      = ACTrieModel::kNullNodeIndex;
    float font_scale_                       = 1.0;
    bool is_no_resize_                      = false;
    bool is_no_decoration_                  = false;
    bool is_window_rounding_disabled_       = false;
    bool is_scroll_patterns_to_bottom_      = false;
    bool is_patterns_auto_scroll_           = true;
    bool is_scroll_found_words_to_bottom_   = false;
    bool is_found_words_auto_scroll_        = true;
    bool is_inputting_text_                 = false;
    bool is_clear_button_pressed_           = false;
    bool show_root_suffix_links_            = false;
    bool show_root_compressed_suffix_links_ = false;
    bool is_bad_symbol_found_               = false;
    bool bad_symbol_modal_opened_           = false;
    char bad_symbol_                        = '\0';
    std::string bad_pattern_;
    std::size_t bad_symbol_position_ = 0;
    std::vector<char> pattern_input_buffer_ =
        std::vector<char>(kPatternInputBufferSize, '\0');
    std::vector<char> text_input_buffer_ =
        std::vector<char>(kTextInputBufferSize, '\0');
};

}  // namespace AppSpace::GraphicsUtils
