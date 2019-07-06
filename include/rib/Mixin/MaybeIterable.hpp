#pragma once

#include "../Iterator/MaybeIterator.hpp"

namespace rib::mixin
{

/**
 * @brief Iterator for data that may have a const value
 * @details
 * @tparam Derived CRTP
 * @tparam T value type
 */
template <class Derived, class T>
struct MaybeConstIterable
{
    /**
     * @brief begin iterator to beginning
     * @details returns iterator to value or null
     * @return constexpr MaybeConstIterator<T, Derived>
     */
    constexpr MaybeConstIterator<T, Derived> begin() const
    {
        auto& d = static_cast<const Derived&>(*this);
        if (d) return &*d;
        return nullptr;
    }

    /**
     * @brief end iterator to the end
     * @details returns iterator to null
     * @return constexpr MaybeConstIterator<T, Derived>
     */
    constexpr MaybeConstIterator<T, Derived> end() const { return nullptr; }
};

/**
 * @brief iterator for data that may have a value
 * @details
 * @tparam Derived CRTP
 * @tparam T value type
 */
template <class Derived, class T>
struct MaybeIterable : public MaybeConstIterable<Derived, T>
{
    /**
     * @brief begin iterator to beginning
     * @details returns iterator to value or null
     * @return constexpr MaybeIterator<T, Derived>
     */
    constexpr MaybeIterator<T, Derived> begin()
    {
        auto& d = static_cast<Derived&>(*this);
        if (d) return &*d;
        return nullptr;
    }

    /**
     * @brief end iterator to the end
     * @details returns iterator to null
     * @return constexpr MaybeIterator<T, Derived>
     */
    constexpr MaybeIterator<T, Derived> end() { return nullptr; }
};

} // namespace rib::mixin
