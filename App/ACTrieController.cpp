#include "ACTrieController.hpp"

namespace AppSpace::Controller {

ACTrieController::ACTrieController(ACTrieModel& host_model)
    : model_(&host_model),
      pattern_port_([this](Pattern pattern) { model_->AddPattern(pattern); }),
      text_port_([this](Text text) { model_->FindAllSubstringsInText(text); }),
      trie_reset_port_([this]() { model_->ResetACTrie(); }) {}

ACTrieController::PatternObserver*
ACTrieController::GetPatternObserverPort() noexcept {
    return &pattern_port_;
}

ACTrieController::TextObserver*
ACTrieController::GetTextObserverPort() noexcept {
    return &text_port_;
}

ACTrieController::TrieResetObserver*
ACTrieController::GetTrieResetObserverPort() noexcept {
    return &trie_reset_port_;
}

}  // namespace AppSpace::Controller
