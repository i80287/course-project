#include "App.hpp"

namespace AppSpace {

App::App() { model_.Subscribe(view_.GetObserverPort()); }

bool App::InitFailed() const noexcept { return view_.InitFailed(); }

void App::Run() {
    // Main loop
    while (!view_.ShouldClose()) {
        view_.Render();
    }
}

}  // namespace AppSpace
