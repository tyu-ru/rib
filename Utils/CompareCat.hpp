#pragma once

#include <utility>
#include "../Traits/TypeTraits.hpp"
#include "../Mixin/CopyMoveTrait.hpp"

namespace rib
{

template <class T>
class CompareCat : mixin::CopyMoveTrait<false, false, false, false, CompareCat<T>>
{
    T&& bind;
    bool pre = true;

    constexpr CompareCat(T&& val, bool p) : bind(std::forward<T&&>(val)), pre(p) {}

public:
    constexpr CompareCat(T&& val) : bind(std::forward<T&&>(val)) {}

    constexpr inline operator bool() const { return pre; }
    constexpr inline bool operator!() const { return !pre; }

#define DECLARE_OPERATOR(op)                                    \
    template <class U>                                          \
    constexpr inline CompareCat<U&&> operator op(U&& rhs) const \
    {                                                           \
        if (!pre) return {std::forward<U>(rhs), false};         \
        bool f = std::forward<T&&>(bind) op rhs;                \
        return {std::forward<U>(rhs), f};                       \
    }
    DECLARE_OPERATOR(<)
    DECLARE_OPERATOR(<=)
    DECLARE_OPERATOR(>)
    DECLARE_OPERATOR(>=)
    DECLARE_OPERATOR(==)
    DECLARE_OPERATOR(!=)
#undef DECLARE_OPERATOR
};
template <class T>
constexpr inline CompareCat<T&&> compCat(T&& x) { return {std::forward<T>(x)}; }

static_assert(std::is_same_v<decltype(compCat(1)),
                             CompareCat<int&&>>);
static_assert(std::is_same_v<decltype(compCat(std::declval<int&>())),
                             CompareCat<int&>>);
static_assert(std::is_same_v<decltype(compCat(std::declval<const int&>())),
                             CompareCat<const int&>>);

static_assert(std::is_same_v<decltype(compCat(1) < 1),
                             CompareCat<int&&>>);
static_assert(std::is_same_v<decltype(compCat(1) < std::declval<int&>()),
                             CompareCat<int&>>);
static_assert(std::is_same_v<decltype(compCat(1) < std::declval<const int&>()),
                             CompareCat<const int&>>);

static_assert(compCat(1) < 2);
static_assert(compCat(-1) < 0);
static_assert(!(compCat(1) < 0));

static_assert(compCat(1) < 2 < 3);
static_assert(compCat(-1) < 0 < 1);
static_assert(!(compCat(1) < 2 < 1));

static_assert(compCat(1) < 2 <= 2 > 1 >= 1 == 1 != 0);
static_assert(!(compCat(1) < 2 <= 2 > 1 >= 1 == 1 != 1));

} // namespace rib
