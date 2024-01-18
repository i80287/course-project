#pragma once

#include <imgui.h>

namespace AppSpace {

/// @brief Returns vector in the middle of the first and second vectors
/// @param first
/// @param second
/// @return
constexpr ImVec2 ImVecMiddle(const ImVec2& first,
                             const ImVec2& second) noexcept {
    return ImVec2((first.x + second.x) * 0.5f, (first.y + second.y) * 0.5f);
}

/// @brief Returns vector in the middle of the first and second vectors
/// @param first
/// @param second
/// @return
constexpr ImVec4 ImVecMiddle(const ImVec4& first,
                             const ImVec4& second) noexcept {
    return ImVec4((first.x + second.x) * 0.5f, (first.y + second.y) * 0.5f,
                  (first.z + second.z) * 0.5f, (first.w + second.w) * 0.5f);
}

/// @brief Adds first vector to the second vector
/// @param first
/// @param second
/// @return
constexpr ImVec2 ImVecAdd(const ImVec2& first, const ImVec2& second) noexcept {
    return ImVec2(first.x + second.x, first.y + second.y);
}

/// @brief Adds first vector to the second vector
/// @param first
/// @param second
/// @return
constexpr ImVec4 ImVecAdd(const ImVec4& first, const ImVec4& second) noexcept {
    return ImVec4(first.x + second.x, first.y + second.y, first.z + second.z,
                  first.w + second.w);
}

/// @brief Subtracts second vector from the first one
/// @param first
/// @param second
/// @return
constexpr ImVec2 ImVecDiff(const ImVec2& first, const ImVec2& second) noexcept {
    return ImVec2(first.x - second.x, first.y - second.y);
}

/// @brief Subtracts second vector from the first one
/// @param first
/// @param second
/// @return
constexpr ImVec4 ImVecDiff(const ImVec4& first, const ImVec4& second) noexcept {
    return ImVec4(first.x - second.x, first.y - second.y, first.z - second.z,
                  first.w - second.w);
}

}  // namespace AppSpace
