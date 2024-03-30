#pragma once

#include <map>
#include <queue>
#include <variant>

#include "../App/ACTrie.hpp"
#include "../App/Observer.hpp"

namespace AppSpace {

class Drawer final {
    using ACTrieModel               = ACTrieDS::ACTrie;
    using FoundSubstringInfo        = ACTrieModel::FoundSubstringInfo;
    using BadInputPatternInfo       = ACTrieModel::BadInputPatternInfo;
    using Pattern                   = ACTrieModel::Pattern;
    using Text                      = ACTrieModel::Text;
    using UpdatedNodeInfo           = ACTrieModel::UpdatedNodeInfo;
    using PatternObserver           = Observer<Pattern>;
    using TextObserver              = Observer<Text>;
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
    void OnNewFrame();

private:
    void SetupImGuiStyle();
    void OnUpdatedNode(UpdatedNodeInfoPassBy updated_node_info);
    void OnFoundSubstring(FoundSubstringInfoPassBy substring_info);
    void OnBadPatternInput(BadInputPatternInfoPassBy bad_input_info);
    void Draw();

    UpdatedNodeObserver updated_node_port_;
    FoundSubstringObserver found_string_port_;
    BadInputPatternObserver bad_input_port_;
    Observable<Pattern> user_pattern_input_port_;
    Observable<Text> user_text_input_port_;

    enum class NodeStatus {
        kOther = 0,
        kAdded,
        kSuffixLinksComputed,
        kFoundSubstringInThisNode,
    };

    using EventType =
        std::variant<UpdatedNodeInfo, FoundSubstringInfo, BadInputPatternInfo>;
    enum EventTypeIndex : std::size_t {
        kUpdateNodeEventIndex      = 0,
        kFoundSubstringEventIndex  = 1,
        kBadInputPatternEventIndex = 2,
    };

    std::queue<EventType> events_;
    std::vector<ACTrieModel::ACTNode> model_nodes_;
    std::map<ACTrieModel::VertexIndex, NodeStatus> node_status_;

    bool suppressor_flag_ = false;
};

}  // namespace AppSpace
