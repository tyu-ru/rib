#pragma once

#include <tuple>
#include <optional>
#include "TypeUtil.hpp"

namespace rib::type::vector
{

template <class... Args>
struct TypeVector
{
    using std_tuple = std::tuple<Args...>;

    static constexpr auto size = sizeof...(Args);
    template <std::size_t i>
    using Element = std::tuple_element_t<i, std_tuple>;

    template <class Predicate>
    static constexpr std::size_t find_if([[maybe_unused]] Predicate pred)
    {
        if constexpr (size == 0) {
            return 0;
        } else {
            std::size_t result{};
            for (bool b : {pred(handle::TypeAdapter<Args>{})...}) {
                if (b) return result;
                ++result;
            }
            return result;
        }
    }
    template <template <class> class Pred>
    static constexpr std::size_t find_if()
    {
        return find_if([](auto x) { return Pred<typename decltype(x)::type>::value; });
    }

    template <class T>
    static constexpr std::size_t find()
    {
        return find_if([](auto x) { return std::is_same_v<T, typename decltype(x)::type>; });
    }

    template <class Predicate>
    static constexpr std::size_t count_if([[maybe_unused]] Predicate pred)
    {
        return (0 + ... + (pred(handle::TypeAdapter<Args>{}) ? 1 : 0));
    }

    template <class T>
    static constexpr std::size_t count()
    {
        return count_if([](auto x) { return std::is_same_v<T, typename decltype(x)::type>; });
    }

    template <class T>
    static constexpr bool contain() { return count<T>() != 0; }

