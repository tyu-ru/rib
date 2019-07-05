#pragma once

#include "../Traits/TypeTraits.hpp"

namespace rib
{

/**
 * @brief Iterator for data that may have a value
 * @tparam T the type of the values that can be obtained by dereferencing the iterator.
 * @tparam Tag tag type
 */
template <class T, class Tag>
class MaybeIterator
{
    T* ptr = nullptr;

public:
    using difference_type = std::size_t;
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using iterator_category = std::forward_iterator_tag;

public:
    /**
     * @brief ctor
     * @param p object ptr. if object is none; p = nullptr
     */
    constexpr MaybeIterator(T* p = nullptr) : ptr(p) {}
    /// copy ctor
    constexpr MaybeIterator(const MaybeIterator&) = default;
    /// move ctor
    constexpr MaybeIterator(MaybeIterator&&) = default;

    /// dereference value (const)
    constexpr const T& operator*() const { return *ptr; }
    /// dereference value (not const. if T is const then disable)
    template <class U = T, trait::concept_t<std::is_same_v<T, U> && !std::is_const_v<T>> = nullptr>
    constexpr T& operator*() { return *ptr; }

    /**
     * @brief increment(prefix)
     * @details iterator will be null
     * @return *this
     */
    constexpr MaybeIterator& operator++()
    {
        ptr = nullptr;
        return *this;
    }
    /**
     * @brief increment(suffix)
     * @details iterator will be null
     * @return *this copy
     */
    constexpr MaybeIterator operator++(int)
    {
#if __cplusplus < 202012L
        auto t = *this;
        ptr = nullptr;
        return t;
#else
        return std::exchange(ptr, nullptr);
#endif
    }

    /// equally compare
    constexpr friend bool operator==(const MaybeIterator& lhs, const MaybeIterator& rhs) { return lhs.ptr == rhs.ptr; }
    /// unequally compare
    constexpr friend bool operator!=(const MaybeIterator& lhs, const MaybeIterator& rhs) { return lhs.ptr != rhs.ptr; }
};

template <class T, class Tag>
using MaybeConstIterator = MaybeIterator<const T, Tag>;

} // namespace rib
