#include "ACTrie.hpp"

#include <cassert>
#include <cctype>
#include <cstdio>
#include <exception>
#include <iterator>
#include <string>
#include <string_view>

namespace AppSpace::ACTrieDS {

ACTrie::ACTrie() {
    nodes_.reserve(kDefaultNodesCapacity);
    nodes_.resize(kDefaultNodesCount);
}

ACTrie& ACTrie::ResetACTrie() {
    nodes_.clear();
    words_lengths_.clear();
    nodes_.resize(kDefaultNodesCapacity);
    return *this;
}

void ACTrie::ComputeLinksForNode(ACTNode& node,
                                 std::queue<vertex_index_t>& bfs_queue) {
    for (std::size_t child_node_index = 0; child_node_index < kAlphabetLength;
         child_node_index++) {
        vertex_index_t child_link_v_index =
            nodes_[node.suffix_link][child_node_index];
        assert(child_link_v_index != kNullNodeIndex);

        // child_index = to(v, sigma)
        vertex_index_t child_index = node[child_node_index];

        // to((v, sigma)) = to((v, sigma)) if (v, sigma) in the rng(to)
        // else to((link(v), sigma)) rng(to) is a range of function
        // 'to'
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

            bfs_queue.push(child_index);
        } else {
            node[child_node_index] = child_link_v_index;
        }
    }
}

void ACTrie::ComputeLinksForNodes() {
    if (IsReady()) {
        return;
    }

    /*
     * See MIPT lecture https://youtu.be/MEFrIcGsw1o for more info
     *
     * Let's denote 'Q' to be the finite set of states
     * Let's denote 'v' to be the state ∈ Q (practically, a vertex in the
     * automaton's graph) Let's denote 'root' to be the start state
     * (practically, a root vertex in the automaton's graph) Let's denote
     * 'Σ' to be the automaton's alphabet Let's denote 'σ' be any symbol ∈
     * Σ Let's denote 'to' to be the transition function (often written as
     * δ) to: Q x Σ
     * -> Q, initially may not be a total function
     *
     * Let's denote 'link' to be a suffix link function (defines suffix
     * link for each state / vertex) Let's denote 'compressed_suffix_link'
     * to be a compressed suffix link function (defines compressed suffix
     * link for each state / vertex)
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
     * defined ∀ σ ∈ Σ: link(to(root, σ)) = { \ link(root), if to(root, σ)
     * was not initially defined
     *                                  \
     *
     *   ∀ σ ∈ Σ: compressed_suffix_link(to(root, σ)) = root
     */

    nodes_[kRootIndex].suffix_link            = kFakePreRootIndex;
    nodes_[kRootIndex].compressed_suffix_link = kRootIndex;

    nodes_[kFakePreRootIndex].edges.fill(kRootIndex);

    std::queue<vertex_index_t> bfs_queue;
    bfs_queue.push(kRootIndex);
    do {
        // In the comments this node is called 'v'
        vertex_index_t vertex_index = bfs_queue.front();
        bfs_queue.pop();
        // vertex_suffix_link_node = to((link(v), sigma))
        ComputeLinksForNode(nodes_[vertex_index], bfs_queue);
    } while (!bfs_queue.empty());
    assert(IsACTrieInCorrectnessState());
}

ACTrie& ACTrie::AddPattern(std::string_view pattern) {
    if (IsReady()) {
        ResetACTrie();
    }

    for (std::size_t i = 0; i < pattern.size(); i++) {
        const char symbol = pattern[i];
        if (SymbolToIndex(symbol) >= kAlphabetLength) {
            bad_input_port_.SetDataAndNotify({i, symbol});
            return *this;
        }
    }

    vertex_index_t current_node_index = kRootIndex;
    auto pattern_iter                 = pattern.begin();
    auto pattern_end                  = pattern.end();
    for (; pattern_iter != pattern_end; ++pattern_iter) {
        vertex_index_t symbol_index = SymbolToIndex(*pattern_iter);
        vertex_index_t next_node_index =
            nodes_[current_node_index][symbol_index];
        if (next_node_index != kNullNodeIndex) {
            current_node_index = next_node_index;
        } else {
            break;
        }
    }

    size_t lasted_max_length = size_t(pattern_end - pattern_iter);
    nodes_.reserve(nodes_.size() + lasted_max_length);

    for (vertex_index_t new_node_index = vertex_index_t(nodes_.size());
         pattern_iter != pattern_end; ++pattern_iter) {
        vertex_index_t symbol_index = SymbolToIndex(*pattern_iter);
        nodes_.emplace_back();
        nodes_[current_node_index][symbol_index] = new_node_index;
        current_node_index                       = new_node_index++;
    }

    nodes_[current_node_index].word_index =
        word_length_t(words_lengths_.size());
    words_lengths_.push_back(word_length_t(pattern.size()));
    return *this;
}

