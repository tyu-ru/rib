#pragma once

#include <cmath>
#include "Quantity.hpp"

namespace rib::units
{

template <class T>
inline constexpr auto sin(const Quantity<dim::Dimension<>, T>& val)
    -> decltype(std::sin(T{}))
{
    return std::sin(val.value());
}

template <class T>
inline constexpr auto cos(const Quantity<dim::Dimension<>, T>& val)
    -> decltype(std::cos(T{}))
{
    return std::cos(val.value());
}

template <class T>
inline constexpr auto tan(const Quantity<dim::Dimension<>, T>& val)
    -> decltype(std::tan(T{}))
{
    return std::tan(val.value());
}

} // namespace rib::units