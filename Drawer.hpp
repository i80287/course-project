#pragma once

#include "ACTrie.hpp"
#include "Observer.hpp"

namespace AppSpace {

class Drawer final {
public:
    using FoundSubstringData = ACTrieDS::ACTrie::FoundSubstringSendData;
    using BadInputStringData = ACTrieDS::ACTrie::BadInputSendData;
    using FoundSubstringObserver = Observer<FoundSubstringData>;
    using BadInputObserver       = Observer<BadInputStringData>;

    Drawer();

    void Draw();

    FoundSubstringObserver* GetFoundStringsObserverPort() noexcept;
    BadInputObserver* GetBadInputObserverPort() noexcept;

private:
    FoundSubstringObserver found_strings_port_;
    BadInputObserver bad_input_port_;
    bool suppressor_ = false;
};

}  // namespace AppSpace
