#pragma once

#include <imgui.h>

#include <deque>
#include <map>
#include <variant>

#include "../App/ACTrie.hpp"
#include "../App/Observer.hpp"
#include "DrawerUtils/StringHistoryManager.hpp"

namespace AppSpace::GraphicsUtils {

class Drawer final {
    using ACTrieModel               = ACTrieDS::ACTrie;
    using FoundSubstringInfo        = ACTrieModel::FoundSubstringInfo;
    using BadInputPatternInfo       = ACTrieModel::BadInputPatternInfo;
    using Pattern                   = ACTrieModel::Pattern;
    using Text                      = ACTrieModel::Text;
    using UpdatedNodeInfo           = ACTrieModel::UpdatedNodeInfo;
    using PatternObserver           = Observer<Pattern>;
    using TextObserver              = Observer<Text>;
    using ACTrieResetObserver       = Observer<void, void>;
    using UpdatedNodeInfoPassBy     = ACTrieModel::UpdatedNodeInfoPassBy;
    using FoundSubstringInfoPassBy  = ACTrieModel::FoundSubstringInfoPassBy;
    using BadInputPatternInfoPassBy = ACTrieModel::BadInputPatternInfoPassBy;
    using UpdatedNodeObserver       = ACTrieModel::UpdatedNodeObserver;
    using FoundSubstringObserver    = ACTrieModel::FoundSubstringObserver;
    using BadInputPatternObserver   = ACTrieModel::BadInputPatternObserver;

public:
    Drawer();

    UpdatedNodeObserver* GetUpdatedNodeObserverPort() noexcept;
    FoundSubstringObserver* GetFoundStringObserverPort() noexcept;
    BadInputPatternObserver* GetBadInputPatternObserverPort() noexcept;
    Drawer& AddPatternSubscriber(PatternObserver* observer);
    Drawer& AddTextSubscriber(TextObserver* observer);
    Drawer& AddACTrieResetSubscriber(ACTrieResetObserver* observer);
    void OnNewFrame();

private:
    void HandleNextEvent();
    void SetupImGuiStyle();
    void OnUpdatedNode(UpdatedNodeInfoPassBy updated_node_info);
    void OnFoundSubstring(FoundSubstringInfoPassBy substring_info);
    void OnBadPatternInput(BadInputPatternInfoPassBy bad_input_info);
    void HandleNodeUpdate(const UpdatedNodeInfo& updated_node_info);
    void HandleFoundSubstring(const FoundSubstringInfo& updated_node_info);
    void HandleBadPatternInput(const BadInputPatternInfo& updated_node_info);
    void Draw();
    void DrawACTrieTree(ImVec2 canvas_screen_pos, ImVec2 canvas_end_pos);
    void DrawIOBlocks(ImVec2 io_blocks_start_pos, ImVec2 io_blocks_end_pos);
    void DrawPatternInputBlock(ImVec2 block_start_pos, ImVec2 block_end_pos);
    void DrawFoundWordsBlock(ImVec2 block_start_pos, ImVec2 block_end_pos);
    void DrawTextInputBlock(ImVec2 block_start_pos, ImVec2 block_end_pos);
    bool AddPatternInput(float text_input_width);
    bool AddTextInput(float text_input_width);
    void PatternInputImGuiCallback(ImGuiInputTextCallbackData& data);
    void TextInputImGuiCallback(ImGuiInputTextCallbackData& data);
    void ClearStateAndNotify();
    void ClearInputBuffer() noexcept;
    static std::string_view TrimSpaces(std::string_view str) noexcept;

    struct Palette final {
        struct AsImU32 final {
            static constexpr ImU32 kGrayColor  = IM_COL32(50, 50, 50, 255);
            static constexpr ImU32 kRedColor   = IM_COL32(250, 60, 50, 255);
            static constexpr ImU32 kBlackColor = IM_COL32_BLACK;
            static constexpr ImU32 kWhiteColor = IM_COL32_WHITE;
            static constexpr ImU32 kGreenColor = IM_COL32(0, 250, 0, 255);
        };

        struct AsImColor final {
            static constexpr ImColor kGrayColor = ImColor(AsImU32::kGrayColor);
            static constexpr ImColor kRedColor  = ImColor(AsImU32::kRedColor);
            static constexpr ImColor kBlackColor =
                ImColor(AsImU32::kBlackColor);
            static constexpr ImColor kWhiteColor =
                ImColor(AsImU32::kWhiteColor);
            static constexpr ImColor kGreenColor =
                ImColor(AsImU32::kGreenColor);
        };
    };

    struct CanvasConstants final {
        static constexpr float kTreeDrawingCanvasScaleX       = 0.6f;
        static constexpr float kTreeDrawingCanvasScaleY       = 1.0f;
        static constexpr float kTreeDrawingCanvasIndentScaleX = 0.005f;
        static constexpr float kPatternInputFieldWidthScaleX  = 0.5f;
        static constexpr float kIOBlocksIndentScaleX          = 0.005f;
        static constexpr float kTextInputHeightScaleY         = 0.05f;
        static constexpr float kTextInputIndentScaleY         = 0.005f;
        static constexpr float kButtonDecreaseScale           = 2.0f;
        static constexpr int kNumberOfIOBlocks                = 2;

        static_assert(kTreeDrawingCanvasScaleX +
                          kTreeDrawingCanvasIndentScaleX +
                          kIOBlocksIndentScaleX <
                      1.0f);
    };

    enum class NodeStatus {
        kOther = 0,
        kAdded,
        kSuffixLinksComputed,
        kFoundSubstringInThisNode,
    };
    struct NodeState final {
        ACTrieModel::VertexIndex node_parent_index;
        NodeStatus status;
        char parent_to_node_edge_symbol;
    };

    using EventType =
        std::variant<UpdatedNodeInfo, FoundSubstringInfo, BadInputPatternInfo>;
    // We use enum instead of enum class because
    //  this constants are used to identify type
    //  according to std::variant<...>::index()
    //  (we need an implicit cast from std::size_t
    //   to our enum and vice versa)
    enum EventTypeIndex : std::size_t {
        kUpdateNodeEventIndex      = 0,
        kFoundSubstringEventIndex  = 1,
        kBadInputPatternEventIndex = 2,
    };

    UpdatedNodeObserver updated_node_port_;
    FoundSubstringObserver found_string_port_;
    BadInputPatternObserver bad_input_port_;
    Observable<Pattern> user_pattern_input_port_;
    Observable<Text> user_text_input_port_;
    Observable<void, void> actrie_reset_port_;
    std::deque<EventType> events_;
    std::vector<ACTrieModel::ACTNode> model_nodes_;
    std::map<ACTrieModel::VertexIndex, NodeState> nodes_status_;
    DrawerUtils::StringHistoryManager patterns_input_history_;
    DrawerUtils::StringHistoryManager texts_input_history_;
    std::vector<std::string> found_words_;
    bool is_no_resize_                                   = false;
    bool is_no_decoration_                               = false;
    bool is_window_rounding_disabled_                    = false;
    bool is_scroll_to_bottom_                            = false;
    bool is_auto_scroll_                                 = false;
    bool is_inputing_text_                               = false;
    bool is_clear_button_pressed_                        = false;
    static constexpr std::size_t kPatternInputBufferSize = 512;
    std::array<char, kPatternInputBufferSize> input_buffer_{'\0'};
};

}  // namespace AppSpace::GraphicsUtils
