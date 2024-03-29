#include "ACTrie.hpp"

#include <cassert>
#include <cctype>
#include <cstdio>
#include <exception>
#include <iterator>
#include <string>
#include <string_view>

namespace AppSpace::ACTrieDS {

ACTrie::ACTrie()
    : pattern_in_port_([this](Pattern pattern) { AddPattern(pattern); }),
      text_in_port_([this](Text text) { FindAllSubstringsInText(text); }) {
    nodes_.reserve(kDefaultNodesCapacity);
    nodes_.resize(kDefaultNodesCount);
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

    VertexIndex current_node_index = kRootIndex;
    auto pattern_iter              = pattern.begin();
    auto pattern_end               = pattern.end();
    for (; pattern_iter != pattern_end; ++pattern_iter) {
        VertexIndex symbol_index    = SymbolToIndex(*pattern_iter);
        VertexIndex next_node_index = nodes_[current_node_index][symbol_index];
        if (next_node_index != kNullNodeIndex) {
            current_node_index = next_node_index;
        } else {
            break;
        }
    }

    size_t lasted_max_length = size_t(pattern_end - pattern_iter);
    nodes_.reserve(nodes_.size() + lasted_max_length);

    for (VertexIndex new_node_index = VertexIndex(nodes_.size());
         pattern_iter != pattern_end; ++pattern_iter) {
        VertexIndex symbol_index = SymbolToIndex(*pattern_iter);
        nodes_.emplace_back();
        nodes_[current_node_index][symbol_index] = new_node_index;
        current_node_index                       = new_node_index++;
    }

    nodes_[current_node_index].word_index = WordLength(words_lengths_.size());
    words_lengths_.push_back(WordLength(pattern.size()));
    return *this;
}

ACTrie& ACTrie::FindAllSubstringsInText(std::string_view text) {
    if (!IsReady()) {
        ComputeLinksForNodes(nodes_);
        assert(IsACTrieInCorrectState());
        is_ready_ = true;
    }

    VertexIndex current_node_index = kRootIndex;
    for (std::size_t i = 0; i < text.size(); i++) {
        VertexIndex symbol_index = SymbolToIndex(text[i]);
        if (symbol_index >= kAlphabetLength) {
            current_node_index = kRootIndex;
            continue;
        }

        current_node_index = nodes_[current_node_index][symbol_index];
        assert(current_node_index != kNullNodeIndex);
        if (nodes_[current_node_index].IsTerminal()) {
            NotifyAboutFoundSubstring(current_node_index, i, text);
        }

        JumpThroughCompressedSuffixLinks(current_node_index, i, text);
    }

    return *this;
}

ACTrie& ACTrie::ResetACTrie() {
    nodes_.clear();
    words_lengths_.clear();
    nodes_.resize(kDefaultNodesCapacity);
    is_ready_ = false;
    return *this;
}

ACTrie& ACTrie::AddSubscriber(Observer<FoundSubstringInfo>* observer) {
    found_substrings_port_.Subscribe(observer);
    return *this;
}

ACTrie& ACTrie::AddSubscriber(Observer<BadInputPatternInfo>* observer) {
    bad_input_port_.Subscribe(observer);
    return *this;
}

void ACTrie::NotifyAboutFoundSubstring(VertexIndex current_node_index,
                                       std::size_t position_in_text,
                                       std::string_view text) {
    auto word_index = nodes_[current_node_index].word_index;
    assert(word_index < words_lengths_.size());
    auto word_length         = words_lengths_[word_index];
    auto word_start_position = position_in_text + 1 - word_length;

    found_substrings_port_.SetDataAndNotify(FoundSubstringInfo{
        .nodes                = nodes_,
        .words_lengths        = words_lengths_,
        .status               = DataStatus::kFoundNewSubstring,
        .found_substring      = text.substr(word_start_position, word_length),
        .current_vertex_index = current_node_index,
    });
}

void ACTrie::JumpThroughCompressedSuffixLinks(VertexIndex current_node_index,
                                              std::size_t position_in_text,
                                              std::string_view text) {
    for (VertexIndex terminal_node_index =
             nodes_[current_node_index].compressed_suffix_link;
         terminal_node_index != kRootIndex;
         terminal_node_index =
             nodes_[terminal_node_index].compressed_suffix_link) {
        assert(terminal_node_index != kNullNodeIndex);
        assert(nodes_[terminal_node_index].IsTerminal());
        NotifyAboutFoundSubstring(terminal_node_index, position_in_text, text);
    }
}

ACTrie::VertexIndex ACTrie::SymbolToIndex(char symbol) noexcept {
    std::int32_t symbol_as_int = static_cast<std::uint8_t>(symbol);
    if constexpr (kIsCaseInsensetive) {
        symbol_as_int = std::tolower(symbol_as_int);
    }

    return static_cast<VertexIndex>(symbol_as_int) - kAlphabetStart;
}

void ACTrie::ComputeLinksForNodes(std::vector<ACTNode>& nodes) {
    nodes[kRootIndex].suffix_link            = kFakePreRootIndex;
    nodes[kRootIndex].compressed_suffix_link = kRootIndex;
    nodes[kFakePreRootIndex].edges.fill(kRootIndex);
    std::queue<VertexIndex> bfs_queue;
    bfs_queue.push(kRootIndex);
    do {
        VertexIndex vertex_index = bfs_queue.front();
        bfs_queue.pop();
        ComputeLinksForNodeChildren(nodes[vertex_index], nodes, bfs_queue);
    } while (!bfs_queue.empty());
}

void ACTrie::ComputeLinksForNodeChildren(ACTNode& node,
                                         std::vector<ACTNode>& nodes,
                                         std::queue<VertexIndex>& bfs_queue) {
    for (std::size_t child_node_index = 0; child_node_index < kAlphabetLength;
         child_node_index++) {
        VertexIndex child_link_v_index =
            nodes[node.suffix_link][child_node_index];
        assert(child_link_v_index != kNullNodeIndex);
        VertexIndex child_index = node[child_node_index];
        if (child_index != kNullNodeIndex) {
            nodes[child_index].suffix_link = child_link_v_index;
            assert(nodes[child_link_v_index].compressed_suffix_link !=
                   kNullNodeIndex);
            nodes[child_index].compressed_suffix_link =
                (!nodes[child_link_v_index].IsTerminal() &&
                 child_link_v_index != kRootIndex)
                    ? nodes[child_link_v_index].compressed_suffix_link
                    : child_link_v_index;
            bfs_queue.push(child_index);
        } else {
            node[child_node_index] = child_link_v_index;
        }
    }
}

bool ACTrie::IsACTrieInCorrectState() const {
    if (nodes_.size() < kDefaultNodesCount) {
        return false;
    }
    if (!IsFakePrerootInCorrectState()) {
        return false;
    }

    auto is_index_correct = [&](VertexIndex index) constexpr noexcept {
        return index >= kFakePreRootIndex && index < nodes_.size();
    };
    for (auto iter = nodes_.begin() + kRootIndex, iter_end = nodes_.end();
         iter != iter_end; ++iter) {
        if (!std::all_of(iter->edges.begin(), iter->edges.end(),
                         is_index_correct)) {
            return false;
        }
        if (!is_index_correct(iter->suffix_link)) {
            return false;
        }
        if (!is_index_correct(iter->compressed_suffix_link)) {
            return false;
        }
        if (iter->IsTerminal() && iter->word_index >= words_lengths_.size()) {
            return false;
        }
    }

    return true;
}

bool ACTrie::IsFakePrerootInCorrectState() const {
    const auto& edges = nodes_[kFakePreRootIndex].edges;
    return std::all_of(edges.begin(), edges.end(), [](VertexIndex child_index) {
        return child_index == kRootIndex;
    });
}

}  // namespace AppSpace::ACTrieDS
