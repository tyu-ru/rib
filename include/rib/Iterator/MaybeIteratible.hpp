#pragma once

#include "MaybeIterator.hpp"

namespace rib
{

/**
 * @brief Iterator for data that may have a const value
 * @details 
 * @tparam Derived 
 * @tparam T 
 */
template <class Derived, class T>
struct MaybeConstIteratible
{
    constexpr MaybeConstIterator<T, Derived> begin() const
    {
        auto& d = static_cast<const Derived&>(*this);
        if (d) return &*d;
        return nullptr;
    }
    constexpr MaybeConstIterator<T, Derived> end() const { return nullptr; }
};

/**
 * @brief 
 * @details 
 * @tparam Derived 
 * @tparam T 
 */
template <class Derived, class T>
struct MaybeIteratible : public MaybeConstIteratible<Derived, T>
{
    /**
     * @brief 
     * 
     * @return constexpr MaybeIterator<T, Derived> 
     */
    constexpr MaybeIterator<T, Derived> begin()
    {
        auto& d = static_cast<Derived&>(*this);
        if (d) return &*d;
        return nullptr;
    }

    constexpr MaybeIterator<T, Derived> end() { return nullptr; }
};

} // namespace rib
