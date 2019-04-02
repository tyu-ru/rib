#pragma once

#include <functional>
#include <type_traits>

namespace rib::func
{

#define DECLARE_COMPOUND_OPERATOR_FUNCTOR(name, op)                                                \
    template <class T = void>                                                                      \
    struct name                                                                                    \
    {                                                                                              \
        constexpr T& operator()(T& lhs, const T& rhs) const { return lhs op rhs; }                 \
    };                                                                                             \
    template <>                                                                                    \
    struct name<void>                                                                              \
    {                                                                                              \
        template <class T, class U>                                                                \
        constexpr auto operator()(T&& lhs, U&& rhs) const -> decltype(lhs op std::forward<U>(rhs)) \
        {                                                                                          \
            return lhs op std::forward<U>(rhs);                                                    \
        }                                                                                          \
    };

DECLARE_COMPOUND_OPERATOR_FUNCTOR(compound_plus, +=)
DECLARE_COMPOUND_OPERATOR_FUNCTOR(compound_minus, -=)
DECLARE_COMPOUND_OPERATOR_FUNCTOR(compound_multiplies, *=)
DECLARE_COMPOUND_OPERATOR_FUNCTOR(compound_divides, /=)
DECLARE_COMPOUND_OPERATOR_FUNCTOR(compound_modulus, %=)

#undef DECLARE_COMPOUND_OPERATOR_FUNCTOR

#define DECLARE_OPERATOR_ABLE_V(name, functor)                  \
    template <class T, class U>                                 \
    struct is_##name##able : std::is_invocable<functor<>, T, U> \
    {                                                           \
    };                                                          \
    template <class T, class U>                                 \
    constexpr bool is_##name##able_v = is_##name##able<T, U>::value;

#define DECLARE_OPERATOR_ABLE_R_V(name, functor)                       \
    template <class R, class T, class U>                               \
    struct is_##name##able_r : std::is_invocable_r<R, functor<>, T, U> \
    {                                                                  \
    };                                                                 \
    template <class R, class T, class U>                               \
    constexpr bool is_##name##able_r_v = is_##name##able_r<R, T, U>::value;

DECLARE_OPERATOR_ABLE_R_V(plus, std::plus)
DECLARE_OPERATOR_ABLE_R_V(minus, std::minus)
DECLARE_OPERATOR_ABLE_R_V(multiplies, std::multiplies)
DECLARE_OPERATOR_ABLE_R_V(divides, std::divides)
DECLARE_OPERATOR_ABLE_R_V(modulus, std::modulus)

DECLARE_OPERATOR_ABLE_R_V(compound_plus, compound_plus)

DECLARE_OPERATOR_ABLE_V(equal_to, std::equal_to)
DECLARE_OPERATOR_ABLE_V(not_equal_to, std::not_equal_to)
DECLARE_OPERATOR_ABLE_V(less, std::less)
DECLARE_OPERATOR_ABLE_V(less_equal, std::less_equal)
DECLARE_OPERATOR_ABLE_V(greater, std::greater)
DECLARE_OPERATOR_ABLE_V(greater_equal, std::greater_equal)

#undef DECLARE_OPERATOR_ABLE_R_V

} // namespace rib::func
