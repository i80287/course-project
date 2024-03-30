#pragma once

#include "ACTrie.hpp"
#include "Observer.hpp"

namespace AppSpace::Controller {

class ACTrieController final {
    using ACTrieModel = ACTrieDS::ACTrie;
    using Pattern     = ACTrieModel::Pattern;
    using Text        = ACTrieModel::Text;

public:
    using PatternObserver = Observer<Pattern>;
    using TextObserver    = Observer<Text>;

    ACTrieController(ACTrieModel& host_model);
    PatternObserver* GetPatternObserverPort() noexcept;
    TextObserver* GetTextObserverPort() noexcept;

private:
    ACTrieModel* model_ = nullptr;
    PatternObserver pattern_port_;
    TextObserver text_port_;
};

}  // namespace AppSpace::Controller
