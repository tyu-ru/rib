#pragma once

#include <type_traits>
#include <utility>
#include "functional_util.hpp"

namespace rib::operators
{

template <class Derived>
struct LessThanComparable
{
    template <class T, std::enable_if_t<func::is_lessable_v<std::decay_t<T>, Derived>, std::nullptr_t> = nullptr>
    friend constexpr bool operator>(const Derived& lhs, T&& rhs)
    {
        return std::forward<T>(rhs) < lhs;
    }

    template <class T, std::enable_if_t<func::is_lessable_v<std::decay_t<T>, Derived>, std::nullptr_t> = nullptr>
    friend constexpr bool operator<=(const Derived& lhs, T&& rhs)
    {
        return !(std::forward<T>(rhs) < lhs);
    }

    template <class T, std::enable_if_t<func::is_lessable_v<Derived, std::decay_t<T>>, std::nullptr_t> = nullptr>
    friend constexpr bool operator>=(const Derived& lhs, T&& rhs)
    {
        return !(lhs < std::forward<T>(rhs));
    }
};

} // namespace rib::operators