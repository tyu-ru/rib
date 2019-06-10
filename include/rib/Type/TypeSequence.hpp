#pragma once

#include <tuple>
#include <algorithm>
#include "TypeUtil.hpp"

namespace rib::type
{

/**
 * @brief type sequence
 * @tparam Args 
 */
template <class... Args>
struct TypeSequence
{
    /// sequence size
    static constexpr auto size = sizeof...(Args);

    /// element at i
    template <std::size_t i>
    using Element = std::tuple_element_t<i, std::tuple<Args...>>;

    /// apply parameter pack to template
    template <template <class...> class Template>
    using Apply = Template<Args...>;

    /// std::tuple
    using std_tuple = std::tuple<Args...>;

private:
    template <template <std::size_t> class UnaryOperation>
    struct TransformByIndex_impl
    {
    private:
        template <class>
        struct Inner;
        template <std::size_t... i>
        struct Inner<std::index_sequence<i...>>
        {
            using type = TypeSequence<typename UnaryOperation<i>::type...>;
        };

    public:
        using type = typename Inner<std::make_index_sequence<size>>::type;
    };

    template <std::size_t i>
    struct Reverse_impl
    {
        using type = Element<size - i - 1>;
    };

    template <std::size_t mid>
    struct Rotate_impl
    {
        static_assert(mid <= size);
        template <std::size_t i>
        struct Inner
        {
            using type = Element<(i + mid) % size>;
        };
        using type = typename TransformByIndex_impl<Inner>::type;
    };

    struct ChopHead_impl
    {
        template <class...>
        struct Inner
        {
            using type = TypeSequence<>;
        };
        template <class T, class... Args2>
        struct Inner<T, Args2...>
        {
            using type = TypeSequence<Args2...>;
        };
        using type = typename Inner<Args...>::type;
    };

    template <std::size_t len>
    struct ChopHeads_impl
    {
        static_assert(len <= size);
        static constexpr auto impl()
        {
            if constexpr (len == 0) {
                return TypeSequence<Args...>{};
            } else {
                return typename ChopHead_impl::type::template ChopHeads<len - 1>{};
            }
        }
        using type = std::invoke_result_t<decltype(impl)>;
    };

    template <std::size_t i, class T>
    struct Replace_impl
    {
        template <std::size_t j>
        struct Inner
        {
            using type = std::conditional_t<j == i, T, Element<j>>;
        };
        using type = typename TransformByIndex_impl<Inner>::type;
    };

public:
    /// transform elements
    template <template <class> class UnaryOperation>
    using Transform = TypeSequence<typename UnaryOperation<Args>::type...>;

    /// transform elements
    template <template <std::size_t> class UnaryOperation>
    using TransformByIndex = typename TransformByIndex_impl<UnaryOperation>::type;

    /// reverse elements
    using Reverse = TransformByIndex<Reverse_impl>;

    /// rotate sequence of elements that element `mid` is at the beginning.
    template <std::size_t mid>
    using Rotate = typename Rotate_impl<mid>::type;

    /// sequence that contains the `size - 1` rightmost types this sequence.
    using ChopHead = typename ChopHead_impl::type;

    /// sequence that contains the `size - len` rightmost types this sequence.
    template <std::size_t len>
    using ChopHeads = typename ChopHeads_impl<len>::type;

    /// sequence that contains the `len` rightmost types this sequence.
    template <std::size_t len>
    using Right = ChopHeads<size - len>;

    /// sequence that contains the `len` leftmost types this sequence.
    template <std::size_t len>
    using Left = typename Reverse::template Right<len>::Reverse;

    /// sequence that contains `len` types of this seqence, starting at the sqecified `pos` index.
    template <std::size_t pos, std::size_t len = size - pos>
    using Mid = typename Rotate<pos>::template Left<len>;

    /// replace element at i with T
    template <std::size_t i, class T>
    using Replace = typename Replace_impl<i, T>::type;

