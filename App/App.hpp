#pragma once

#include "../GraphicFacades/ImGuiFacade.hpp"
#include "ACTrie.hpp"
#include "ACTrieController.hpp"
#include "Drawer.hpp"

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
    Controller::ACTrieController model_controller_;
};

}  // namespace AppSpace
