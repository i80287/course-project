#include "StringHistoryManager.hpp"

#include <cstdint>

namespace AppSpace::GraphicsUtils::DrawerUtils {

std::string_view StringHistoryManager::Last() const noexcept {
    assert(!Empty());
    return history_.back();
}

void StringHistoryManager::PopLast() noexcept {
    assert(!Empty());
    history_.pop_back();
}

std::string StringHistoryManager::PopAndGetLast() noexcept {
    assert(!Empty());
    std::string last_elem(std::move(history_.back()));
    history_.pop_back();
    return last_elem;
}

void StringHistoryManager::Clear() noexcept {
    history_.clear();
}

std::string_view StringHistoryManager::MoveToPreviousInputAndRead() noexcept {
    assert(!Empty());
    current_history_position_--;
    if (IndexToSignedIndex(current_history_position_) < 0) {
        current_history_position_ = history_.size() - 1;
    }
    return history_[current_history_position_];
}

std::string_view StringHistoryManager::MoveToNextInputAndRead() noexcept {
    assert(!Empty());
    current_history_position_++;
    if (current_history_position_ >= history_.size()) {
        current_history_position_ = 0;
    }
    return history_[current_history_position_];
}

}  // namespace AppSpace::GraphicsUtils::DrawerUtils
