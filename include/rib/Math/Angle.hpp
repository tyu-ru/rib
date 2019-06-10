#pragma once

#include <utility>
#include <type_traits>
#include "Constant.hpp"

/// math
namespace rib::math
{

/// radian -> degree conversion
template <class T>
inline constexpr T rad2deg(const T& x) { return x * 180. / constant::pi<T>; }
/// degree -> radian conversion
template <class T>
inline constexpr T deg2rad(const T& x) { return x * constant::pi<T> / 180.; }

} // namespace rib::math
