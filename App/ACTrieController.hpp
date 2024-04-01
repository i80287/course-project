#pragma once

#include "ACTrie.hpp"
#include "Observer.hpp"

namespace AppSpace::Controller {

class ACTrieController final {
    using ACTrieModel = ACTrieDS::ACTrie;
    using Pattern     = ACTrieModel::Pattern;
    using Text        = ACTrieModel::Text;

public:
    using PatternObserver     = Observer<Pattern>;
    using TextObserver        = Observer<Text>;
    using ACTrieResetObserver = Observer<void, void>;

    ACTrieController(ACTrieModel& host_model);
    PatternObserver* GetPatternObserverPort() noexcept;
    TextObserver* GetTextObserverPort() noexcept;
    ACTrieResetObserver* GetACTrieResetObserverPort() noexcept;

private:
    ACTrieModel* model_ = nullptr;
    PatternObserver pattern_port_;
    TextObserver text_port_;
    ACTrieResetObserver actrie_reset_port_;
};

}  // namespace AppSpace::Controller
