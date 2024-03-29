#pragma once

#include "ACTrie.hpp"
#include "Drawer.hpp"
#include "GraphicFacades/ImGuiFacade.hpp"

namespace AppSpace {

class App {
    using ModelType = ACTrieDS::ACTrie;

public:
    App();
    void Run();
private:
    GraphicFacades::ImGuiFacade imgui_facade_;
    Drawer view_;
    ModelType model_;
};

}  // namespace AppSpace
