#pragma once

#include <imgui.h>

namespace AppSpace::ImGuiExtensions {

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
constexpr ImVec2 operator+(const ImVec2& first, const ImVec2& second) noexcept {
    return ImVec2(first.x + second.x, first.y + second.y);
}

/// @brief Adds first vector to the second vector
/// @param first
/// @param second
/// @return
constexpr ImVec4 operator+(const ImVec4& first, const ImVec4& second) noexcept {
    return ImVec4(first.x + second.x, first.y + second.y, first.z + second.z,
                  first.w + second.w);
}

/// @brief Subtracts second vector from the first one
/// @param first
/// @param second
/// @return
constexpr ImVec2 operator-(const ImVec2& first, const ImVec2& second) noexcept {
    return ImVec2(first.x - second.x, first.y - second.y);
}

/// @brief Subtracts second vector from the first one
/// @param first
/// @param second
/// @return
constexpr ImVec4 operator-(const ImVec4& first, const ImVec4& second) noexcept {
    return ImVec4(first.x - second.x, first.y - second.y, first.z - second.z,
                  first.w - second.w);
}

/// @brief Multiplies components of the @a `vec` by @a `num`
/// @tparam Scalar arithmetic type
/// @param vec
/// @param num
/// @return vec * num
template <class Scalar>
    requires std::is_arithmetic_v<Scalar>
constexpr ImVec2 operator*(const ImVec2& vec, Scalar num) noexcept {
    return ImVec2(vec.x * num, vec.y * num);
}

/// @brief Multiplies components of the @a `vec` by @a `num`
/// @tparam Scalar arithmetic type
/// @param vec
/// @param num
/// @return vec * num
template <class Scalar>
    requires std::is_arithmetic_v<Scalar>
constexpr ImVec4 operator*(const ImVec4& vec, Scalar num) noexcept {
    return ImVec4(vec.x * num, vec.y * num, vec.z * num, vec.w * num);
}

/// @brief Divides components of the @a `vec` by @a `num`
/// @tparam Scalar arithmetic type
/// @param vec
/// @param num
/// @return vec / num
template <class Scalar>
    requires std::is_arithmetic_v<Scalar>
constexpr ImVec2 operator/(const ImVec2& vec, Scalar num) noexcept {
    return ImVec2(vec.x / num, vec.y / num);
}

/// @brief Divides components of the @a `vec` by @a `num`
/// @tparam Scalar arithmetic type
/// @param vec
/// @param num
/// @return vec / num
template <class Scalar>
    requires std::is_arithmetic_v<Scalar>
constexpr ImVec4 operator/(const ImVec4& vec, Scalar num) noexcept {
    return ImVec4(vec.x / num, vec.y / num, vec.z / num, vec.w / num);
}

}  // namespace AppSpace::ImGuiExtensions
