#pragma once

#include <functional>
#include <type_traits>

#include "TypeTraits.hpp"

namespace rib::trait
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

template <typename T = void>
struct dereference
{
    constexpr T operator()(const T& x) const { return *x; }
};
template <>
struct dereference<void>
{
    template <class T>
    constexpr auto operator()(T&& t) const
        -> decltype(*std::forward<T>(t)) { return *std::forward<T>(t); }
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
DECLARE_UNARY_OPERATOR_ABLE(dereference, dereference)

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

/**
 * @brief type to which the member belong
 */
template <class>
struct belong_type
{
    using type = void;
};
template <class T, class U>
struct belong_type<U T::*>
{
    using type = T;
};
template <class R, class T, class... Args>
struct belong_type<R (T::*)(Args...)>
{
    using type = T;
};
template <class T>
using belong_type_t = typename belong_type<T>::type;

static_assert(std::is_same_v<belong_type_t<int(int)>, void>);
static_assert([] {
    struct A
    {
    };
    return std::is_same_v<belong_type_t<int A::*>, A>;
}());
static_assert([] {
    struct A
    {
    };
    return std::is_same_v<belong_type_t<int (A::*)(int)>, A>;
}());

/**
 * @brief strict invoke result type check
 * @tparam R result type
 * @tparam F invokeable type
 * @tparam Args 
 */
template <class R, class F, class... Args>
struct is_result
    : std::is_same<
          R,
          typename std::conditional_t<
              std::is_member_function_pointer_v<F> || std::is_member_object_pointer_v<F>,
              std::invoke_result<F, belong_type_t<F>, Args...>,
              std::invoke_result<F, Args...>>::type>
{
};
template <class R, class F, class... Args>
constexpr bool is_result_v = is_result<R, F, Args...>::value;

static_assert(is_result_v<int, int(int), int>);
static_assert(!is_result_v<int, long(int), int>);

namespace detail
{
template <class R, class T, class T1, class... Args>
inline constexpr decltype(auto) invoke_constexpr_helper(R T::*f, T1&& t1, Args&&... args)
{
    if constexpr (std::is_member_function_pointer_v<decltype(f)>) {
        if constexpr (std::is_base_of_v<T, std::decay_t<T1>>) {
            return (std::forward<T1>(t1).*f)(std::forward<Args>(args)...);
        } else if constexpr (is_template_specialized_by_type_v<std::reference_wrapper, std::decay_t<T1>>) {
            return (std::forward<T1>(t1.get().*f))(std::forward<Args>(args)...);
        } else {
            return ((*std::forward<T1>(t1)).*f)(std::forward<Args>(args)...);
        }
    } else {
        static_assert(std::is_member_object_pointer_v<decltype(f)>);
        static_assert(sizeof...(args) == 0);
        if constexpr (std::is_base_of_v<T, std::decay_t<T1>>) {
            return std::forward<T1>(t1).*f;
        } else if constexpr (is_template_specialized_by_type_v<std::reference_wrapper, std::decay_t<T1>>) {
            return t1.get().*f;
        } else {
            return (*std::forward<T1>(t1)).*f;
        }
    }
}
template <class F, class... Args>
inline constexpr decltype(auto) invoke_constexpr_helper(F&& f, Args&&... args)
{
    return std::forward<F>(f)(std::forward<Args>(args)...);
}

} // namespace detail

template <class F, class... Args>
inline constexpr std::invoke_result_t<F, Args...> invoke_constexpr(F&& f, Args&&... args) noexcept(std::is_nothrow_invocable_v<F, Args...>)
{
    return detail::invoke_constexpr_helper(std::forward<F>(f), std::forward<Args>(args)...);
}

static_assert(invoke_constexpr([](auto x) { return x + 1; }, 1) == 2);
static_assert([] {
    struct A
    {
        int x;
        constexpr int f(int y) { return x + y; }
    };
    A a{1};
    return invoke_constexpr(&A::x, a) == 1 &&
           invoke_constexpr(&A::f, a, 1) == 2;
}());

} // namespace rib::trait
