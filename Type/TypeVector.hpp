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

    template <template <class...> class Template>
    using Apply = Template<Args...>;

private:
    template <template <std::size_t> class UnaryOperation>
    struct Transform_impl
    {
    private:
        template <class>
        struct Inner;
        template <std::size_t... i>
        struct Inner<std::index_sequence<i...>>
        {
            using type = TypeVector<typename UnaryOperation<i>::type...>;
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
        using type = typename Transform_impl<Inner>::type;
    };

    struct ChopHead_impl
    {
        template <class...>
        struct Inner
        {
            using type = TypeVector<>;
        };
        template <class T, class... Args2>
        struct Inner<T, Args2...>
        {
            using type = TypeVector<Args2...>;
        };
        using type = typename Inner<Args...>::type;
    };

    template <std::size_t len>
    struct ChopHeads_impl
    {
        static_assert(len <= size);
        template <std::size_t len2, class>
        struct Inner
        {
            using type = typename ChopHead_impl::type::template ChopHeads<len - 1>;
        };
        template <class Dummy>
        struct Inner<0, Dummy>
        {
            using type = TypeVector<Args...>;
        };
        using type = typename Inner<len, void>::type;
    };
    template <std::size_t i, class T>
    struct Replace_impl
    {
        template <std::size_t j>
        struct Inner
        {
            using type = std::conditional_t<j == i, T, Element<j>>;
        };
        using type = typename Transform_impl<Inner>::type;
    };

public:
    template <template <std::size_t> class UnaryOperation>
    using Transform = typename Transform_impl<UnaryOperation>::type;

    using Reverse = Transform<Reverse_impl>;

    template <std::size_t mid>
    using Rotate = typename Rotate_impl<mid>::type;

    using ChopHead = typename ChopHead_impl::type;

    template <std::size_t len>
    using ChopHeads = typename ChopHeads_impl<len>::type;

    template <std::size_t len>
    using Right = ChopHeads<size - len>;

    template <std::size_t len>
    using Left = typename Reverse::template Right<len>::Reverse;

    template <std::size_t pos, std::size_t len = size - pos>
    using Mid = typename Rotate<pos>::template Left<len>;

    template <std::size_t i, class T>
    using Replace = typename Replace_impl<i, T>::type;

    template <std::size_t i, std::size_t j>
    using Swap = typename Replace<i, Element<j>>::template Replace<j, Element<i>>;

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

static_assert(std::is_same_v<TypeVector<int, char>::Apply<std::tuple>, std::tuple<int, char>>);

static_assert(std::is_same_v<TypeVector<>::Reverse, TypeVector<>>);
static_assert(std::is_same_v<TypeVector<int, char, double>::Reverse,
                             TypeVector<double, char, int>>);

static_assert(std::is_same_v<TypeVector<>::Rotate<0>, TypeVector<>>);
static_assert(std::is_same_v<TypeVector<int, char, double>::Rotate<0>,
                             TypeVector<int, char, double>>);
static_assert(std::is_same_v<TypeVector<int, char, double>::Rotate<1>,
                             TypeVector<char, double, int>>);
static_assert(std::is_same_v<TypeVector<int, char, double>::Rotate<2>,
                             TypeVector<double, int, char>>);
static_assert(std::is_same_v<TypeVector<int, char, double>::Rotate<3>,
                             TypeVector<int, char, double>>);

static_assert(std::is_same_v<TypeVector<>::ChopHead, TypeVector<>>);
static_assert(std::is_same_v<TypeVector<int>::ChopHead, TypeVector<>>);
static_assert(std::is_same_v<TypeVector<int, char>::ChopHead, TypeVector<char>>);

static_assert(std::is_same_v<TypeVector<>::ChopHeads<0>, TypeVector<>>);
static_assert(std::is_same_v<TypeVector<int, char, double>::ChopHeads<0>,
                             TypeVector<int, char, double>>);
static_assert(std::is_same_v<TypeVector<int, char, double>::ChopHeads<1>,
                             TypeVector<char, double>>);
static_assert(std::is_same_v<TypeVector<int, char, double>::ChopHeads<2>,
                             TypeVector<double>>);
static_assert(std::is_same_v<TypeVector<int, char, double>::ChopHeads<3>,
                             TypeVector<>>);

static_assert(std::is_same_v<TypeVector<>::Right<0>,
                             TypeVector<>>);
static_assert(std::is_same_v<TypeVector<int, char, double>::Right<0>,
                             TypeVector<>>);
static_assert(std::is_same_v<TypeVector<int, char, double>::Right<1>,
                             TypeVector<double>>);
static_assert(std::is_same_v<TypeVector<int, char, double>::Right<2>,
                             TypeVector<char, double>>);
static_assert(std::is_same_v<TypeVector<int, char, double>::Right<3>,
                             TypeVector<int, char, double>>);

static_assert(std::is_same_v<TypeVector<>::Left<0>,
                             TypeVector<>>);
static_assert(std::is_same_v<TypeVector<int, char, double>::Left<0>,
                             TypeVector<>>);
static_assert(std::is_same_v<TypeVector<int, char, double>::Left<1>,
                             TypeVector<int>>);
static_assert(std::is_same_v<TypeVector<int, char, double>::Left<2>,
                             TypeVector<int, char>>);
static_assert(std::is_same_v<TypeVector<int, char, double>::Left<3>,
                             TypeVector<int, char, double>>);

static_assert(std::is_same_v<TypeVector<int, char, double>::Mid<0, 0>,
                             TypeVector<>>);
static_assert(std::is_same_v<TypeVector<int, char, double>::Mid<0, 1>,
                             TypeVector<int>>);
static_assert(std::is_same_v<TypeVector<int, char, double>::Mid<0, 2>,
                             TypeVector<int, char>>);
static_assert(std::is_same_v<TypeVector<int, char, double>::Mid<0, 3>,
                             TypeVector<int, char, double>>);
static_assert(std::is_same_v<TypeVector<int, char, double>::Mid<1, 0>,
                             TypeVector<>>);
static_assert(std::is_same_v<TypeVector<int, char, double>::Mid<1, 1>,
                             TypeVector<char>>);
static_assert(std::is_same_v<TypeVector<int, char, double>::Mid<1, 2>,
                             TypeVector<char, double>>);
static_assert(std::is_same_v<TypeVector<int, char, double>::Mid<2, 1>,
                             TypeVector<double>>);

static_assert(std::is_same_v<TypeVector<int, char, double>::Mid<0>,
                             TypeVector<int, char, double>>);
static_assert(std::is_same_v<TypeVector<int, char, double>::Mid<1>,
                             TypeVector<char, double>>);
static_assert(std::is_same_v<TypeVector<int, char, double>::Mid<2>,
                             TypeVector<double>>);
static_assert(std::is_same_v<TypeVector<int, char, double>::Mid<3>,
                             TypeVector<>>);

static_assert(std::is_same_v<TypeVector<int, char, double>::Replace<2, float>,
                             TypeVector<int, char, float>>);

static_assert(std::is_same_v<TypeVector<int, char>::Swap<0, 1>,
                             TypeVector<char, int>>);
static_assert(std::is_same_v<TypeVector<int, char, double>::Swap<0, 2>,
                             TypeVector<double, char, int>>);

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

template <class V1, class V2, const auto& compare>
struct TypeVector_merge
{
private:
    static constexpr auto impl()
    {
        if constexpr (V1::size == 0) {
            return V2{};
        } else if constexpr (V2::size == 0) {
            return V1{};
        } else {
            using H1 = typename V1::template Element<0>;
            using H2 = typename V2::template Element<0>;
            if constexpr (compare(handle::TypeAdapter<H1>{}, handle::TypeAdapter<H2>{})) {
                return TypeVector_cat_t<TypeVector<H1>, typename TypeVector_merge<typename V1::ChopHead, V2, compare>::type>{};
            } else {
                return TypeVector_cat_t<TypeVector<H2>, typename TypeVector_merge<typename V2::ChopHead, V1, compare>::type>{};
            }
        }
    }

public:
    using type = std::invoke_result_t<decltype(impl)>;
};
template <class TypeVector1, class TypeVector2, const auto& compare>
using TypeVector_merge_t = typename TypeVector_merge<TypeVector1, TypeVector2, compare>::type;

namespace impl_tests
{
template <int x>
using INT = std::integral_constant<int, x>;

constexpr auto yes_man = [](auto...) { return true; };
constexpr auto no_man = [](auto...) { return false; };
constexpr auto lmd = [](auto x, auto y) -> bool {
    return (decltype(x)::type::value) < (decltype(y)::type::value);
};

static_assert(std::is_same_v<TypeVector_merge_t<TypeVector<>, TypeVector<>, yes_man>,
                             TypeVector<>>);
static_assert(std::is_same_v<TypeVector_merge_t<TypeVector<int>, TypeVector<>, yes_man>,
                             TypeVector<int>>);
static_assert(std::is_same_v<TypeVector_merge_t<TypeVector<>, TypeVector<int>, yes_man>,
                             TypeVector<int>>);
static_assert(std::is_same_v<TypeVector_merge_t<TypeVector<int>, TypeVector<char>, yes_man>,
                             TypeVector<int, char>>);
static_assert(std::is_same_v<TypeVector_merge_t<TypeVector<int>, TypeVector<char>, no_man>,
                             TypeVector<char, int>>);

static_assert(std::is_same_v<TypeVector_merge_t<TypeVector<INT<1>>, TypeVector<INT<2>>, lmd>,
                             TypeVector<INT<1>, INT<2>>>);
static_assert(std::is_same_v<TypeVector_merge_t<TypeVector<INT<1>, INT<3>, INT<4>>, TypeVector<INT<2>, INT<5>>, lmd>,
                             TypeVector<INT<1>, INT<2>, INT<3>, INT<4>, INT<5>>>);

} // namespace impl_tests

template <class TypeVec, const auto& compare>
struct TypeVector_sort
{
private:
    static constexpr auto impl()
    {
        constexpr auto s = TypeVec::size;
        if constexpr (s < 2) {
            return TypeVec{};
        } else {
            using L = typename TypeVector_sort<typename TypeVec::template Left<s / 2>, compare>::type;
            using R = typename TypeVector_sort<typename TypeVec::template Mid<s / 2>, compare>::type;
            return TypeVector_merge_t<L, R, compare>{};
        }
    }

public:
    using type = std::invoke_result_t<decltype(impl)>;
};

template <class TypeVec, const auto& compare>
using TypeVector_sort_t = typename TypeVector_sort<TypeVec, compare>::type;

namespace impl_tests
{
static_assert(std::is_same_v<TypeVector_sort_t<TypeVector<>, lmd>,
                             TypeVector<>>);
static_assert(std::is_same_v<TypeVector_sort_t<TypeVector<INT<1>>, lmd>,
                             TypeVector<INT<1>>>);
static_assert(std::is_same_v<TypeVector_sort_t<TypeVector<INT<1>, INT<2>>, lmd>,
                             TypeVector<INT<1>, INT<2>>>);
static_assert(std::is_same_v<TypeVector_sort_t<TypeVector<INT<1>, INT<2>, INT<3>, INT<4>, INT<5>>, lmd>,
                             TypeVector<INT<1>, INT<2>, INT<3>, INT<4>, INT<5>>>);
static_assert(std::is_same_v<TypeVector_sort_t<TypeVector<INT<5>, INT<3>, INT<4>, INT<1>, INT<2>>, lmd>,
                             TypeVector<INT<1>, INT<2>, INT<3>, INT<4>, INT<5>>>);
} // namespace impl_tests

} // namespace rib::type::vector
