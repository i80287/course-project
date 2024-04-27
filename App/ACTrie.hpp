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

    static constexpr bool kIsCaseInsensitive = false;
    static constexpr char kAlphabetStart     = 'A';
    static constexpr char kAlphabetEnd       = 'z';
    static constexpr std::size_t kAlphabetLength =
        kAlphabetEnd - kAlphabetStart + 1;
    static constexpr VertexIndex kNullNodeIndex     = 0;
    static constexpr VertexIndex kFakePreRootIndex  = kNullNodeIndex + 1;
    static constexpr VertexIndex kRootIndex         = kFakePreRootIndex + 1;
    static constexpr VertexIndex kInitialNodesCount = kRootIndex + 1;

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
            assert(index < edges.size());
            return edges[index];
        }

        VertexIndex& operator[](std::size_t index) noexcept {
            assert(index < edges.size());
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
        VertexIndex parent_node_index;
        std::reference_wrapper<const ACTNode> node;
        UpdatedNodeStatus status;
        char parent_to_node_edge_symbol;
    };
    struct FoundSubstringInfo {
        std::string_view found_substring;
        std::size_t substring_start_index;
        VertexIndex current_vertex_index;
    };
    struct BadInputPatternInfo {
        std::size_t symbol_index;
        char bad_symbol;
    };
    using PassingThroughInfo        = VertexIndex;
    using UpdatedNodeInfoPassBy     = UpdatedNodeInfo;
    using FoundSubstringInfoPassBy  = FoundSubstringInfo;
    using BadInputPatternInfoPassBy = BadInputPatternInfo;
    using PassingThroughInfoPassBy  = PassingThroughInfo;
    using UpdatedNodeObserver =
        Observer<UpdatedNodeInfo, UpdatedNodeInfoPassBy>;
    using FoundSubstringObserver =
        Observer<FoundSubstringInfo, FoundSubstringInfoPassBy>;
    using BadInputPatternObserver =
        Observer<BadInputPatternInfo, BadInputPatternInfoPassBy>;
    using PassingThroughObserver =
        Observer<PassingThroughInfo, PassingThroughInfoPassBy>;

    ACTrie();
    ACTrie& AddPattern(Pattern pattern);
    ACTrie& BuildACTrie();
    ACTrie& ResetACTrie();
    ACTrie& FindAllSubstringsInText(Text text);
    ACTrie& AddSubscriber(UpdatedNodeObserver* observer);
    ACTrie& AddSubscriber(FoundSubstringObserver* observer);
    ACTrie& AddSubscriber(BadInputPatternObserver* observer);
    ACTrie& AddSubscriber(PassingThroughObserver* observer);
    constexpr std::size_t NodesSize() const noexcept;
    constexpr std::size_t PatternsSize() const noexcept;
    static constexpr VertexIndex SymbolToIndex(char symbol) noexcept;
    static constexpr char IndexToSymbol(VertexIndex index) noexcept;

private:
    static constexpr std::size_t kDefaultNodesCapacity = 16;
    static constexpr WordLength SizeToWordLength(std::size_t size) noexcept;
    void CreateInitialNodes();
    void NotifyAboutFoundSubstring(VertexIndex current_node_index,
                                   std::size_t position_in_text,
                                   std::string_view text);
    void JumpThroughCompressedSuffixLinks(VertexIndex current_node_index,
                                          std::size_t position_in_text,
                                          std::string_view text);
    void ComputeLinksForNodeChildren(VertexIndex node_index,
                                     std::queue<VertexIndex>& queue);
    bool IsACTrieInCorrectState() const;
    bool IsFakePreRootNodeInCorrectState() const;
    void NotifyAboutAddedNode(VertexIndex added_node_index,
                              VertexIndex parent_node_index,
                              char parent_to_node_edge_symbol);
    void NotifyAboutComputedSuffixLinks(VertexIndex node_index,
                                        VertexIndex parent_node_index,
                                        char parent_to_node_edge_symbol);
    void NotifyAboutInitialNodes();
    void NotifyAboutPassingThroughNode(VertexIndex node_index);

    std::vector<ACTNode> nodes_;
    std::vector<WordLength> words_lengths_;
    bool is_ready_ = false;
    Observable<UpdatedNodeInfo, UpdatedNodeInfoPassBy> updated_nodes_port_;
    Observable<FoundSubstringInfo, FoundSubstringInfoPassBy>
        found_substrings_port_;
    Observable<BadInputPatternInfo, BadInputPatternInfoPassBy> bad_input_port_;
    Observable<PassingThroughInfo, PassingThroughInfoPassBy>
        passing_through_port_;
};

constexpr std::size_t ACTrie::NodesSize() const noexcept {
    return nodes_.size();
}

constexpr std::size_t ACTrie::PatternsSize() const noexcept {
    return words_lengths_.size();
}

constexpr ACTrie::VertexIndex ACTrie::SymbolToIndex(char symbol) noexcept {
    std::int32_t symbol_as_int = static_cast<std::uint8_t>(symbol);
    if constexpr (kIsCaseInsensitive) {
        symbol_as_int = std::tolower(symbol_as_int);
    }

    return static_cast<VertexIndex>(symbol_as_int) - kAlphabetStart;
}

constexpr char ACTrie::IndexToSymbol(VertexIndex index) noexcept {
    return static_cast<char>(kAlphabetStart + index);
}

constexpr ACTrie::WordLength ACTrie::SizeToWordLength(
    std::size_t size) noexcept {
    return static_cast<WordLength>(size);
}

}  // namespace AppSpace::ACTrieDS
