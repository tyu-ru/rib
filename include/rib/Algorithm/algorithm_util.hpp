#pragma once

#include <algorithm>
#include "../Traits/TypeTraits.hpp"
#include "../Traits/FuncTraits.hpp"

namespace rib::algo
{

template <class T, trait::concept_t<std::is_member_pointer_v<T>> = nullptr>
constexpr auto gen_member_less(T mem_ptr)
{
    return [=](auto&& lhs, auto&& rhs) {
        return trait::invoke_constexpr(mem_ptr, std::forward<decltype(lhs)>(lhs)) <
               trait::invoke_constexpr(mem_ptr, std::forward<decltype(rhs)>(rhs));
    };
}

/// xxx_if algorithm

#define DECLARE_XXX_IF_ALGORITHM(func_name)                                                                                  \
    template <class InputIterator, class F, class T>                                                                         \
    inline constexpr auto func_name(InputIterator first, InputIterator last, F fn, T&& value)                                \
    {                                                                                                                        \
        return std::func_name(first, last,                                                                                   \
                              [&](auto&& v) { return trait::invoke_constexpr(fn, std::forward<decltype(v)>(v)) == value; }); \
    }                                                                                                                        \
    template <class Container, class F, class T>                                                                             \
    inline constexpr auto func_name(Container&& c, F fn, T&& value)                                                          \
    {                                                                                                                        \
        using std::begin, std::end;                                                                                          \
        return func_name(begin(c), end(c), fn, std::forward<T>(value));                                                      \
    }

DECLARE_XXX_IF_ALGORITHM(all_of)
DECLARE_XXX_IF_ALGORITHM(any_of)
DECLARE_XXX_IF_ALGORITHM(none_of)
DECLARE_XXX_IF_ALGORITHM(find_if)
DECLARE_XXX_IF_ALGORITHM(find_if_not)
DECLARE_XXX_IF_ALGORITHM(count_if)

#undef DECLARE_XXX_IF_ALGORITHM

/// container wrapper

template <class Container, class T>
inline constexpr auto find(Container&& c, T&& value)
{
    using std::begin, std::end;
    return std::find(begin(c), end(c), value);
}

} // namespace rib::algo
