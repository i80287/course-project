#include "App.hpp"

#include <ctime>

namespace AppSpace {

App::App() {
    model_.Subscribe(view_.GetFoundStringsObserverPort());
    model_.Subscribe(view_.GetBadInputObserverPort());
}

void App::Run() {
    while (!imgui_facade_.ShouldClose()) {
        imgui_facade_.RenderRuntimeLoopIteration([this]() {
            view_.Draw();
        });
    }
}

}  // namespace AppSpace
