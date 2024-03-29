#pragma once

#include "ACTrie.hpp"
#include "Drawer.hpp"

namespace AppSpace {

class App {
    using ModelType = ACTrieDS::ACTrie;

public:
    App();
    void Run();
private:
    Drawer view_;
    ModelType model_;
};

}  // namespace AppSpace
