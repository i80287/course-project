#pragma once

#include "ACTrie.hpp"
#include "Observer.hpp"

namespace AppSpace {

class Drawer final {
    using FoundSubstringInfo     = ACTrieDS::ACTrie::FoundSubstringInfo;
    using BadInputPatternInfo    = ACTrieDS::ACTrie::BadInputPatternInfo;
    using Pattern                = ACTrieDS::ACTrie::Pattern;
    using Text                   = ACTrieDS::ACTrie::Text;
    using PatternObserver        = Observer<Pattern>;
    using TextObserver           = Observer<Text>;
    using FoundSubstringObserver = Observer<FoundSubstringInfo>;
    using BadInputObserver       = Observer<BadInputPatternInfo>;
    using FoundSubstringSentBy   = FoundSubstringObserver::SendTDataBy;
    using BadInputSentBy         = BadInputObserver::SendTDataBy;

public:
    Drawer();
    void Draw();

    FoundSubstringObserver* GetFoundStringsObserverPort() noexcept;
    BadInputObserver* GetBadInputObserverPort() noexcept;
    Drawer& AddPatternSubscriber(PatternObserver* observer);
    Drawer& AddTextSubscriber(TextObserver* observer);

private:
    void SetupImGuiStyle();
    void OnFoundSubstring(FoundSubstringSentBy substring_info);
    void OnBadPatternInput(BadInputSentBy bad_input_info);

    FoundSubstringObserver found_strings_port_;
    BadInputObserver bad_input_port_;
    Observable<Pattern> user_pattern_input_port_;
    Observable<Text> user_text_input_port_;
    bool suppressor_ = false;
};

}  // namespace AppSpace
