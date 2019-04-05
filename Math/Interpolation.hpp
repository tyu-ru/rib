#pragma once

namespace rib::math
{

/// liner interpolate (std::lerp isn't constexpr)
template <class T>
inline constexpr T lerp(const T& x1, const T& x2, float t) { return x1 + t * (x2 - x1); }
/// liner interpolate (std::lerp isn't constexpr)
template <class T>
inline constexpr T lerp(const T& x1, const T& x2, double t) { return x1 + t * (x2 - x1); }
/// liner interpolate (std::lerp isn't constexpr)
template <class T>
inline constexpr T lerp(const T& x1, const T& x2, long double t) { return x1 + t * (x2 - x1); }

} // namespace rib::math
