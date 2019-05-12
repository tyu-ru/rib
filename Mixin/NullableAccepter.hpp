#pragma once

#include <type_traits>
#include "../Traits/TypeTraits.hpp"
#include "../Traits/FuncTraits.hpp"

namespace rib::mixin
{

template <class Derived>
class NullableAccepter
{
public:
    template <class F, class... Args>
    constexpr const Derived& visit(F&& f, Args&&... args) const&
    {
        const Derived& d = static_cast<Derived&>(*this);
        if (static_cast<bool>(d)) {
            trait::invoke_constexpr(std::forward<F>(f), *d, std::forward<Args>(args)...);
        }
        return d;
    }
    template <class F, class... Args>
    constexpr Derived& visit(F&& f, Args&&... args) &
    {
        Derived& d = static_cast<Derived&>(*this);
        if (static_cast<bool>(d)) {
            trait::invoke_constexpr(std::forward<F>(f), *d, std::forward<Args>(args)...);
        }
        return d;
    }
    template <class F, class... Args>
    constexpr Derived visit(F&& f, Args&&... args) &&
    {
        Derived& d = static_cast<Derived&>(*this);
        if (static_cast<bool>(d)) {
            trait::invoke_constexpr(std::forward<F>(f), *d, std::forward<Args>(args)...);
        }
        return std::move(d);
    }
};

} // namespace rib::mixin
