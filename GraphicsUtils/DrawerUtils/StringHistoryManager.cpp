#include "StringHistoryManager.hpp"

#include <cstdint>

namespace AppSpace::GraphicsUtils::DrawerUtils {

std::string_view StringHistoryManager::MoveToPreviousInputAndRead() noexcept {
    assert(!Empty());
    current_history_position_--;
    if (static_cast<std::ptrdiff_t>(current_history_position_) < 0) {
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
