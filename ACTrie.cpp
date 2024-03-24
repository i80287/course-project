#include "ACTrie.hpp"

#include <cassert>      // assert
#include <cctype>       // std::tolower
#include <cstdio>       // std::getchar, std::fprintf
#include <deque>        // std::deque
#include <exception>    // std::terminate
#include <iterator>     // std::size
#include <string>       // std::string
#include <string_view>  // std::string_view

#include "app_config.hpp"

namespace AppSpace {

namespace ACTrieDS {

template <char TrieMinChar, char TrieMaxChar, bool IsCaseInsensetive>
ACTrie<TrieMinChar, TrieMaxChar, IsCaseInsensetive>::ACTrie() {
    nodes_.reserve(kDefaultNodesCapacity);
    nodes_.resize(kDefaultNodesCount);
}

template <char TrieMinChar, char TrieMaxChar, bool IsCaseInsensetive>
bool ACTrie<TrieMinChar, TrieMaxChar, IsCaseInsensetive>::ContainsPattern(
    std::string_view pattern) const noexcept {
    vertex_index_t current_node_index = kRootIndex;
    for (char c : pattern) {
        int32_t sigma = int32_t(uint8_t(c));
        if constexpr (kIsCaseInsensetive) {
            sigma = std::tolower(sigma);
        }

        if (!IsInAlphabet(sigma)) {
            return false;
        }

        vertex_index_t next_node_index =
            nodes_[current_node_index][CharToIndex(sigma)];
        if (next_node_index != kNullNodeIndex) {
            current_node_index = next_node_index;
        } else {
            return false;
        }
    }

    return nodes_[current_node_index].IsTerminal();
}

template <char TrieMinChar, char TrieMaxChar, bool IsCaseInsensetive>
void ACTrie<TrieMinChar, TrieMaxChar, IsCaseInsensetive>::ComputeLinks() {
    assert(!IsReady());

    /*
     * See MIPT lecture https://youtu.be/MEFrIcGsw1o for more info
     *
     * Let's denote 'Q' to be the finite set of states
     * Let's denote 'v' to be the state ∈ Q (practically, a vertex in the
     * automaton's graph) Let's denote 'root' to be the start state
     * (practically, a root vertex in the automaton's graph) Let's denote 'Σ' to
     * be the automaton's alphabet Let's denote 'σ' be any symbol ∈ Σ Let's
     * denote 'to' to be the transition function (often written as δ) to: Q x Σ
     * -> Q, initially may not be a total function
     *
     * Let's denote 'link' to be a suffix link function (defines suffix link for
     * each state / vertex) Let's denote 'compressed_suffix_link' to be a
     * compressed suffix link function (defines compressed suffix link for each
     * state / vertex)
     *
     *
     * After adding patterns 'to' may not be a total function.
     * Steps to make 'to' a total function (with domain = Q x Σ):
     *
     * 1)
     *   ∀ σ ∈ Σ: to(fake_preroot, σ) = root
     *
     *   link(root) = fake_preroot
     *   compressed_suffix_link(root) = root
     *
     * 2) For all σ ∈ Σ:
     *
     *                            /
     *                           / to(root, σ) if to(root, σ) is defined
     *   ∀ σ ∈ Σ: to(root, σ) = {
     *                           \ root, otherwise
     *                            \
     *
     *                                   /
     *                                 / root, if to(root, σ) is initially
     * defined ∀ σ ∈ Σ: link(to(root, σ)) = { \ link(root), if to(root, σ) was
     * not initially defined
     *                                  \
     *
     *   ∀ σ ∈ Σ: compressed_suffix_link(to(root, σ)) = root
     */

    nodes_[kRootIndex].suffix_link = kFakePreRootIndex;
    nodes_[kRootIndex].compressed_suffix_link = kRootIndex;

    nodes_[kFakePreRootIndex].edges.fill(kRootIndex);

    // Run BFS through all nodes.
    std::deque<vertex_index_t> bfs_queue;
    bfs_queue.push_back(kRootIndex);

    do {
        // In the comments this node is called 'v'
        vertex_index_t vertex_index = bfs_queue.front();
        bfs_queue.pop_front();

        // vertex_suffix_link_node = to((link(v), sigma))
        ACTNode& node = nodes_[vertex_index];
        const ACTNode& vertex_suffix_link_node = nodes_[node.suffix_link];

        // For each char (sigma) in the Alphabet vertex_suffix_link_node[sigma]
        // is the child such: v --sigma--> child
        for (size_t sigma = 0; sigma != kAlphabetLength; ++sigma) {
            vertex_index_t child_link_v_index = vertex_suffix_link_node[sigma];
            assert(child_link_v_index != kNullNodeIndex);

            // child_index = to(v, sigma)
            vertex_index_t child_index = node[sigma];

            // to((v, sigma)) = to((v, sigma)) if (v, sigma) in the rng(to)
            // else to((link(v), sigma)) rng(to) is a range of function 'to'
            if (child_index != kNullNodeIndex) {
                // link(to(v, sigma)) = to((link(v), sigma)) if (v, sigma)
                // in the rng(to)
                nodes_[child_index].suffix_link = child_link_v_index;

                assert(nodes_[child_link_v_index].compressed_suffix_link !=
                       kNullNodeIndex);

                // comp(v) = link(v) if link(v) is terminal or root else
                // comp(link(v))
                nodes_[child_index].compressed_suffix_link =
                    (!nodes_[child_link_v_index].IsTerminal() &&
                     child_link_v_index != kRootIndex)
                        ? nodes_[child_link_v_index].compressed_suffix_link
                        : child_link_v_index;

                bfs_queue.push_back(child_index);
            } else {
                node[sigma] = child_link_v_index;
            }
        }
    } while (!bfs_queue.empty());

    // Call method iff NDEBUG is not defined
    assert((CheckComputedLinks(), true));
}

template <char TrieMinChar, char TrieMaxChar, bool IsCaseInsensetive>
void ACTrie<TrieMinChar, TrieMaxChar, IsCaseInsensetive>::AddPattern(
    std::string_view pattern) {
    assert(!IsReady());

    vertex_index_t current_node_index = kRootIndex;
    auto pattern_iter = pattern.begin();
    auto pattern_end = pattern.end();
    for (; pattern_iter != pattern_end; ++pattern_iter) {
        int32_t sigma = int32_t(uint8_t(*pattern_iter));
        if constexpr (kIsCaseInsensetive) {
            sigma = std::tolower(sigma);
        }

        if (unlikely(!IsInAlphabet(sigma))) {
            PatternCharOutOfBounds(sigma);
        }

        vertex_index_t next_node_index =
            nodes_[current_node_index][CharToIndex(sigma)];
        if (next_node_index != kNullNodeIndex) {
            current_node_index = next_node_index;
        } else {
            break;
        }
    }

    size_t lasted_max_length = size_t(pattern_end - pattern_iter);
    nodes_.reserve(nodes_.size() + lasted_max_length);

    /*
     * Inserts substring [i..length - 1] of pattern if i < length (<=> i !=
     * length) If i == length, then for cycle will no execute
     */
    for (vertex_index_t new_node_index = vertex_index_t(nodes_.size());
         pattern_iter != pattern_end; ++pattern_iter) {
        int32_t sigma = int32_t(uint8_t(*pattern_iter));
        if constexpr (kIsCaseInsensetive) {
            sigma = std::tolower(sigma);
        }

        if (unlikely(!IsInAlphabet(sigma))) {
            PatternCharOutOfBounds(sigma);
        }

        nodes_.emplace_back();
        nodes_[current_node_index][CharToIndex(sigma)] = new_node_index;
        current_node_index = new_node_index++;
    }

    nodes_[current_node_index].word_index =
        word_length_t(words_lengths_.size());
    words_lengths_.push_back(word_length_t(pattern.size()));
}

template <char TrieMinChar, char TrieMaxChar, bool IsCaseInsensetive>
template <class FindCallback>
#if __cplusplus >= 202002L
    requires RunTextFindCallback<FindCallback>
#endif
void ACTrie<TrieMinChar, TrieMaxChar, IsCaseInsensetive>::RunText(
    std::string_view text, FindCallback find_callback) const {
    assert(IsReady());

    if constexpr (std::is_pointer_v<decltype(find_callback)>) {
        assert(find_callback != nullptr);
    }

    vertex_index_t current_node_index = kRootIndex;
    size_t i = 0;
    for (auto iter = text.begin(), end = text.end(); iter != end; ++iter, ++i) {
        int32_t sigma = int32_t(uint8_t(*iter));
        if constexpr (kIsCaseInsensetive) {
            sigma = std::tolower(sigma);
        }

        if (!IsInAlphabet(sigma)) {
            current_node_index = kRootIndex;
            continue;
        }

        current_node_index = nodes_[current_node_index][CharToIndex(sigma)];
        assert(current_node_index != kNullNodeIndex);
        if (nodes_[current_node_index].IsTerminal()) {
            size_t word_index = nodes_[current_node_index].word_index;
            assert(word_index < words_lengths_.size());
            size_t word_length = words_lengths_[word_index];
            size_t l = i + 1 - word_length;
            find_callback(text.substr(l, word_length), l);
        }

        // Jump up through compressed suffix links
        for (vertex_index_t tmp_node_index =
                 nodes_[current_node_index].compressed_suffix_link;
             tmp_node_index != kRootIndex;
             tmp_node_index = nodes_[tmp_node_index].compressed_suffix_link) {
            assert(tmp_node_index != kNullNodeIndex &&
                   nodes_[tmp_node_index].IsTerminal());
            size_t word_index = nodes_[tmp_node_index].word_index;
            assert(word_index < words_lengths_.size());
            size_t word_length = words_lengths_[word_index];
            size_t l = i + 1 - word_length;
            find_callback(text.substr(l, word_length), l);
        }
    }
}

template <char TrieMinChar, char TrieMaxChar, bool IsCaseInsensetive>
void ACTrie<TrieMinChar, TrieMaxChar, IsCaseInsensetive>::CheckComputedLinks()
    const {
    auto iter = nodes_.begin();

    size_t max_node_index_excluding = nodes_.size();
    assert(max_node_index_excluding >= kDefaultNodesCount);
    size_t max_word_end_index_excl = words_lengths_.size();

    static_assert(
        kNullNodeIndex == kNullNodeIndex,
        "Current impl of CheckComputedLinks() relies on kNullNodeIndex");
    // Skip null node
    ++iter;
    // Now iter points on fake preroot node
    // fake preroot node does not have suffix_link_index and
    // compressed_suffix_link all children point to root
    for (vertex_index_t child_index : iter->edges) {
        assert(child_index == kRootIndex);
    }

    ++iter;
    // Now iter points on the root node
    for (auto iter_end = nodes_.end(); iter != iter_end; ++iter) {
        static_assert(kNullNodeIndex < kFakePreRootIndex);

        for (vertex_index_t child_index : iter->edges) {
            assert(child_index >= kFakePreRootIndex &&
                   child_index < max_node_index_excluding);
        }

        vertex_index_t suffix_link_index = iter->suffix_link;
        assert(suffix_link_index >= kFakePreRootIndex &&
               suffix_link_index < max_node_index_excluding);

        vertex_index_t compressed_suffix_link_index =
            iter->compressed_suffix_link;
        assert(compressed_suffix_link_index >= kFakePreRootIndex &&
               compressed_suffix_link_index < max_node_index_excluding);

        assert(!iter->IsTerminal() ||
               iter->word_index < max_word_end_index_excl);
    }

    are_links_computed_ = true;
}

template <char TrieMinChar, char TrieMaxChar, bool IsCaseInsensetive>
void ACTrie<TrieMinChar, TrieMaxChar,
            IsCaseInsensetive>::PatternCharOutOfBounds(int32_t chr) noexcept {
    std::fprintf(stderr,
                 "error in ACTrie::AddPattern(std::string_view): char %c from "
                 "input pattern "
                 "is not in the alphabet ['%c'; '%c']",
                 char(chr), TrieMinChar, TrieMaxChar);
    std::terminate();
}

template <char TrieMinChar, char TrieMaxChar, bool IsCaseInsensetive>
bool ACTrie<TrieMinChar, TrieMaxChar, IsCaseInsensetive>::IsReady()
    const noexcept {
    return are_links_computed_;
}

template <char TrieMinChar, char TrieMaxChar, bool IsCaseInsensetive>
size_t ACTrie<TrieMinChar, TrieMaxChar, IsCaseInsensetive>::NodesSize()
    const noexcept {
    return nodes_.size();
}

template <char TrieMinChar, char TrieMaxChar, bool IsCaseInsensetive>
size_t ACTrie<TrieMinChar, TrieMaxChar, IsCaseInsensetive>::PatternsSize()
    const noexcept {
    return words_lengths_.size();
}

template <char TrieMinChar, char TrieMaxChar, bool IsCaseInsensetive>
void ACTrie<TrieMinChar, TrieMaxChar, IsCaseInsensetive>::
    ReservePlaceForPatterns(size_t patterns_capacity) {
    words_lengths_.reserve(patterns_capacity);
}

template <char TrieMinChar, char TrieMaxChar, bool IsCaseInsensetive>
void ACTrie<TrieMinChar, TrieMaxChar, IsCaseInsensetive>::Subscribe(
    ObserverType* observer) {
    port_.Subscribe(observer, {&this->nodes_, &this->words_lengths_});
}

}  // namespace ACTrieDS

template class ACTrieDS::ACTrie<kMinTrieChar, kMaxTrieChar, kIsCaseInsensetive>;

}  // namespace AppSpace
