#pragma once

#include "../GraphicsUtils/ImGuiFacade.hpp"
#include "ACTrie.hpp"
#include "ACTrieController.hpp"
#include "../GraphicsUtils/Drawer.hpp"

namespace AppSpace {

class App final {
    using ModelType = ACTrieDS::ACTrie;

public:
    App();
    void Run();

private:
    GraphicsUtils::ImGuiFacade imgui_facade_;
    Drawer view_;
    ModelType model_;
    Controller::ACTrieController model_controller_;
};

}  // namespace AppSpace
