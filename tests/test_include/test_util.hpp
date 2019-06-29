#pragma once

#include "rib/Type/TypeSequence.hpp"

template <class T>
struct identity
{
    using type = T;
};

template <class, std::size_t>
struct index_seq_cat;
template <std::size_t... i, std::size_t x>
struct index_seq_cat<std::index_sequence<i...>, x>
{
    using type = std::index_sequence<i..., x>;
};
template <class S1, std::size_t x>
using index_seq_cat_t = typename index_seq_cat<S1, x>::type;

static_assert(std::is_same_v<index_seq_cat_t<std::make_index_sequence<3>, 3>,
                             std::index_sequence<0, 1, 2, 3>>);

template <class, std::size_t>
struct is_included_index;
template <std::size_t... i, std::size_t x>
struct is_included_index<std::index_sequence<i...>, x> : std::bool_constant<(... || (i == x))>
{
};
template <class S, std::size_t x>
inline constexpr bool is_included_index_v = is_included_index<S, x>::value;

static_assert(is_included_index_v<std::index_sequence<>, 0> == false);
static_assert(is_included_index_v<std::index_sequence<0>, 0> == true);
static_assert(is_included_index_v<std::index_sequence<1, 2>, 0> == false);
static_assert(is_included_index_v<std::index_sequence<1, 0, 2>, 0> == true);

/// all permutation n,m
template <std::size_t n, std::size_t m, bool repetition, std::enable_if_t<(n >= m), std::nullptr_t> = nullptr>
struct make_index_all_permutation
{
private:
    using n_index_seq = std::make_index_sequence<n>;

    template <class S1, class S2>
    struct dispatcher;

    template <class S, std::size_t x, bool = !repetition && is_included_index_v<S, x>>
    struct next_perm
    {
        using temp = index_seq_cat_t<S, x>;
        using type = typename std::conditional_t<temp::size() >= m,
                                                 identity<rib::type::TypeSequence<temp>>,
                                                 dispatcher<temp, n_index_seq>>::type;
    };
    template <class S, std::size_t x>
    struct next_perm<S, x, true>
    {
        using type = rib::type::TypeSequence<>;
    };

    template <class S1, std::size_t... i>
    struct dispatcher<S1, std::index_sequence<i...>>
    {
        using type = rib::type::TypeSequence_cat_t<typename next_perm<S1, i>::type...>;
    };

public:
    using type = std::conditional_t<m == 0, std::tuple<>, typename dispatcher<std::index_sequence<>, n_index_seq>::type::std_tuple>;
};
template <std::size_t n, std::size_t m, bool repetition>
using make_index_all_permutation_t = typename make_index_all_permutation<n, m, repetition>::type;

static_assert(std::is_same_v<make_index_all_permutation_t<0, 0, true>,
                             std::tuple<>>);
static_assert(std::is_same_v<make_index_all_permutation_t<3, 0, true>,
                             std::tuple<>>);
static_assert(std::is_same_v<make_index_all_permutation_t<3, 1, true>,
                             std::tuple<std::index_sequence<0>,
                                        std::index_sequence<1>,
                                        std::index_sequence<2>>>);
static_assert(std::is_same_v<make_index_all_permutation_t<3, 2, true>,
                             std::tuple<std::index_sequence<0, 0>, std::index_sequence<0, 1>, std::index_sequence<0, 2>,
                                        std::index_sequence<1, 0>, std::index_sequence<1, 1>, std::index_sequence<1, 2>,
                                        std::index_sequence<2, 0>, std::index_sequence<2, 1>, std::index_sequence<2, 2>>>);

static_assert(std::is_same_v<make_index_all_permutation_t<0, 0, false>,
                             std::tuple<>>);
static_assert(std::is_same_v<make_index_all_permutation_t<3, 0, false>,
                             std::tuple<>>);
static_assert(std::is_same_v<make_index_all_permutation_t<3, 1, false>,
                             std::tuple<std::index_sequence<0>,
                                        std::index_sequence<1>,
                                        std::index_sequence<2>>>);
static_assert(std::is_same_v<make_index_all_permutation_t<3, 2, false>,
                             std::tuple<std::index_sequence<0, 1>, std::index_sequence<0, 2>,
                                        std::index_sequence<1, 0>, std::index_sequence<1, 2>,
                                        std::index_sequence<2, 0>, std::index_sequence<2, 1>>>);

template <template <class...> class Template, std::size_t n, bool repetition, class... Args>
struct make_type_permutation
{
private:
    template <class>
    struct type_place;
    template <std::size_t... i>
    struct type_place<std::index_sequence<i...>>
    {
        using type = Template<std::tuple_element_t<i, std::tuple<Args...>>...>;
    };

    template <class>
    struct impl;
    template <class... S>
    struct impl<std::tuple<S...>>
    {
        using type = std::tuple<typename type_place<S>::type...>;
    };

public:
    using type = typename impl<make_index_all_permutation_t<sizeof...(Args), n, repetition>>::type;
};
template <template <class...> class Template, std::size_t n, bool repetition, class... Args>
using make_type_permutation_t = typename make_type_permutation<Template, n, repetition, Args...>::type;

static_assert(std::is_same_v<make_type_permutation_t<std::tuple, 2, false, int, char, float>,
                             std::tuple<std::tuple<int, char>, std::tuple<int, float>,
                                        std::tuple<char, int>, std::tuple<char, float>,
                                        std::tuple<float, int>, std::tuple<float, char>>>);
