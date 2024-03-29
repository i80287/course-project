#pragma once

#include <array>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <limits>
#include <queue>
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

    struct ACTNode {
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

        [[nodiscard]] VertexIndex operator[](
            std::size_t index) const noexcept {
            assert(index < kAlphabetLength);
            return edges[index];
        }

        [[nodiscard]] VertexIndex& operator[](std::size_t index) noexcept {
            assert(index < kAlphabetLength);
            return edges[index];
        }

        [[nodiscard]] constexpr bool IsTerminal() const noexcept {
            return word_index != kMissingWord;
        }
    };

    enum class DataStatus : std::uint32_t { kFoundNewSubstring };

    struct FoundSubstringInfo {
        std::reference_wrapper<const std::vector<ACTNode>> nodes;
        std::reference_wrapper<const std::vector<WordLength>>
            words_lengths;
        DataStatus status{};
        Pattern found_substring{};
        std::size_t substring_start_index{};
        VertexIndex current_vertex_index{};
    };

    struct BadInputPatternInfo {
        std::size_t symbol_index{};
        char bad_symbol{};
    };

    ACTrie();
    ACTrie& AddPattern(Pattern pattern);
    bool ContainsPattern(Pattern pattern) const noexcept;
    ACTrie& FindAllSubstringsInText(Text text);
    ACTrie& ResetACTrie();
    ACTrie& AddSubscriber(Observer<FoundSubstringInfo>* observer);
    ACTrie& AddSubscriber(Observer<BadInputPatternInfo>* observer);

    constexpr std::size_t NodesSize() const noexcept {
        return nodes_.size();
    }
    constexpr std::size_t PatternsSize() const noexcept {
        return words_lengths_.size();
    }

private:
    static constexpr std::size_t kDefaultNodesCapacity = 16;
    static constexpr VertexIndex kNullNodeIndex        = 0;
    static constexpr VertexIndex kFakePreRootIndex = kNullNodeIndex + 1;
    static constexpr VertexIndex kRootIndex        = kFakePreRootIndex + 1;
    static constexpr VertexIndex kDefaultNodesCount = kRootIndex + 1;

    void NotifyAboutFoundSubstring(VertexIndex current_node_index,
                                   std::size_t position_in_text,
                                   std::string_view text);
    void JumpThroughCompressedSuffixLinks(VertexIndex current_node_index,
                                          std::size_t position_in_text,
                                          std::string_view text);
    constexpr bool IsReady() const noexcept;
    static VertexIndex SymbolToIndex(char symbol) noexcept;
    static void ComputeLinksForNodes(std::vector<ACTNode>&);
    static void ComputeLinksForNode(ACTNode&, std::vector<ACTNode>&,
                                    std::queue<VertexIndex>&);
    bool IsACTrieInCorrectState() const;
    bool IsFakePrerootInCorrectState() const;

    std::vector<ACTNode> nodes_;
    std::vector<WordLength> words_lengths_;
    bool is_ready_ = false;
    Observable<FoundSubstringInfo> found_substrings_port_;
    Observable<BadInputPatternInfo> bad_input_port_;
    Observer<Pattern> pattern_in_port_;
    Observer<Text> text_in_port_;
};

constexpr bool ACTrie::IsReady() const noexcept {
    return is_ready_;
}

ACTrie::VertexIndex ACTrie::SymbolToIndex(char symbol) noexcept {
    std::int32_t symbol_as_int = static_cast<std::uint8_t>(symbol);
    if constexpr (kIsCaseInsensetive) {
        symbol_as_int = std::tolower(symbol_as_int);
    }

    return static_cast<VertexIndex>(symbol_as_int) - kAlphabetStart;
}

}  // namespace AppSpace::ACTrieDS
