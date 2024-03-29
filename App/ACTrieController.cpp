#include "ACTrieController.hpp"

namespace AppSpace::Controller {

ACTrieController::ACTrieController(ACTrieModel& host_model)
    : model_(&host_model),
      pattern_port_([this](Pattern pattern) { model_->AddPattern(pattern); }),
      text_port_([this](Text text) { model_->FindAllSubstringsInText(text); }) {
}

ACTrieController::PatternObserver*
ACTrieController::GetPatternObserverPort() noexcept {
    return &pattern_port_;
}

ACTrieController::TextObserver*
ACTrieController::GetTextObserverPort() noexcept {
    return &text_port_;
}

}  // namespace AppSpace::Controller
