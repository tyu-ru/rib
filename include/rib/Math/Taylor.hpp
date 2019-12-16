#pragma once

#include <cstddef>

namespace rib::math
{

/**
 * @brief calculate maclaurin series
 * @details \f[\sum_{i=0}^n \frac{f(i)}{n!}x^i\f]
 * @param x
 * @param f f(i) -> \f$func^{(n)}(0)\f$
 * @param n degree. caluclate [0, n]_th items
 * @return constexpr T
 */
template <class T, class F>
inline constexpr T maclaurinSeries(const T& x, F&& f, std::size_t n)
{
    T sum = 0, xn = 1, fa = 1;
    for (std::size_t i = 0; i <= n; ++i, fa *= i, xn *= x) {
        sum += f(i) * xn / fa;
    }
    return sum;
}

/// typical function maclaurin series
namespace maclaurin
{

/// e^x
template <class T>
inline constexpr T exp(const T& x, std::size_t n)
{
    return maclaurinSeries(x, [](auto) { return 1; }, n);
}
/// sinx
template <class T>
inline constexpr T sin(const T& x, std::size_t n)
{
    return maclaurinSeries(x, [](std::size_t i) { int t[4] = { 0,1,0,-1 }; return t[i%4]; }, n);
}
/// cosx
template <class T>
inline constexpr T cos(const T& x, std::size_t n)
{
    return maclaurinSeries(x, [](std::size_t i) { int t[4] = { 1,0,-1,0 }; return t[i%4]; }, n);
}
/// sinhx
template <class T>
inline constexpr T sinh(const T& x, std::size_t n)
{
    return maclaurinSeries(x, [](std::size_t i) { return i % 2; }, n);
}
/// coshx
template <class T>
inline constexpr T cosh(const T& x, std::size_t n)
{
    return maclaurinSeries(x, [](std::size_t i) { return (i + 1) % 2; }, n);
}

} // namespace maclaurin

} // namespace rib::math
