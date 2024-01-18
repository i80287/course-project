#pragma once

#include "ACTrie.hpp"
#include "Drawer.hpp"
#include "app_config.hpp"

namespace AppSpace {

class App {
    using ModelType =
        ACTrieDS::ACTrie<kMinTrieChar, kMaxTrieChar, kIsCaseInsensetive>;

public:
    App();
    void Run();
    [[nodiscard]] bool InitFailed() const noexcept;

private:
    Drawer view_;
    ModelType model_;
};

}  // namespace AppSpace
