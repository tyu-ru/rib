#pragma once

#include <algorithm>
#include <iterator>
#include "../Algorithm/SortingNetWork.hpp"
#include "../Math/Interpolation.hpp"

namespace rib
{

template <class T>
inline constexpr T evenMedian(const T& x, const T& y)
{
    T t = y - x;
    return x + t / 2;
}

/**
 * @brief calculate median in O(N)
 * if the number of elements is fixed, consider medhian<n>()
 * @param first the range of elements to examine
 * @param last the range of elements to examine
 * @return std::iterator_traits<InputIterator>::value_type
 */
template <class InputIterator>
inline constexpr auto median(InputIterator first, InputIterator last)
    -> typename std::iterator_traits<InputIterator>::value_type
{
    auto d = std::distance(fist, last);
    auto m1 = std::next(first, d / 2);
    std::nth_element(first, m1, last);
    if (d % 2) return *m1; // odd
    return evenMedian(*m1, *std::min_element(std::next(m1), last));
}

/**
 * @brief calculate median in O(NlogN)
 * @tparam n number of elements to examine
 * @param first the range of elements to examine
 * @return std::iterator_traits<RandomAccessIterator>::value_type
 */
template <std::size_t n, class RandomAccessIterator>
inline constexpr auto median(RandomAccessIterator first)
    -> typename std::iterator_traits<RandomAccessIterator>::value_type
{
    srtnw::SortingNetwork<srtnw::BoseNelson<n>>::sort(first);
    if constexpr (n % 2) {
        return *first[n / 2]; //odd
    } else {
        return evenMedian(first[n / 2], first[n / 2 + 1]);
    }
}

} // namespace rib