    static constexpr bool unique() { return (... && (count<Args>() == 1)); }
};

static_assert(sizeof(TypeVector<int, char, double>) == 1);

static_assert(TypeVector<>::size == 0);
static_assert(TypeVector<int, char>::size == 2);

static_assert(std::is_same_v<TypeVector<int, char>::Element<0>, int>);
static_assert(std::is_same_v<TypeVector<int, char>::Element<1>, char>);

static_assert(TypeVector<>::find_if<std::is_unsigned>() == 0);
static_assert(TypeVector<int, unsigned int>::find_if<std::is_unsigned>() == 1);

static_assert(TypeVector<>::find<int>() == 0);
static_assert(TypeVector<int>::find<int>() == 0);
static_assert(TypeVector<int, char>::find<char>() == 1);
static_assert(TypeVector<int, char>::find<double>() == 2);

static_assert(TypeVector<>::count<int>() == 0);
static_assert(TypeVector<int>::count<int>() == 1);
static_assert(TypeVector<int, char>::count<char>() == 1);
static_assert(TypeVector<int, int>::count<int>() == 2);
static_assert(TypeVector<int, char>::count<double>() == 0);

static_assert(TypeVector<int>::contain<int>());
static_assert(TypeVector<int, char>::contain<char>());
static_assert(!TypeVector<int, char>::contain<double>());

static_assert(TypeVector<>::unique());
static_assert(TypeVector<int>::unique());
static_assert(TypeVector<int, char>::unique());
static_assert(!TypeVector<int, int>::unique());

template <class... Args>
struct TypeVector_gen
{
    using type = TypeVector<Args...>;
};
template <class... Args>
struct TypeVector_gen<std::tuple<Args...>>
{
    using type = TypeVector<Args...>;
};
template <class... Args>
using TypeVector_gen_t = typename TypeVector_gen<Args...>::type;

static_assert(std::is_same_v<TypeVector_gen_t<int, char>, TypeVector<int, char>>);
static_assert(std::is_same_v<TypeVector_gen_t<std::tuple<int, char>>, TypeVector<int, char>>);

template <class...>
struct TypeVector_cat;
template <class... Args1, class... Args2>
struct TypeVector_cat<TypeVector<Args1...>, TypeVector<Args2...>>
{
    using type = TypeVector<Args1..., Args2...>;
};
template <class... Args1, class... Args2, class... TypeVectors>
struct TypeVector_cat<TypeVector<Args1...>, TypeVector<Args2...>, TypeVectors...>
{
    using type = typename TypeVector_cat<TypeVector<Args1..., Args2...>, TypeVectors...>::type;
};
template <class... TypeVectors>
using TypeVector_cat_t = typename TypeVector_cat<TypeVectors...>::type;

static_assert(std::is_same_v<TypeVector_cat_t<TypeVector<>, TypeVector<char>>,
                             TypeVector<char>>);
static_assert(std::is_same_v<TypeVector_cat_t<TypeVector<int>, TypeVector<int, char>>,
                             TypeVector<int, int, char>>);
static_assert(std::is_same_v<TypeVector_cat_t<TypeVector<int>, TypeVector<char>, TypeVector<double>>,
                             TypeVector<int, char, double>>);

/*
template <class TypeVec>
struct TypeVector_reverse
{
private:
    template <class>
    struct Inner;
    template <std::size_t... i>
    struct Inner<std::index_sequence<i...>>
    {
        using type = TypeVector<TypeVector_get_t<TypeVec, (TypeVec::size - i - 1)>...>;
    };

public:
    using type = typename Inner<std::make_index_sequence<TypeVec::size>>::type;
};
template <class TypeVec>
using TypeVector_reverse_t = typename TypeVector_reverse<TypeVec>::type;
static_assert(std::is_same_v<TypeVector_reverse_t<TypeVector<>>, TypeVector<>>);
static_assert(std::is_same_v<TypeVector_reverse_t<TypeVector<int, char, double>>,
                             TypeVector<double, char, int>>);


template <class TypeVec, std::size_t mid>
struct TypeVector_rotate
{
    static_assert(mid <= TypeVec::size);

private:
    template <class>
    struct Inner;
    template <std::size_t... i>
    struct Inner<std::index_sequence<i...>>
    {
        using type = TypeVector<TypeVector_get_t<TypeVec, ((i + mid) % TypeVec::size)>...>;
    };

public:
    using type = typename Inner<std::make_index_sequence<TypeVec::size>>::type;
};
template <class TypeVec, std::size_t mid>
using TypeVector_rotate_t = typename TypeVector_rotate<TypeVec, mid>::type;

static_assert(std::is_same_v<TypeVector_rotate_t<TypeVector<>, 0>, TypeVector<>>);
static_assert(std::is_same_v<TypeVector_rotate_t<TypeVector<int, char, double>, 0>,
                             TypeVector<int, char, double>>);
static_assert(std::is_same_v<TypeVector_rotate_t<TypeVector<int, char, double>, 1>,
                             TypeVector<char, double, int>>);
static_assert(std::is_same_v<TypeVector_rotate_t<TypeVector<int, char, double>, 2>,
                             TypeVector<double, int, char>>);
static_assert(std::is_same_v<TypeVector_rotate_t<TypeVector<int, char, double>, 3>,
                             TypeVector<int, char, double>>);

template <class TypeVec, std::size_t length>
struct TypeVector_right
{
    static_assert(length <= TypeVec::size);

private:
    template <std::size_t x, class TT>
    struct TrimHead
    {
    private:
        template <class>
        struct Inner;
        template <class T, class... Args>
        struct Inner<TypeVector<T, Args...>>
        {
            using type = typename TrimHead<x - 1, TypeVector<Args...>>::type;
        };

    public:
        using type = typename Inner<TT>::type;
    };
    template <class TT>
    struct TrimHead<0, TT>
    {
        using type = TT;
    };

public:
    using type = typename TrimHead<TypeVec::size - length, TypeVec>::type;
};
template <class TypeVec, std::size_t length>
using TypeVector_right_t = typename TypeVector_right<TypeVec, length>::type;

static_assert(std::is_same_v<TypeVector_right_t<TypeVector<>, 0>,
                             TypeVector<>>);
static_assert(std::is_same_v<TypeVector_right_t<TypeVector<int, char, double>, 0>,
                             TypeVector<>>);
static_assert(std::is_same_v<TypeVector_right_t<TypeVector<int, char, double>, 1>,
                             TypeVector<double>>);
static_assert(std::is_same_v<TypeVector_right_t<TypeVector<int, char, double>, 2>,
                             TypeVector<char, double>>);
static_assert(std::is_same_v<TypeVector_right_t<TypeVector<int, char, double>, 3>,
                             TypeVector<int, char, double>>);

template <class TypeVec, std::size_t length>
struct TypeVector_left
{
    static_assert(length <= TypeVec::size);
    using type = TypeVector_right_t<TypeVector_rotate_t<TypeVec, length>, length>;
};
template <class TypeVec, std::size_t length>
using TypeVector_left_t = typename TypeVector_left<TypeVec, length>::type;

static_assert(std::is_same_v<TypeVector_left_t<TypeVector<>, 0>,
                             TypeVector<>>);
static_assert(std::is_same_v<TypeVector_left_t<TypeVector<int, char, double>, 0>,
                             TypeVector<>>);
static_assert(std::is_same_v<TypeVector_left_t<TypeVector<int, char, double>, 1>,
                             TypeVector<int>>);
static_assert(std::is_same_v<TypeVector_left_t<TypeVector<int, char, double>, 2>,
                             TypeVector<int, char>>);
static_assert(std::is_same_v<TypeVector_left_t<TypeVector<int, char, double>, 3>,
                             TypeVector<int, char, double>>);

template <class TypeVec, std::size_t pos, std::size_t length = TypeVec::size - pos>
struct TypeVector_mid
{
    static_assert(pos <= TypeVec::size);
    static_assert(pos + length <= TypeVec::size);
    using type = TypeVector_left_t<TypeVector_rotate_t<TypeVec, pos>, length>;
};
template <class TypeVec, std::size_t pos, std::size_t length = TypeVec::size - pos>
using TypeVector_mid_t = typename TypeVector_mid<TypeVec, pos, length>::type;

static_assert(std::is_same_v<TypeVector_mid_t<TypeVector<int, char, double>, 0, 0>,
                             TypeVector<>>);
static_assert(std::is_same_v<TypeVector_mid_t<TypeVector<int, char, double>, 0, 1>,
                             TypeVector<int>>);
static_assert(std::is_same_v<TypeVector_mid_t<TypeVector<int, char, double>, 0, 2>,
                             TypeVector<int, char>>);
static_assert(std::is_same_v<TypeVector_mid_t<TypeVector<int, char, double>, 0, 3>,
                             TypeVector<int, char, double>>);
static_assert(std::is_same_v<TypeVector_mid_t<TypeVector<int, char, double>, 1, 0>,
                             TypeVector<>>);
static_assert(std::is_same_v<TypeVector_mid_t<TypeVector<int, char, double>, 1, 1>,
                             TypeVector<char>>);
static_assert(std::is_same_v<TypeVector_mid_t<TypeVector<int, char, double>, 1, 2>,
                             TypeVector<char, double>>);
static_assert(std::is_same_v<TypeVector_mid_t<TypeVector<int, char, double>, 2, 1>,
                             TypeVector<double>>);

static_assert(std::is_same_v<TypeVector_mid_t<TypeVector<int, char, double>, 0>,
                             TypeVector<int, char, double>>);
static_assert(std::is_same_v<TypeVector_mid_t<TypeVector<int, char, double>, 1>,
                             TypeVector<char, double>>);
static_assert(std::is_same_v<TypeVector_mid_t<TypeVector<int, char, double>, 2>,
                             TypeVector<double>>);
static_assert(std::is_same_v<TypeVector_mid_t<TypeVector<int, char, double>, 3>,
                             TypeVector<>>);

template <class TypeVec, std::size_t i, class T>
struct TypeVector_replace
{
private:
    template <std::size_t x>
    using elm = TypeVector_get_t<TypeVec, x>;

    template <class>
    struct Inner;
    template <std::size_t... index>
    struct Inner<std::index_sequence<index...>>
    {
        using type = TypeVector<std::conditional_t<index != i, elm<index>, T>...>;
    };

public:
    using type = typename Inner<std::make_index_sequence<TypeVec::size>>::type;
};
template <class TypeVec, std::size_t i, class T>
using TypeVector_replace_t = typename TypeVector_replace<TypeVec, i, T>::type;

static_assert(std::is_same_v<TypeVector_replace_t<TypeVector<int, char, double>, 2, float>,
                             TypeVector<int, char, float>>);

template <class TypeVec, std::size_t i, std::size_t j>
struct TypeVector_swap
{
private:
    template <std::size_t x>
    using elm = TypeVector_get_t<TypeVec, x>;

public:
    using type = TypeVector_replace_t<TypeVector_replace_t<TypeVec, j, elm<i>>, i, elm<j>>;
};
template <class TypeVec, std::size_t i, std::size_t j>
using TypeVector_swap_t = typename TypeVector_swap<TypeVec, i, j>::type;

static_assert(std::is_same_v<TypeVector_swap_t<TypeVector<int, char>, 0, 1>,
                             TypeVector<char, int>>);
static_assert(std::is_same_v<TypeVector_swap_t<TypeVector<int, char, double>, 0, 2>,
                             TypeVector<double, char, int>>);

template <class TT1, class TT2, class Compare>
constexpr auto TypeVector_merge(Compare compare)
{
    if constexpr (TT1::size == 0) {
        return TT2{};
    } else if constexpr (TT2::size == 0) {
        return TT1{};
    } else {
        using H1 = TypeVector_get_t<TT1, 0>;
        using H2 = TypeVector_get_t<TT2, 0>;
        if constexpr (compare(handle::TypeAdapter<H1>{}, handle::TypeAdapter<H2>{})) {
            return TypeVector_cat_t<TypeVector<H1>, decltype(TypeVector_merge<TypeVector_mid_t<TT1, 1>, TT2>(compare))>{};
        } else {
            return TypeVector_cat_t<TypeVector<H2>, decltype(TypeVector_merge<TT1, TypeVector_mid_t<TT2, 1>>(compare))>{};
        }
    }
}
template <class TypeVector1, class TypeVector2, auto compare>
using TypeVector_merge_t = decltype(TypeVector_merge<TypeVector1, TypeVector2>(compare));

namespace test_TypeVector
{
template <int x>
using INT = std::integral_constant<int, x>;

constexpr auto yes_man = [](auto...) { return true; };
constexpr auto no_man = [](auto...) { return false; };
constexpr auto lmd = [](auto x, auto y) -> bool {
    return (decltype(x)::type::value) < (decltype(y)::type::value);
};

static_assert(std::is_same_v<decltype(TypeVector_merge<TypeVector<>, TypeVector<>>(yes_man)),
                             TypeVector<>>);
static_assert(std::is_same_v<decltype(TypeVector_merge<TypeVector<int>, TypeVector<>>(yes_man)),
                             TypeVector<int>>);
static_assert(std::is_same_v<decltype(TypeVector_merge<TypeVector<>, TypeVector<int>>(yes_man)),
                             TypeVector<int>>);
static_assert(std::is_same_v<decltype(TypeVector_merge<TypeVector<int>, TypeVector<int>>(yes_man)),
                             TypeVector<int, int>>);
static_assert(std::is_same_v<decltype(TypeVector_merge<TypeVector<int>, TypeVector<char>>(no_man)),
                             TypeVector<char, int>>);
static_assert(std::is_same_v<decltype(TypeVector_merge<TypeVector<INT<0>>, TypeVector<INT<1>>>(lmd)),
                             TypeVector<INT<0>, INT<1>>>);
static_assert(std::is_same_v<decltype(TypeVector_merge<TypeVector<INT<1>, INT<3>, INT<4>>, TypeVector<INT<2>, INT<5>>>(lmd)),
                             TypeVector<INT<1>, INT<2>, INT<3>, INT<4>, INT<5>>>);
} // namespace test_TypeVector

template <class TypeVec, class Compare>
constexpr auto TypeVector_sort([[maybe_unused]] Compare compare)
{
    constexpr auto s = TypeVec::size;
    if constexpr (s < 2) {
        return TypeVec{};
    } else {
        using L = decltype(TypeVector_sort<TypeVector_left_t<TypeVec, s / 2>>(compare));
        using R = decltype(TypeVector_sort<TypeVector_mid_t<TypeVec, s / 2>>(compare));
        return TypeVector_merge<L, R>(compare);
    }
}

template <class TypeVec, auto compare>
using TypeVector_sort_t = decltype(TypeVector_sort<TypeVec>(compare));

namespace test_TypeVector
{
static_assert(std::is_same_v<decltype(TypeVector_sort<TypeVector<>>(lmd)),
                             TypeVector<>>);
static_assert(std::is_same_v<decltype(TypeVector_sort<TypeVector<INT<1>>>(lmd)),
                             TypeVector<INT<1>>>);
static_assert(std::is_same_v<decltype(TypeVector_sort<TypeVector<INT<1>, INT<2>>>(lmd)),
                             TypeVector<INT<1>, INT<2>>>);
static_assert(std::is_same_v<decltype(TypeVector_sort<TypeVector<INT<1>, INT<2>, INT<3>, INT<4>, INT<5>>>(lmd)),
                             TypeVector<INT<1>, INT<2>, INT<3>, INT<4>, INT<5>>>);
static_assert(std::is_same_v<decltype(TypeVector_sort<TypeVector<INT<5>, INT<3>, INT<4>, INT<1>, INT<2>>>(lmd)),
                             TypeVector<INT<1>, INT<2>, INT<3>, INT<4>, INT<5>>>);
} // namespace test_TypeVector

*/
} // namespace rib::type::vector
