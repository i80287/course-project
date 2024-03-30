#include "StringHistoryManager.hpp"

#include <cctype>
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

std::string_view StringHistoryManager::TrimSpaces(
    std::string_view str) noexcept {
    std::size_t l = 0;
    while (l < str.size() && std::isspace(static_cast<std::uint8_t>(str[l]))) {
        l++;
    }
    std::size_t r = str.size();
    while (r > l && std::isspace(static_cast<std::uint8_t>(str[r - 1]))) {
        r--;
    }
    return {str.begin() + l, r - l};
}

}  // namespace AppSpace::GraphicsUtils::DrawerUtils
