#pragma once

#include <iterator>

namespace rib::math
{

/// residual sum of squares
template <class InputIterator1, class InputIterator2>
inline constexpr auto rss(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2)
    -> typename std::iterator_traits<InputIterator1>::value_type
{
    typename std::iterator_traits<InputIterator1>::value_type sum = 0, t = 0;
    while (first1 != last1) {
        t = *first1 - *first2;
        sum += t * t;
        ++first1;
        ++first2;
    }
    return sum;
}

} // namespace rib::math
