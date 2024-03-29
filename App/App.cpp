#include "App.hpp"

namespace AppSpace {

App::App() : model_controller_(model_) {
    model_.AddSubscriber(view_.GetFoundStringsObserverPort())
        .AddSubscriber(view_.GetBadInputObserverPort());
    view_.AddPatternSubscriber(model_controller_.GetPatternObserverPort())
        .AddTextSubscriber(model_controller_.GetTextObserverPort());
}

void App::Run() {
    imgui_facade_.StartRuntimeLoop([this]() { view_.Draw(); });
}

}  // namespace AppSpace
