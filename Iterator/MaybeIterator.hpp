#pragma once

#include "../Traits/TypeTraits.hpp"

namespace rib
{

template <class Derived, class T>
struct MayBeIteratible;

template <class T, class Tag>
class MaybeIterator
{
    friend MayBeIteratible<Tag, T>;

public:
    using difference_type = std::size_t;
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using iterator_category = std::forward_iterator_tag;

public:
    constexpr MaybeIterator() = default;
    constexpr MaybeIterator(const MaybeIterator&) = default;
    constexpr MaybeIterator(MaybeIterator&&) = default;

    constexpr const T& operator*() const { return *ptr; }
    constexpr T& operator*() { return *ptr; }

    constexpr MaybeIterator& operator++()
    {
        ptr = nullptr;
        return *this;
    }
    constexpr MaybeIterator operator++(int)
    {
        auto t = *this;
        ptr = nullptr;
        return t;
    }

    constexpr friend bool operator==(const MaybeIterator& lhs, const MaybeIterator& rhs)
    {
        return lhs.ptr == rhs.ptr;
    }
    constexpr friend bool operator!=(const MaybeIterator& lhs, const MaybeIterator& rhs)
    {
        return lhs.ptr != rhs.ptr;
    }

private:
    constexpr MaybeIterator(T* p) : ptr(p) {}
    T* ptr = nullptr;
};

template <class T, class Tag>
using MaybeConstIterator = MaybeIterator<const T, Tag>;

template <class Derived, class T>
struct MayBeIteratible
{
    constexpr MaybeConstIterator<T, Derived> begin() const
    {
        auto& d = static_cast<const Derived&>(*this);
        if (d) return &*d;
        return nullptr;
    }
    constexpr MaybeIterator<T, Derived> begin()
    {
        auto& d = static_cast<Derived&>(*this);
        if (d) return &*d;
        return nullptr;
    }

    constexpr MaybeConstIterator<T, Derived> end() const { return nullptr; }
    constexpr MaybeIterator<T, Derived> end() { return nullptr; }
};

} // namespace rib