ACTrie& ACTrie::FindAllSubstringsInText(std::string_view text) {
    if (!IsReady()) {
        ComputeLinksForNodes();
    }

    vertex_index_t current_node_index = kRootIndex;
    for (std::size_t i = 0; i < text.size(); i++) {
        const char symbol           = text[i];
        vertex_index_t symbol_index = SymbolToIndex(symbol);
        if (symbol_index >= kAlphabetLength) {
            current_node_index = kRootIndex;
            continue;
        }

        current_node_index = nodes_[current_node_index][symbol_index];
        assert(current_node_index != kNullNodeIndex);
        if (nodes_[current_node_index].IsTerminal()) {
            auto word_index = nodes_[current_node_index].word_index;
            assert(word_index < words_lengths_.size());
            auto word_length         = words_lengths_[word_index];
            auto word_start_position = i + 1 - word_length;
            found_substrings_port_.SetDataAndNotify(FoundSubstringSendData{
                .nodes         = nodes_,
                .words_lengths = words_lengths_,
                .status        = DataStatus::kFoundNewSubstring,
                .found_substring =
                    text.substr(word_start_position, word_length),
                .current_vertex_index = current_node_index,
            });
        }

        // Jump up through compressed suffix links
        for (vertex_index_t terminal_node_index =
                 nodes_[current_node_index].compressed_suffix_link;
             terminal_node_index != kRootIndex;
             terminal_node_index =
                 nodes_[terminal_node_index].compressed_suffix_link) {
            assert(terminal_node_index != kNullNodeIndex);
            assert(nodes_[terminal_node_index].IsTerminal());
            auto word_index = nodes_[terminal_node_index].word_index;
            assert(word_index < words_lengths_.size());
            auto word_length         = words_lengths_[word_index];
            auto word_start_position = i + 1 - word_length;
            found_substrings_port_.SetDataAndNotify(FoundSubstringSendData{
                .nodes         = nodes_,
                .words_lengths = words_lengths_,
                .status        = DataStatus::kFoundNewSubstring,
                .found_substring =
                    text.substr(word_start_position, word_length),
                .current_vertex_index = current_node_index,
            });
        }
    }

    return *this;
}

bool ACTrie::IsACTrieInCorrectnessState() const {
    if (nodes_.size() < kDefaultNodesCount)
        return false;
    auto iter = nodes_.begin();
    // We skip first node (null node) because it doesn't
    //  have any specific state that should be checked
    ++iter;
    if (iter != nodes_.begin() + kFakePreRootIndex)
        return false;
    for (vertex_index_t child_index : iter->edges) {
        if (child_index != kRootIndex)
            return false;
    }

    ++iter;
    if (iter != nodes_.begin() + kRootIndex)
        return false;
    auto is_index_correct = [&](vertex_index_t index) constexpr noexcept {
        return index >= kFakePreRootIndex && index < nodes_.size();
    };
    for (auto iter_end = nodes_.end(); iter != iter_end; ++iter) {
        for (vertex_index_t child_index : iter->edges) {
            if (!is_index_correct(child_index))
                return false;
        }
        if (!is_index_correct(iter->suffix_link))
            return false;
        if (!is_index_correct(iter->compressed_suffix_link))
            return false;
        if (iter->IsTerminal() && iter->word_index >= words_lengths_.size())
            return false;
    }

    return true;
}

ACTrie& ACTrie::Subscribe(Observer<FoundSubstringSendData>* observer) {
    found_substrings_port_.Subscribe(observer);
    return *this;
}

ACTrie& ACTrie::Subscribe(Observer<BadInputSendData>* observer) {
    bad_input_port_.Subscribe(observer);
    return *this;
}

}  // namespace AppSpace::ACTrieDS
