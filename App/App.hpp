#pragma once

#include "../GraphicsUtils/Drawer.hpp"
#include "../GraphicsUtils/ImGuiFacade.hpp"
#include "ACTrie.hpp"
#include "ACTrieController.hpp"

namespace AppSpace {

class App final {
    using ModelType   = ACTrieDS::ACTrie;
    using ImGuiFacade = GraphicsUtils::ImGuiFacade;
    using Drawer      = GraphicsUtils::Drawer;

public:
    App();
    void Run();

private:
    GraphicsUtils::ImGuiFacade imgui_facade_;
    GraphicsUtils::Drawer view_;
    ModelType model_;
    ACTrieController model_controller_;
};

}  // namespace AppSpace
