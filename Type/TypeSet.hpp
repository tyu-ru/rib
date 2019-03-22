#pragma once

#include "TypeSequence.hpp"

namespace rib::type
{

template <class... Args>
struct TypeSet
{
    static_assert(TypeSequence<Args...>::unique_v);

    static constexpr std::size_t size = sizeof...(Args);

    using Sequence = TypeSequence<Args...>;

    template <class T>
    using Insert = std::conditional_t<Sequence::template contain_v<T>, TypeSet<Args...>, TypeSet<Args..., T>>;
};

static_assert(TypeSet<>::size == 0);
static_assert(TypeSet<int, char>::size == 2);

template <class, class>
struct TypeSet_same;
template <>
struct TypeSet_same<TypeSet<>, TypeSet<>> : std::true_type
{
};
template <class... Args1, class... Args2>
struct TypeSet_same<TypeSet<Args1...>, TypeSet<Args2...>>
{
private:
    static constexpr bool impl()
    {
        [[maybe_unused]] TypeSet<Args1...> ts1;
        [[maybe_unused]] TypeSet<Args2...> ts2;
        using T1 = typename TypeSequence<Args1...>::template Element<0>;
        constexpr auto i = TypeSequence<Args2...>::template find_v<T1>;
        if constexpr (i < sizeof...(Args2)) {
            return TypeSet_same<typename TypeSequence<Args1...>::ChopHead::template Apply<TypeSet>,
                                typename TypeSequence<Args2...>::template Swap<i, 0>::ChopHead::template Apply<TypeSet>>::value;
        } else {
            return false;
        }
    }

public:
    static constexpr bool value = sizeof...(Args1) == sizeof...(Args2)
                                      ? impl()
                                      : false;
};
template <class TS1, class TS2>
inline constexpr bool TypeSet_same_v = TypeSet_same<TS1, TS2>::value;

static_assert(TypeSet_same_v<TypeSet<>, TypeSet<>>);
static_assert(TypeSet_same_v<TypeSet<int>, TypeSet<int>>);
static_assert(TypeSet_same_v<TypeSet<int, char>, TypeSet<char, int>>);
static_assert(TypeSet_same_v<TypeSet<int, char>, TypeSet<int>> == false);
static_assert(TypeSet_same_v<TypeSet<int, char>, TypeSet<int, double>> == false);

static_assert(TypeSet_same_v<TypeSet<>::Insert<int>,
                             TypeSet<int>>);
static_assert(TypeSet_same_v<TypeSet<int>::Insert<int>,
                             TypeSet<int>>);
static_assert(TypeSet_same_v<TypeSet<int>::Insert<char>,
                             TypeSet<char, int>>);

template <class... Args>
struct TypeSet_construct
{
    using type = TypeSet<>;
};
template <class T, class... Args>
struct TypeSet_construct<T, Args...>
{
    using type = typename TypeSet_construct<Args...>::type::template Insert<T>;
};
template <class... Args>
using TypeSet_construct_t = typename TypeSet_construct<Args...>::type;

static_assert(TypeSet_same_v<TypeSet_construct_t<>,
                             TypeSet<>>);
static_assert(TypeSet_same_v<TypeSet_construct_t<int>,
                             TypeSet<int>>);
static_assert(TypeSet_same_v<TypeSet_construct_t<int, char>,
                             TypeSet<int, char>>);
static_assert(TypeSet_same_v<TypeSet_construct_t<int, int>,
                             TypeSet<int>>);

} // namespace rib::type
