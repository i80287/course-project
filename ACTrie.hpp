#pragma once

#include <array>
#include <cassert>
#include <cstdint>
#include <iterator>
#include <limits>
#include <queue>
#include <string_view>
#include <vector>

#include "Observer.hpp"

namespace AppSpace::ACTrieDS {

class ACTrie final {
public:
    using vertex_index_t = std::uint32_t;
    using word_length_t  = std::uint32_t;

    static constexpr bool kIsCaseInsensetive = false;
    static constexpr char kAlphabetStart     = 'A';
    static constexpr char kAlphabetEnd       = 'z';
    static constexpr std::size_t kAlphabetLength =
        kAlphabetEnd - kAlphabetStart + 1;

    struct ACTNode {
        static constexpr word_length_t kMissingWord =
            std::numeric_limits<word_length_t>::max();

        // Indexes in array of nodes
        std::array<vertex_index_t, kAlphabetLength> edges{kNullNodeIndex};

        // Index in array of nodes
        vertex_index_t suffix_link = kNullNodeIndex;

        // Index in array of nodes
        vertex_index_t compressed_suffix_link = kNullNodeIndex;

        /*
         * Index of the word in the ac trie which ends on this
         * kMissingWord if node is not terminal
         */
        word_length_t word_index = kMissingWord;

        [[nodiscard]] vertex_index_t operator[](
            std::size_t index) const noexcept {
            assert(index < kAlphabetLength);
            return edges[index];
        }

        [[nodiscard]] vertex_index_t& operator[](
            std::size_t index) noexcept {
            assert(index < kAlphabetLength);
            return edges[index];
        }

        [[nodiscard]] constexpr bool IsTerminal() const noexcept {
            return word_index != kMissingWord;
        }
    };

    enum class DataStatus : std::uint32_t { kFoundNewSubstring };

    struct FoundSubstringSendData {
        std::reference_wrapper<const std::vector<ACTNode>> nodes;
        std::reference_wrapper<const std::vector<word_length_t>> words_lengths;
        DataStatus status{};
        std::string_view found_substring{};
        std::size_t substring_start_index{};
        vertex_index_t current_vertex_index{};
    };

    struct BadInputSendData {
        std::size_t symbol_index{};
        char bad_symbol{};
    };

    ACTrie();
    ACTrie& AddPattern(std::string_view pattern);
    bool ContainsPattern(std::string_view pattern) const noexcept;
    ACTrie& FindAllSubstringsInText(std::string_view text);
    ACTrie& ResetACTrie();
    ACTrie& Subscribe(Observer<FoundSubstringSendData>* observer);
    ACTrie& Subscribe(Observer<BadInputSendData>* observer);

    constexpr std::size_t NodesSize() const noexcept {
        return nodes_.size();
    }
    constexpr std::size_t PatternsSize() const noexcept {
        return words_lengths_.size();
    }

private:
    static constexpr std::size_t kDefaultNodesCapacity = 16;
    static constexpr vertex_index_t kNullNodeIndex     = 0;
    static constexpr vertex_index_t kFakePreRootIndex = kNullNodeIndex + 1;
    static constexpr vertex_index_t kRootIndex = kFakePreRootIndex + 1;
    static constexpr vertex_index_t kDefaultNodesCount = kRootIndex + 1;

    static vertex_index_t SymbolToIndex(char symbol) noexcept {
        std::int32_t symbol_as_int = static_cast<std::uint8_t>(symbol);
        if constexpr (kIsCaseInsensetive) {
            symbol_as_int = std::tolower(symbol_as_int);
        }

        return static_cast<vertex_index_t>(symbol_as_int) - kAlphabetStart;
    }

    constexpr bool IsReady() const noexcept {
        return is_ready_;
    }

    void ComputeLinksForNodes();

    void ComputeLinksForNode(ACTNode&, std::queue<vertex_index_t>&);

    bool IsACTrieInCorrectnessState() const;

    std::vector<ACTNode> nodes_;
    std::vector<word_length_t> words_lengths_;
    bool is_ready_ = false;
    Observable<FoundSubstringSendData> found_substrings_port_;
    Observable<BadInputSendData> bad_input_port_;
};

}  // namespace AppSpace::ACTrieDS
