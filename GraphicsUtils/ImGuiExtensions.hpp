#pragma once

#include <imgui.h>

namespace AppSpace::ImGuiExtensions {

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

/// @brief Returns vector in the middle of the first and second vectors
/// @param first
/// @param second
/// @return
constexpr ImVec2 ImVecMiddle(const ImVec2& first,
                             const ImVec2& second) noexcept {
    return (first + second) * 0.5f;
}

/// @brief Returns vector in the middle of the first and second vectors
/// @param first
/// @param second
/// @return
constexpr ImVec4 ImVecMiddle(const ImVec4& first,
                             const ImVec4& second) noexcept {
    return (first + second) * 0.5f;
}

template <class Scalar>
    requires std::is_arithmetic_v<Scalar>
constexpr ImVec2 operator+(const ImVec2& vec, Scalar num) noexcept {
    return vec + ImVec2(num, num);
}

template <class Scalar>
    requires std::is_arithmetic_v<Scalar>
constexpr ImVec4 operator+(const ImVec4& vec, Scalar num) noexcept {
    return vec + ImVec4(num, num, num, num);
}

template <class Scalar>
    requires std::is_arithmetic_v<Scalar>
constexpr ImVec2 operator-(const ImVec2& vec, Scalar num) noexcept {
    return vec - ImVec2(num, num);
}

template <class Scalar>
    requires std::is_arithmetic_v<Scalar>
constexpr ImVec4 operator-(const ImVec4& vec, Scalar num) noexcept {
    return vec - ImVec4(num, num, num, num);
}

constexpr bool operator==(const ImVec2& lhs, const ImVec2& rhs) noexcept {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

constexpr bool operator!=(const ImVec2& lhs, const ImVec2& rhs) noexcept {
    return !(lhs == rhs);
}

constexpr bool operator==(const ImVec4& lhs, const ImVec4& rhs) noexcept {
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w;
}

constexpr bool operator!=(const ImVec4& lhs, const ImVec4& rhs) noexcept {
    return !(lhs == rhs);
}

}  // namespace AppSpace::ImGuiExtensions
