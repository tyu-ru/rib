#pragma once

#include <iterator>
#include "../Traits/FuncTraits.hpp"

namespace rib
{

template <class Iterator, auto f>
class ReinterpretIterator
{
    Iterator itr;

public:
    using difference_type = typename std::iterator_traits<Iterator>::difference_type;
    using value_type = typename std::iterator_traits<Iterator>::value_type;
    using pointer = typename std::iterator_traits<Iterator>::pointer;
    using reference = typename std::iterator_traits<Iterator>::reference;
    using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;

public:
    constexpr ReinterpretIterator(const Iterator& itr_) : itr(itr_) {}

    constexpr bool operator==(const ReinterpretIterator& rhs) const { return itr == rhs.itr; }
    constexpr bool operator!=(const ReinterpretIterator& rhs) const { return itr != rhs.itr; }
    constexpr bool operator==(const Iterator& rhs) const { return itr == rhs; }
    constexpr bool operator!=(const Iterator& rhs) const { return itr != rhs; }

    constexpr ReinterpretIterator& operator+=(const difference_type& n)
    {
        std::advance(itr, n);
        return *this;
    }
    constexpr ReinterpretIterator& operator-=(const difference_type& n)
    {
        std::advance(itr, -n);
        return *this;
    }

    constexpr ReinterpretIterator operator+(const difference_type& n) const { return std::next(itr, n); }
    constexpr ReinterpretIterator operator-(const difference_type& n) const { return std::prev(itr, -n); }

    constexpr difference_type operator-(const ReinterpretIterator& rhs) const { return itr - rhs.itr; }

    constexpr ReinterpretIterator& operator++() { return ++itr, *this; }
    constexpr ReinterpretIterator& operator--() { return --itr, *this; }
    constexpr ReinterpretIterator operator++(int)
    {
        auto it = itr;
        ++itr;
        return {it};
    }
    constexpr ReinterpretIterator operator--(int)
    {
        auto it = itr;
        --itr;
        return {it};
    }

    constexpr decltype(auto) operator*() { return trait::invoke_constexpr(f, itr); }

    constexpr Iterator iterator() const { return itr; }
};

template <auto f, class Iterator>
ReinterpretIterator<std::decay_t<Iterator>, f> makeReinterpretIterator(Iterator itr)
{
    return {itr};
}

} // namespace rib