    /// swap element i & element j
    template <std::size_t i, std::size_t j>
    using Swap = typename Replace<i, Element<j>>::template Replace<j, Element<i>>;

    template <template <class> class Pred>
    static constexpr std::size_t find_if_helper()
    {
        if constexpr (size == 0) {
            return 0;
        } else {
            std::size_t result{};
            for (bool b : {Pred<Args>::value...}) {
                if (b) return result;
                ++result;
            }
            return result;
        }
    }

    template <template <class> class Pred>
    using find_if = std::integral_constant<std::size_t, find_if_helper<Pred>()>;
    template <template <class> class Pred>
    static constexpr std::size_t find_if_v = find_if<Pred>::value;

    template <class T>
    using find = find_if<TemplateBind<std::is_same, T>::template type1>;
    template <class T>
    static constexpr std::size_t find_v = find<T>::value;

    template <template <class> class Pred>
    using count_if = std::integral_constant<std::size_t, (0 + ... + (Pred<Args>::value ? 1 : 0))>;
    template <template <class> class Pred>
    static constexpr std::size_t count_if_v = count_if<Pred>::value;

    template <class T>
    using count = count_if<TemplateBind<std::is_same, T>::template type1>;
    template <class T>
    static constexpr std::size_t count_v = count<T>::value;

    template <template <class> class Pred>
    using any_of = std::bool_constant<count_if_v<Pred> != 0>;
    template <template <class> class Pred>
    static constexpr bool any_of_v = any_of<Pred>::value;

    template <template <class> class Pred>
    using all_of = std::bool_constant<count_if_v<Pred> == size>;
    template <template <class> class Pred>
    static constexpr bool all_of_v = all_of<Pred>::value;

    template <template <class> class Pred>
    using none_of = std::negation<any_of<Pred>>;
    template <template <class> class Pred>
    static constexpr bool none_of_v = none_of<Pred>::value;

    template <class T>
    using contain = std::bool_constant<count_v<T> != 0>;
    template <class T>
    static constexpr bool contain_v = contain<T>::value;

