#include "App.hpp"

namespace AppSpace {

App::App() : model_controller_(model_) {
    model_.AddSubscriber(view_.GetFoundStringObserverPort())
        .AddSubscriber(view_.GetBadInputPatternObserverPort())
        .AddSubscriber(view_.GetUpdatedNodeObserverPort());
    view_.AddPatternSubscriber(model_controller_.GetPatternObserverPort())
        .AddTextSubscriber(model_controller_.GetTextObserverPort())
        .AddACTrieResetSubscriber(
            model_controller_.GetACTrieResetObserverPort())
        .AddACTrieBuildSubscriber(
            model_controller_.GetACTrieBuildObserverPort());
}

void App::Run() {
    imgui_facade_.StartRuntimeLoop([this]() { view_.OnNewFrame(); });
}

}  // namespace AppSpace
