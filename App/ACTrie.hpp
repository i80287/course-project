#pragma once

#include <array>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <limits>
#include <queue>
#include <string>
#include <string_view>
#include <vector>

#include "Observer.hpp"

namespace AppSpace::ACTrieDS {

class ACTrie final {
public:
    using VertexIndex = std::uint32_t;
    using WordLength  = std::uint32_t;
    using Pattern     = std::string_view;
    using Text        = std::string_view;

    static constexpr bool kIsCaseInsensetive = false;
    static constexpr char kAlphabetStart     = 'A';
    static constexpr char kAlphabetEnd       = 'z';
    static constexpr std::size_t kAlphabetLength =
        kAlphabetEnd - kAlphabetStart + 1;
    static constexpr VertexIndex kNullNodeIndex     = 0;
    static constexpr VertexIndex kFakePreRootIndex  = kNullNodeIndex + 1;
    static constexpr VertexIndex kRootIndex         = kFakePreRootIndex + 1;
    static constexpr VertexIndex kDefaultNodesCount = kRootIndex + 1;

    struct ACTNode final {
        static constexpr WordLength kMissingWord =
            std::numeric_limits<WordLength>::max();

        // Indexes in array of nodes
        std::array<VertexIndex, kAlphabetLength> edges{kNullNodeIndex};

        // Index in array of nodes
        VertexIndex suffix_link = kNullNodeIndex;

        // Index in array of nodes
        VertexIndex compressed_suffix_link = kNullNodeIndex;

        /*
         * Index of the word in the ac trie which ends on this
         * kMissingWord if node is not terminal
         */
        WordLength word_index = kMissingWord;

        VertexIndex operator[](std::size_t index) const noexcept {
            assert(index < kAlphabetLength);
            return edges[index];
        }

        VertexIndex& operator[](std::size_t index) noexcept {
            assert(index < kAlphabetLength);
            return edges[index];
        }

        constexpr bool IsTerminal() const noexcept {
            return word_index != kMissingWord;
        }
    };

    enum class UpdatedNodeStatus {
        kAdded,
        kSuffixLinksComputed,
    };

    struct UpdatedNodeInfo {
        VertexIndex node_index;
        VertexIndex node_parent_index;
        std::reference_wrapper<const ACTNode> node;
        UpdatedNodeStatus status;
        char parent_to_node_edge_symbol;
    };
    struct FoundSubstringInfo {
        std::string found_substring;
        std::size_t substring_start_index;
        VertexIndex current_vertex_index;
    };
    struct BadInputPatternInfo {
        std::size_t symbol_index;
        char bad_symbol;
    };
    using UpdatedNodeInfoPassBy = std::add_rvalue_reference_t<UpdatedNodeInfo>;
    using FoundSubstringInfoPassBy =
        std::add_rvalue_reference_t<FoundSubstringInfo>;
    using BadInputPatternInfoPassBy =
        std::add_rvalue_reference_t<BadInputPatternInfo>;
    using UpdatedNodeObserver =
        Observer<UpdatedNodeInfo, UpdatedNodeInfoPassBy>;
    using FoundSubstringObserver =
        Observer<FoundSubstringInfo, FoundSubstringInfoPassBy>;
    using BadInputPatternObserver =
        Observer<BadInputPatternInfo, BadInputPatternInfoPassBy>;

    ACTrie();
    ACTrie& AddPattern(Pattern pattern);
    ACTrie& FindAllSubstringsInText(Text text);
    ACTrie& ResetACTrie();
    ACTrie& AddSubscriber(UpdatedNodeObserver* observer);
    ACTrie& AddSubscriber(FoundSubstringObserver* observer);
    ACTrie& AddSubscriber(BadInputPatternObserver* observer);
    constexpr std::size_t NodesSize() const noexcept;
    constexpr std::size_t PatternsSize() const noexcept;

private:
    static constexpr std::size_t kDefaultNodesCapacity = 16;
    void NotifyAboutFoundSubstring(VertexIndex current_node_index,
                                   std::size_t position_in_text,
                                   std::string_view text);
    void JumpThroughCompressedSuffixLinks(VertexIndex current_node_index,
                                          std::size_t position_in_text,
                                          std::string_view text);
    constexpr bool IsReady() const noexcept;
    static VertexIndex SymbolToIndex(char symbol) noexcept;
    ACTrie& ComputeLinksForNodes();
    void ComputeLinksForNodeChildren(VertexIndex node_index,
                                     std::queue<VertexIndex>& queue);
    bool IsACTrieInCorrectState() const;
    bool IsFakePrerootInCorrectState() const;
    void NotifyAboutAddedNode(VertexIndex added_node_index,
                              VertexIndex node_parent_index,
                              char parent_to_node_edge_symbol);
    void NotifyAboutComputedSuffixLinks(VertexIndex node_index,
                                        VertexIndex node_parent_index,
                                        char parent_to_node_edge_symbol);

    std::vector<ACTNode> nodes_;
    std::vector<WordLength> words_lengths_;
    bool is_ready_ = false;
    Observable<UpdatedNodeInfo, UpdatedNodeInfoPassBy> updated_nodes_port_;
    Observable<FoundSubstringInfo, FoundSubstringInfoPassBy>
        found_substrings_port_;
    Observable<BadInputPatternInfo, BadInputPatternInfoPassBy> bad_input_port_;
    Observer<Pattern> pattern_in_port_;
    Observer<Text> text_in_port_;
};

constexpr bool ACTrie::IsReady() const noexcept {
    return is_ready_;
}

constexpr std::size_t ACTrie::NodesSize() const noexcept {
    return nodes_.size();
}

constexpr std::size_t ACTrie::PatternsSize() const noexcept {
    return words_lengths_.size();
}

}  // namespace AppSpace::ACTrieDS
