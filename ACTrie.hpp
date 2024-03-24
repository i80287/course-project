#pragma once

#include <array>
#include <climits>
#include <cstdint>
#include <iterator>
#include <string_view>
#include <vector>

#include "Observable.hpp"
#include "app_config.hpp"

namespace AppSpace {

namespace ACTrieDS {

using std::size_t;
using std::uint32_t;

#if __cplusplus >= 202002L
template <class FindCallback>
concept RunTextFindCallback =
    requires(FindCallback func, std::string_view found_word,
             size_t start_index_in_original_text) {
        func(found_word, start_index_in_original_text);
    };
#endif

template <char TrieMinChar = 'A', char TrieMaxChar = 'z',
          bool IsCaseInsensetive = false>
class ACTrie final {
    static_assert('\0' < TrieMinChar && TrieMinChar < TrieMaxChar &&
                      TrieMaxChar <= CHAR_MAX,
                  "Incorrect min and max chars in ACTrie");

public:
    using vertex_index_t = uint32_t;
    using word_length_t = uint32_t;

    static constexpr bool kIsCaseInsensetive = IsCaseInsensetive;
    static constexpr uint8_t kAlphabetStart = uint8_t(TrieMinChar);
    static constexpr uint8_t kAlphabetEnd = uint8_t(TrieMaxChar);
    // Default value = 'z' - 'A' + 1 = 58
    static constexpr uint8_t kAlphabetLength =
        kAlphabetEnd - kAlphabetStart + 1;
    static constexpr size_t kDefaultNodesCapacity = 16;

    static constexpr vertex_index_t kNullNodeIndex = 0;
    static constexpr vertex_index_t kFakePreRootIndex = 1;
    static constexpr vertex_index_t kRootIndex = 2;
    // Three nodes: null node, fake preroot node and root node
    static constexpr vertex_index_t kDefaultNodesCount = 3;

    static_assert(std::max(kNullNodeIndex,
                           std::max(kFakePreRootIndex, kRootIndex)) <
                  kDefaultNodesCount);

    struct ACTNode {
        static constexpr uint32_t kMissingWord = uint32_t(-1);

        // Indexes in array of nodes
        std::array<vertex_index_t, kAlphabetLength> edges{vertex_index_t(kNullNodeIndex)};

        // Index in array of nodes
        vertex_index_t suffix_link = kNullNodeIndex;

        // Index in array of nodes
        vertex_index_t compressed_suffix_link = kNullNodeIndex;

        /*
         * Index of the word in the ac trie which ends on this
         * kMissingWord if node is not terminal
         */
        uint32_t word_index = kMissingWord;

        vertex_index_t operator[](size_t sigma) const noexcept {
            assert(sigma < kAlphabetLength);
            return edges[sigma];
        }

        vertex_index_t& operator[](size_t sigma) noexcept {
            assert(sigma < kAlphabetLength);
            return edges[sigma];
        }

        [[nodiscard]] constexpr bool IsTerminal() const noexcept {
            return word_index != kMissingWord;
        }
    };

    enum class DrawStatus : uint32_t {};

    using SubscribeData =
        std::pair<std::vector<ACTNode>*, std::vector<uint32_t>*>;
    using SendData = std::pair<vertex_index_t, DrawStatus>;
    using ObservableType = Observable<SubscribeData, SendData>;
    using ObserverType = ObservableType::ObserverType;
    using SubscribeDataType = ObserverType::SubscribeDataType;

    ACTrie();

    void AddPattern(std::string_view pattern);

    bool ContainsPattern(std::string_view pattern) const noexcept;

    void ComputeLinks();

    size_t NodesSize() const noexcept;

    size_t PatternsSize() const noexcept;

    void ReservePlaceForPatterns(size_t patterns_capacity);

    template <class FindCallback>
#if __cplusplus >= 202002L
        requires RunTextFindCallback<FindCallback>
#endif
    void RunText(std::string_view text, FindCallback find_callback) const;

    void Subscribe(ObserverType* observable);

private:
    static constexpr vertex_index_t CharToIndex(int32_t chr) noexcept {
        return vertex_index_t(chr) - kAlphabetStart;
    }

    static constexpr bool IsInAlphabet(int32_t chr) noexcept {
        return uint32_t(chr) - kAlphabetStart <= kAlphabetEnd - kAlphabetStart;
    }

    /// @brief Method to check whether ComputeLinks method was called or not.
    bool IsReady() const noexcept;

    /// @brief Method to check class' consistency and correct structure of the
    /// actrie after ComputeLinks method call.
    void CheckComputedLinks() const;

    /// @brief Method that prints error message to the stderr and calls
    /// std::terminate
    /// @param chr char from the pattern which is not in the state machine's
    /// alphabet
    [[noreturn]] static void PatternCharOutOfBounds(int32_t chr) noexcept;

    std::vector<ACTNode> nodes_;
    std::vector<word_length_t> words_lengths_;
    mutable bool are_links_computed_ = false;
    ObservableType port_;
};

}  // namespace ACTrieDS

}  // namespace AppSpace
