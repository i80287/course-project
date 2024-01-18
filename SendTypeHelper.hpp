#pragma once

#include <type_traits>

namespace AppSpace {

template <class T>
using SendTypeHelper =
    std::conditional_t<std::is_scalar_v<T>, T,
                       std::add_lvalue_reference_t<std::add_const_t<T>>>;

}  // namespace AppSpace
