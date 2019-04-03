#pragma once

#include <functional>
#include <type_traits>

namespace rib::func
{

template <typename T = void>
struct unary_plus
{
    constexpr T operator()(const T& x) const { return +x; }
};

template <>
struct unary_plus<void>
{
    template <class T>
    constexpr auto operator()(T&& t) const
        -> decltype(+std::forward<T>(t)) { return *std::forward<T>(t); }
};

#define DECLARE_COMPOUND_OPERATOR_FUNCTOR(name, op)         \
    template <class T = void>                               \
    struct name                                             \
    {                                                       \
        constexpr T& operator()(T& lhs, const T& rhs) const \
        {                                                   \
            return lhs op rhs;                              \
        }                                                   \
    };                                                      \
    template <>                                             \
    struct name<void>                                       \
    {                                                       \
        template <class T, class U>                         \
        constexpr auto operator()(T&& lhs, U&& rhs) const   \
            -> decltype(lhs op std::forward<U>(rhs))        \
        {                                                   \
            return lhs op std::forward<U>(rhs);             \
        }                                                   \
    };

DECLARE_COMPOUND_OPERATOR_FUNCTOR(compound_plus, +=)
DECLARE_COMPOUND_OPERATOR_FUNCTOR(compound_minus, -=)
DECLARE_COMPOUND_OPERATOR_FUNCTOR(compound_multiplies, *=)
DECLARE_COMPOUND_OPERATOR_FUNCTOR(compound_divides, /=)
DECLARE_COMPOUND_OPERATOR_FUNCTOR(compound_modulus, %=)

#undef DECLARE_COMPOUND_OPERATOR_FUNCTOR

#define DECLARE_UNARY_OPERATOR_ABLE(name, functor)                  \
    template <class T>                                              \
    struct is_##name##able : std::is_invocable<functor<>, T>        \
    {                                                               \
    };                                                              \
    template <class T>                                              \
    constexpr bool is_##name##able_v = is_##name##able<T>::value;   \
    template <class R, class T>                                     \
    struct is_##name##able_r : std::is_invocable_r<R, functor<>, T> \
    {                                                               \
    };                                                              \
    template <class R, class T>                                     \
    constexpr bool is_##name##able_r_v = is_##name##able_r<R, T>::value;

DECLARE_UNARY_OPERATOR_ABLE(unary_plus, unary_plus)
DECLARE_UNARY_OPERATOR_ABLE(negate, std::negate)

#undef DECLARE_UNARY_OPERATOR_ABLE

#define DECLARE_BINARY_OPERATOR_ABLE(name, functor)                    \
    template <class T, class U>                                        \
    struct is_##name##able : std::is_invocable<functor<>, T, U>        \
    {                                                                  \
    };                                                                 \
    template <class T, class U>                                        \
    constexpr bool is_##name##able_v = is_##name##able<T, U>::value;   \
    template <class R, class T, class U>                               \
    struct is_##name##able_r : std::is_invocable_r<R, functor<>, T, U> \
    {                                                                  \
    };                                                                 \
    template <class R, class T, class U>                               \
    constexpr bool is_##name##able_r_v = is_##name##able_r<R, T, U>::value;

DECLARE_BINARY_OPERATOR_ABLE(plus, std::plus)
DECLARE_BINARY_OPERATOR_ABLE(minus, std::minus)
DECLARE_BINARY_OPERATOR_ABLE(multiplies, std::multiplies)
DECLARE_BINARY_OPERATOR_ABLE(divides, std::divides)
DECLARE_BINARY_OPERATOR_ABLE(modulus, std::modulus)

DECLARE_BINARY_OPERATOR_ABLE(compound_plus, compound_plus)
DECLARE_BINARY_OPERATOR_ABLE(compound_minus, compound_minus)
DECLARE_BINARY_OPERATOR_ABLE(compound_multiplies, compound_multiplies)
DECLARE_BINARY_OPERATOR_ABLE(compound_divides, compound_divides)
DECLARE_BINARY_OPERATOR_ABLE(compound_modulus, compound_modulus)

DECLARE_BINARY_OPERATOR_ABLE(equal, std::equal_to)
DECLARE_BINARY_OPERATOR_ABLE(not_equal, std::not_equal_to)
DECLARE_BINARY_OPERATOR_ABLE(less, std::less)
DECLARE_BINARY_OPERATOR_ABLE(less_equal, std::less_equal)
DECLARE_BINARY_OPERATOR_ABLE(greater, std::greater)
DECLARE_BINARY_OPERATOR_ABLE(greater_equal, std::greater_equal)

#undef DECLARE_BINARY_OPERATOR_ABLE

} // namespace rib::func
