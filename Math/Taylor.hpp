#pragma once

#include <cstddef>

namespace rib::math
{

/**
 * @brief calculate maclaurin series
 * @details 
 * @param x 
 * @param f f(i) -> (i_th derivative of function) of 0
 * @param n caluclate [0, n]th items
 * @return constexpr T 
 */
template <class T, class F>
inline constexpr T maclaurinSeries(const T& x, F f, std::size_t n)
{
    T sum = 0, xn = 1, fa = 1;
    for (std::size_t i = 0; i <= n; ++i, fa *= i, xn *= x) {
        sum += f(i) * xn / fa;
    }
    return sum;
}

namespace maclaurin
{

template <class T>
inline constexpr T exp(const T& x, std::size_t n)
{
    return maclaurinSeries(x, [](auto) { return 1; }, n);
}
template <class T>
inline constexpr T sin(const T& x, std::size_t n)
{
    return maclaurinSeries(x, [](std::size_t i) { int t[4] = { 0,1,0,-1 }; return t[i%4]; }, n);
}
template <class T>
inline constexpr T cos(const T& x, std::size_t n)
{
    return maclaurinSeries(x, [](std::size_t i) { int t[4] = { 1,0,-1,0 }; return t[i%4]; }, n);
}
template <class T>
inline constexpr T sinh(const T& x, std::size_t n)
{
    return maclaurinSeries(x, [](std::size_t i) { return i % 2; }, n);
}
template <class T>
inline constexpr T cosh(const T& x, std::size_t n)
{
    return maclaurinSeries(x, [](std::size_t i) { return (i + 1) % 2; }, n);
}

} // namespace maclaurin

} // namespace rib::math
