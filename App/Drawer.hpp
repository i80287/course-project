#pragma once

#include "ACTrie.hpp"
#include "Observer.hpp"

namespace AppSpace {

class Drawer final {
    using FoundSubstringInfo  = ACTrieDS::ACTrie::FoundSubstringInfo;
    using BadInputPatternInfo = ACTrieDS::ACTrie::BadInputPatternInfo;
    using Pattern             = ACTrieDS::ACTrie::Pattern;
    using Text                = ACTrieDS::ACTrie::Text;
    using PatternObserver     = Observer<Pattern>;
    using TextObserver        = Observer<Text>;

public:
    using FoundSubstringObserver = Observer<FoundSubstringInfo>;
    using BadInputObserver       = Observer<BadInputPatternInfo>;

    Drawer();
    void Draw();

    FoundSubstringObserver* GetFoundStringsObserverPort() noexcept;
    BadInputObserver* GetBadInputObserverPort() noexcept;
    Drawer& AddPatternSubscriber(PatternObserver* observer);
    Drawer& AddTextSubscriber(TextObserver* observer);

private:
    FoundSubstringObserver found_strings_port_;
    BadInputObserver bad_input_port_;
    Observable<Pattern> user_pattern_input_port_;
    Observable<Text> user_text_input_port_;
    bool suppressor_ = false;
};

}  // namespace AppSpace
