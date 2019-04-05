#pragma once

#include <cstddef>

namespace rib::math
{

/**
 * @brief calculate maclaurin series
 * @details 
 * @param x 
 * @param f f(i) -> (i_th derivative of function) of 0
 * @param n caluclate [0, n)th items
 * @return constexpr T 
 */
template <class T, class F>
inline constexpr T maclaurinSeries(const T& x, F f, std::size_t n)
{
    T sum = 0, xn = 1, fa = 1;
    for (std::size_t i = 0; i < n; ++i, fa *= i, xn *= x) {
        sum += f(i) * xn / fa;
    }
    return sum;
}

} // namespace rib::math
