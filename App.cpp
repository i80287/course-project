#include "App.hpp"

namespace AppSpace {

App::App() {
    model_.Subscribe(view_.GetFoundStringsObserverPort());
    model_.Subscribe(view_.GetBadInputObserverPort());
}

void App::Run() {
    // Main loop
    while (!view_.ShouldClose()) {
        view_.Render();
    }
}

}  // namespace AppSpace
