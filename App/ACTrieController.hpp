#pragma once

#include "Observer.hpp"
#include "ACTrie.hpp"

namespace AppSpace::Controller {

class ACTrieController {
    using ACTrieModel = ACTrieDS::ACTrie;
    using Pattern = ACTrieModel::Pattern;
    using Text = ACTrieModel::Text;
public:
    using PatternObserver = Observer<Pattern>;
    using TextObserver = Observer<Text>;

    ACTrieController(ACTrieModel& host_model);
    PatternObserver* GetPatternObserverPort() noexcept;
    TextObserver* GetTextObserverPort() noexcept;
private:
    ACTrieModel* model_ = nullptr;
    PatternObserver pattern_port_;
    TextObserver text_port_;
};

}  // namespace AppSpace::Controller
