#pragma once

#include <type_traits>
#include <utility>
#include "Traits/FuncTraits.hpp"

namespace rib::operators
{

template <class Derived>
struct LessThanComparable
{
    template <class T, std::enable_if_t<trait::is_lessable_v<std::decay_t<T>, Derived>, std::nullptr_t> = nullptr>
    friend constexpr bool operator>(const Derived& lhs, T&& rhs)
    {
        return std::forward<T>(rhs) < lhs;
    }

    template <class T, std::enable_if_t<trait::is_lessable_v<std::decay_t<T>, Derived>, std::nullptr_t> = nullptr>
    friend constexpr bool operator<=(const Derived& lhs, T&& rhs)
    {
        return !(std::forward<T>(rhs) < lhs);
    }

    template <class T, std::enable_if_t<trait::is_lessable_v<Derived, std::decay_t<T>>, std::nullptr_t> = nullptr>
    friend constexpr bool operator>=(const Derived& lhs, T&& rhs)
    {
        return !(lhs < std::forward<T>(rhs));
    }
};

template <class Derived>
struct EqualityComparable
{
    template <class T, std::enable_if_t<trait::is_equalable_v<Derived, std::decay_t<T>>, std::nullptr_t> = nullptr>
    friend constexpr bool operator!=(const Derived& lhs, T&& rhs)
    {
        return !(lhs == rhs);
    }
};

} // namespace rib::operators