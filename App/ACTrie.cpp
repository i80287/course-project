#include "ACTrie.hpp"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <string_view>

namespace AppSpace::ACTrieDS {

ACTrie::ACTrie() {
    nodes_.reserve(kDefaultNodesCapacity);
    nodes_.resize(kInitialNodesCount);
    nodes_[kFakePreRootIndex].edges.fill(kRootIndex);
}

ACTrie& ACTrie::AddPattern(std::string_view pattern) {
    assert(!is_ready_ || notified_about_initial_nodes_);
    if (!notified_about_initial_nodes_) {
        NotifyAboutInitialNodes();
    }
    if (is_ready_) {
        ResetACTrie();
    }

    for (std::size_t i = 0; i < pattern.size(); i++) {
        const char symbol = pattern[i];
        if (SymbolToIndex(symbol) >= kAlphabetLength) {
            bad_input_port_.Notify({i, symbol});
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
        char symbol              = *pattern_iter;
        VertexIndex symbol_index = SymbolToIndex(symbol);
        nodes_.emplace_back();
        nodes_[current_node_index][symbol_index] = new_node_index;
        NotifyAboutAddedNode(new_node_index, current_node_index, symbol);
        current_node_index = new_node_index++;
    }

    nodes_[current_node_index].word_index =
        static_cast<WordLength>(words_lengths_.size());
    words_lengths_.push_back(static_cast<WordLength>(pattern.size()));
    return *this;
}

ACTrie& ACTrie::BuildACTrie() {
    assert(!is_ready_);
    if (!notified_about_initial_nodes_) {
        NotifyAboutInitialNodes();
    }
    nodes_[kRootIndex].suffix_link            = kFakePreRootIndex;
    nodes_[kRootIndex].compressed_suffix_link = kRootIndex;
    NotifyAboutComputedSuffixLinks(kRootIndex, kFakePreRootIndex, '\0');

    std::queue<VertexIndex> bfs_queue;
    bfs_queue.push(kRootIndex);
    do {
        VertexIndex vertex_index = bfs_queue.front();
        bfs_queue.pop();
        ComputeLinksForNodeChildren(vertex_index, bfs_queue);
    } while (!bfs_queue.empty());
    is_ready_ = true;
    return *this;
}

ACTrie& ACTrie::ResetACTrie() {
    is_ready_                     = false;
    notified_about_initial_nodes_ = false;
    nodes_.clear();
    words_lengths_.clear();
    nodes_.resize(kInitialNodesCount);
    nodes_[kFakePreRootIndex].edges.fill(kRootIndex);
    NotifyAboutInitialNodes();
    return *this;
}

ACTrie& ACTrie::FindAllSubstringsInText(std::string_view text) {
    if (!is_ready_) {
        BuildACTrie();
        assert(IsACTrieInCorrectState());
    }

    VertexIndex current_node_index = kRootIndex;
    for (std::size_t i = 0; i < text.size(); i++) {
        VertexIndex symbol_index = SymbolToIndex(text[i]);
        if (symbol_index >= kAlphabetLength) {
            current_node_index = kRootIndex;
            NotifyAboutPassingThroughNode(current_node_index);
            continue;
        }

        current_node_index = nodes_[current_node_index][symbol_index];
        NotifyAboutPassingThroughNode(current_node_index);
        assert(current_node_index != kNullNodeIndex);
        if (nodes_[current_node_index].IsTerminal()) {
            NotifyAboutFoundSubstring(current_node_index, i, text);
        }

        JumpThroughCompressedSuffixLinks(current_node_index, i, text);
    }

    return *this;
}

ACTrie& ACTrie::AddSubscriber(UpdatedNodeObserver* observer) {
    updated_nodes_port_.Subscribe(observer);
    NotifyAboutInitialNodes();
    return *this;
}

ACTrie& ACTrie::AddSubscriber(FoundSubstringObserver* observer) {
    found_substrings_port_.Subscribe(observer);
    return *this;
}

ACTrie& ACTrie::AddSubscriber(BadInputPatternObserver* observer) {
    bad_input_port_.Subscribe(observer);
    return *this;
}

ACTrie& ACTrie::AddSubscriber(PassingThroughObserver* observer) {
    passing_through_port_.Subscribe(observer);
    return *this;
}

void ACTrie::NotifyAboutFoundSubstring(VertexIndex current_node_index,
                                       std::size_t position_in_text,
                                       std::string_view text) {
    auto word_index = nodes_[current_node_index].word_index;
    assert(word_index < words_lengths_.size());
    auto word_length         = words_lengths_[word_index];
    auto word_start_position = position_in_text + 1 - word_length;

    found_substrings_port_.Notify(FoundSubstringInfo{
        .found_substring       = text.substr(word_start_position, word_length),
        .substring_start_index = word_start_position,
        .current_vertex_index  = current_node_index,
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

void ACTrie::ComputeLinksForNodeChildren(VertexIndex node_index,
                                         std::queue<VertexIndex>& bfs_queue) {
    ACTNode& node = nodes_[node_index];
    for (VertexIndex child_node_symbol_index = 0;
         child_node_symbol_index < kAlphabetLength; child_node_symbol_index++) {
        VertexIndex child_link_v_index =
            nodes_[node.suffix_link][child_node_symbol_index];
        assert(child_link_v_index != kNullNodeIndex);
        VertexIndex child_index = node[child_node_symbol_index];
        if (child_index != kNullNodeIndex) {
            nodes_[child_index].suffix_link = child_link_v_index;
            assert(nodes_[child_link_v_index].compressed_suffix_link !=
                   kNullNodeIndex);

            bool suffix_child_is_terminal_or_root =
                nodes_[child_link_v_index].IsTerminal() ||
                child_link_v_index == kRootIndex;
            nodes_[child_index].compressed_suffix_link =
                suffix_child_is_terminal_or_root
                    ? child_link_v_index
                    : nodes_[child_link_v_index].compressed_suffix_link;

            char parent_to_node_edge_symbol =
                IndexToSymbol(child_node_symbol_index);
            NotifyAboutComputedSuffixLinks(child_index, node_index,
                                           parent_to_node_edge_symbol);

            bfs_queue.push(child_index);
        } else {
            node[child_node_symbol_index] = child_link_v_index;
        }
    }
}

bool ACTrie::IsACTrieInCorrectState() const {
    if (nodes_.size() < kInitialNodesCount) {
        return false;
    }
    if (!IsFakePreRootNodeInCorrectState()) {
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

bool ACTrie::IsFakePreRootNodeInCorrectState() const {
    const auto& edges = nodes_[kFakePreRootIndex].edges;
    return std::all_of(edges.begin(), edges.end(), [](VertexIndex child_index) {
        return child_index == kRootIndex;
    });
}

void ACTrie::NotifyAboutAddedNode(VertexIndex added_node_index,
                                  VertexIndex parent_node_index,
                                  char parent_to_node_edge_symbol) {
    updated_nodes_port_.Notify(UpdatedNodeInfo{
        .node_index                 = added_node_index,
        .parent_node_index          = parent_node_index,
        .node                       = nodes_[added_node_index],
        .status                     = UpdatedNodeStatus::kAdded,
        .parent_to_node_edge_symbol = parent_to_node_edge_symbol,
    });
}

void ACTrie::NotifyAboutComputedSuffixLinks(VertexIndex node_index,
                                            VertexIndex parent_node_index,
                                            char parent_to_node_edge_symbol) {
    updated_nodes_port_.Notify(UpdatedNodeInfo{
        .node_index                 = node_index,
        .parent_node_index          = parent_node_index,
        .node                       = nodes_[node_index],
        .status                     = UpdatedNodeStatus::kSuffixLinksComputed,
        .parent_to_node_edge_symbol = parent_to_node_edge_symbol,
    });
}

void ACTrie::NotifyAboutInitialNodes() {
    assert(!notified_about_initial_nodes_);
    NotifyAboutAddedNode(kNullNodeIndex, kNullNodeIndex, '\0');
    NotifyAboutAddedNode(kFakePreRootIndex, kNullNodeIndex, '\0');
    NotifyAboutAddedNode(kRootIndex, kFakePreRootIndex, '\0');
    notified_about_initial_nodes_ = true;
}

void ACTrie::NotifyAboutPassingThroughNode(VertexIndex node_index) {
    passing_through_port_.Notify(node_index);
}

}  // namespace AppSpace::ACTrieDS