    using unique = std::bool_constant<(... && (count_v<Args> == 1))>;
    static constexpr bool unique_v = unique::value;
};

static_assert(sizeof(TypeSequence<int, char, double>) == 1);

static_assert(TypeSequence<>::size == 0);
static_assert(TypeSequence<int, char>::size == 2);

static_assert(std::is_same_v<TypeSequence<int, char>::Element<0>, int>);
static_assert(std::is_same_v<TypeSequence<int, char>::Element<1>, char>);

static_assert(std::is_same_v<TypeSequence<int, char>::Apply<std::tuple>, std::tuple<int, char>>);
static_assert(std::is_same_v<TypeSequence<int, char>::std_tuple, std::tuple<int, char>>);

static_assert(std::is_same_v<TypeSequence<int, char>::Transform<std::add_const>,
                             TypeSequence<const int, const char>>);

static_assert(std::is_same_v<TypeSequence<>::Reverse, TypeSequence<>>);
static_assert(std::is_same_v<TypeSequence<int, char, double>::Reverse,
                             TypeSequence<double, char, int>>);

static_assert(std::is_same_v<TypeSequence<>::Rotate<0>, TypeSequence<>>);
static_assert(std::is_same_v<TypeSequence<int, char, double>::Rotate<0>,
                             TypeSequence<int, char, double>>);
static_assert(std::is_same_v<TypeSequence<int, char, double>::Rotate<1>,
                             TypeSequence<char, double, int>>);
static_assert(std::is_same_v<TypeSequence<int, char, double>::Rotate<2>,
                             TypeSequence<double, int, char>>);
static_assert(std::is_same_v<TypeSequence<int, char, double>::Rotate<3>,
                             TypeSequence<int, char, double>>);

static_assert(std::is_same_v<TypeSequence<>::ChopHead, TypeSequence<>>);
static_assert(std::is_same_v<TypeSequence<int>::ChopHead, TypeSequence<>>);
static_assert(std::is_same_v<TypeSequence<int, char>::ChopHead, TypeSequence<char>>);

static_assert(std::is_same_v<TypeSequence<>::ChopHeads<0>, TypeSequence<>>);
static_assert(std::is_same_v<TypeSequence<int, char, double>::ChopHeads<0>,
                             TypeSequence<int, char, double>>);
static_assert(std::is_same_v<TypeSequence<int, char, double>::ChopHeads<1>,
                             TypeSequence<char, double>>);
static_assert(std::is_same_v<TypeSequence<int, char, double>::ChopHeads<2>,
                             TypeSequence<double>>);
static_assert(std::is_same_v<TypeSequence<int, char, double>::ChopHeads<3>,
                             TypeSequence<>>);

static_assert(std::is_same_v<TypeSequence<>::Right<0>,
                             TypeSequence<>>);
static_assert(std::is_same_v<TypeSequence<int, char, double>::Right<0>,
                             TypeSequence<>>);
static_assert(std::is_same_v<TypeSequence<int, char, double>::Right<1>,
                             TypeSequence<double>>);
static_assert(std::is_same_v<TypeSequence<int, char, double>::Right<2>,
                             TypeSequence<char, double>>);
static_assert(std::is_same_v<TypeSequence<int, char, double>::Right<3>,
                             TypeSequence<int, char, double>>);

static_assert(std::is_same_v<TypeSequence<>::Left<0>,
                             TypeSequence<>>);
static_assert(std::is_same_v<TypeSequence<int, char, double>::Left<0>,
                             TypeSequence<>>);
static_assert(std::is_same_v<TypeSequence<int, char, double>::Left<1>,
                             TypeSequence<int>>);
static_assert(std::is_same_v<TypeSequence<int, char, double>::Left<2>,
                             TypeSequence<int, char>>);
static_assert(std::is_same_v<TypeSequence<int, char, double>::Left<3>,
                             TypeSequence<int, char, double>>);

static_assert(std::is_same_v<TypeSequence<int, char, double>::Mid<0, 0>,
                             TypeSequence<>>);
static_assert(std::is_same_v<TypeSequence<int, char, double>::Mid<0, 1>,
                             TypeSequence<int>>);
static_assert(std::is_same_v<TypeSequence<int, char, double>::Mid<0, 2>,
                             TypeSequence<int, char>>);
static_assert(std::is_same_v<TypeSequence<int, char, double>::Mid<0, 3>,
                             TypeSequence<int, char, double>>);
static_assert(std::is_same_v<TypeSequence<int, char, double>::Mid<1, 0>,
                             TypeSequence<>>);
static_assert(std::is_same_v<TypeSequence<int, char, double>::Mid<1, 1>,
                             TypeSequence<char>>);
static_assert(std::is_same_v<TypeSequence<int, char, double>::Mid<1, 2>,
                             TypeSequence<char, double>>);
static_assert(std::is_same_v<TypeSequence<int, char, double>::Mid<2, 1>,
                             TypeSequence<double>>);

static_assert(std::is_same_v<TypeSequence<int, char, double>::Mid<0>,
                             TypeSequence<int, char, double>>);
static_assert(std::is_same_v<TypeSequence<int, char, double>::Mid<1>,
                             TypeSequence<char, double>>);
static_assert(std::is_same_v<TypeSequence<int, char, double>::Mid<2>,
                             TypeSequence<double>>);
static_assert(std::is_same_v<TypeSequence<int, char, double>::Mid<3>,
                             TypeSequence<>>);

static_assert(std::is_same_v<TypeSequence<int, char, double>::Replace<2, float>,
                             TypeSequence<int, char, float>>);

static_assert(std::is_same_v<TypeSequence<int, char>::Swap<0, 1>,
                             TypeSequence<char, int>>);
static_assert(std::is_same_v<TypeSequence<int, char, double>::Swap<0, 2>,
                             TypeSequence<double, char, int>>);

static_assert(TypeSequence<>::find_if_v<std::is_unsigned> == 0);
static_assert(TypeSequence<int, unsigned int>::find_if_v<std::is_unsigned> == 1);

static_assert(TypeSequence<>::find_v<int> == 0);
static_assert(TypeSequence<int>::find_v<int> == 0);
static_assert(TypeSequence<int, char>::find_v<char> == 1);
static_assert(TypeSequence<int, char>::find_v<double> == 2);

static_assert(TypeSequence<>::count_if_v<std::is_unsigned> == 0);
static_assert(TypeSequence<int, unsigned int>::count_if_v<std::is_unsigned> == 1);

static_assert(TypeSequence<>::count_v<int> == 0);
static_assert(TypeSequence<int>::count_v<int> == 1);
static_assert(TypeSequence<int, char>::count_v<char> == 1);
static_assert(TypeSequence<int, int>::count_v<int> == 2);
static_assert(TypeSequence<int, char>::count_v<double> == 0);

static_assert(TypeSequence<int, char>::any_of_v<std::is_unsigned> == false);
static_assert(TypeSequence<unsigned int, char>::any_of_v<std::is_unsigned> == true);

static_assert(TypeSequence<int, char>::all_of_v<std::is_signed> == true);
static_assert(TypeSequence<unsigned int, char>::all_of_v<std::is_signed> == false);

static_assert(TypeSequence<int, char>::none_of_v<std::is_unsigned> == true);
static_assert(TypeSequence<unsigned int, char>::none_of_v<std::is_unsigned> == false);

static_assert(TypeSequence<int>::contain_v<int> == true);
static_assert(TypeSequence<int, char>::contain_v<char> == true);
static_assert(TypeSequence<int, char>::contain_v<double> == false);

static_assert(TypeSequence<>::unique_v == true);
static_assert(TypeSequence<int>::unique_v == true);
static_assert(TypeSequence<int, char>::unique_v == true);
static_assert(TypeSequence<int, int>::unique_v == false);

/// concatenate sequences.
template <class...>
struct TypeSequence_cat;
template <class... Args1, class... Args2>
struct TypeSequence_cat<TypeSequence<Args1...>, TypeSequence<Args2...>>
{
    using type = TypeSequence<Args1..., Args2...>;
};
template <class... Args1, class... Args2, class... TypeSequences>
struct TypeSequence_cat<TypeSequence<Args1...>, TypeSequence<Args2...>, TypeSequences...>
{
    using type = typename TypeSequence_cat<TypeSequence<Args1..., Args2...>, TypeSequences...>::type;
};
template <class... TypeSequences>
using TypeSequence_cat_t = typename TypeSequence_cat<TypeSequences...>::type;

static_assert(std::is_same_v<TypeSequence_cat_t<TypeSequence<>, TypeSequence<char>>,
                             TypeSequence<char>>);
static_assert(std::is_same_v<TypeSequence_cat_t<TypeSequence<int>, TypeSequence<int, char>>,
                             TypeSequence<int, int, char>>);
static_assert(std::is_same_v<TypeSequence_cat_t<TypeSequence<int>, TypeSequence<char>, TypeSequence<double>>,
                             TypeSequence<int, char, double>>);

template <class TS1, class TS2, const auto& compare>
struct TypeSequence_merge
{
private:
    static constexpr auto impl()
    {
        if constexpr (TS1::size == 0) {
            return TS2{};
        } else if constexpr (TS2::size == 0) {
            return TS1{};
        } else {
            using H1 = typename TS1::template Element<0>;
            using H2 = typename TS2::template Element<0>;
            if constexpr (compare(TypeAdapter<H1>{}, TypeAdapter<H2>{})) {
                return TypeSequence_cat_t<TypeSequence<H1>, typename TypeSequence_merge<typename TS1::ChopHead, TS2, compare>::type>{};
            } else {
                return TypeSequence_cat_t<TypeSequence<H2>, typename TypeSequence_merge<typename TS2::ChopHead, TS1, compare>::type>{};
            }
        }
    }

public:
    using type = std::invoke_result_t<decltype(impl)>;
};
template <class TS1, class TS2, const auto& compare>
using TypeSequence_merge_t = typename TypeSequence_merge<TS1, TS2, compare>::type;

namespace impl_tests
{
template <int x>
using INT = std::integral_constant<int, x>;

constexpr auto yes_man = [](auto...) { return true; };
constexpr auto no_man = [](auto...) { return false; };
constexpr auto lmd = [](auto x, auto y) -> bool {
    return (decltype(x)::type::value) < (decltype(y)::type::value);
};

static_assert(std::is_same_v<TypeSequence_merge_t<TypeSequence<>, TypeSequence<>, yes_man>,
                             TypeSequence<>>);
static_assert(std::is_same_v<TypeSequence_merge_t<TypeSequence<int>, TypeSequence<>, yes_man>,
                             TypeSequence<int>>);
static_assert(std::is_same_v<TypeSequence_merge_t<TypeSequence<>, TypeSequence<int>, yes_man>,
                             TypeSequence<int>>);
static_assert(std::is_same_v<TypeSequence_merge_t<TypeSequence<int>, TypeSequence<char>, yes_man>,
                             TypeSequence<int, char>>);
static_assert(std::is_same_v<TypeSequence_merge_t<TypeSequence<int>, TypeSequence<char>, no_man>,
                             TypeSequence<char, int>>);

static_assert(std::is_same_v<TypeSequence_merge_t<TypeSequence<INT<1>>, TypeSequence<INT<2>>, lmd>,
                             TypeSequence<INT<1>, INT<2>>>);
static_assert(std::is_same_v<TypeSequence_merge_t<TypeSequence<INT<1>, INT<3>, INT<4>>, TypeSequence<INT<2>, INT<5>>, lmd>,
                             TypeSequence<INT<1>, INT<2>, INT<3>, INT<4>, INT<5>>>);

} // namespace impl_tests

template <class TS, const auto& compare>
struct TypeSequence_sort
{
private:
    static constexpr auto impl()
    {
        constexpr auto s = TS::size;
        if constexpr (s < 2) {
            return TS{};
        } else {
            using L = typename TypeSequence_sort<typename TS::template Left<s / 2>, compare>::type;
            using R = typename TypeSequence_sort<typename TS::template Mid<s / 2>, compare>::type;
            return TypeSequence_merge_t<L, R, compare>{};
        }
    }

public:
    using type = std::invoke_result_t<decltype(impl)>;
};

template <class TS, const auto& compare>
using TypeSequence_sort_t = typename TypeSequence_sort<TS, compare>::type;

namespace impl_tests
{
static_assert(std::is_same_v<TypeSequence_sort_t<TypeSequence<>, lmd>,
                             TypeSequence<>>);
static_assert(std::is_same_v<TypeSequence_sort_t<TypeSequence<INT<1>>, lmd>,
                             TypeSequence<INT<1>>>);
static_assert(std::is_same_v<TypeSequence_sort_t<TypeSequence<INT<1>, INT<2>>, lmd>,
                             TypeSequence<INT<1>, INT<2>>>);
static_assert(std::is_same_v<TypeSequence_sort_t<TypeSequence<INT<1>, INT<2>, INT<3>, INT<4>, INT<5>>, lmd>,
                             TypeSequence<INT<1>, INT<2>, INT<3>, INT<4>, INT<5>>>);
static_assert(std::is_same_v<TypeSequence_sort_t<TypeSequence<INT<5>, INT<3>, INT<4>, INT<1>, INT<2>>, lmd>,
                             TypeSequence<INT<1>, INT<2>, INT<3>, INT<4>, INT<5>>>);
} // namespace impl_tests

} // namespace rib::type
