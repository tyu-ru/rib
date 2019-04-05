#pragma once

#include <type_traits>
#include "../Type/UniqueType.hpp"

namespace rib::number
{

template <class T>
struct Algebra
{
    constexpr Algebra(T) {}
    constexpr Algebra() {}
};

#define UNIQUE_ALGEBRA Algebra([] {})
using Algebra0 = Algebra<type::UniqueType0>;
using Algebra1 = Algebra<type::UniqueType1>;
using Algebra2 = Algebra<type::UniqueType2>;
using Algebra3 = Algebra<type::UniqueType3>;
using Algebra4 = Algebra<type::UniqueType4>;
using Algebra5 = Algebra<type::UniqueType5>;
using Algebra6 = Algebra<type::UniqueType6>;
using Algebra7 = Algebra<type::UniqueType7>;

template <class T1, class T2>
struct Summation
{
};
template <class T1, class T2>
struct Difference
{
};
template <class T1, class T2>
struct Product
{
};
template <class T1, class T2>
struct Quotient
{
};

template <class T1, class T2>
inline constexpr auto operator+(Algebra<T1>, Algebra<T2>)
    -> Summation<Algebra<T1>, Algebra<T2>> { return {}; }
template <class T1, class T2>
inline constexpr auto operator-(Algebra<T1>, Algebra<T2>)
    -> Difference<Algebra<T1>, Algebra<T2>> { return {}; }
template <class T1, class T2>
inline constexpr auto operator*(Algebra<T1>, Algebra<T2>)
    -> Product<Algebra<T1>, Algebra<T2>> { return {}; }
template <class T1, class T2>
inline constexpr auto operator/(Algebra<T1>, Algebra<T2>)
    -> Quotient<Algebra<T1>, Algebra<T2>> { return {}; }

static_assert([] {
    auto a = UNIQUE_ALGEBRA;
    auto b = UNIQUE_ALGEBRA;
    return !std::is_same_v<decltype(a), decltype(b)>;
}());

static_assert([] {
    auto a = UNIQUE_ALGEBRA;
    auto b = UNIQUE_ALGEBRA;
    auto c = a + b;
    auto d = b + a;
    return std::is_same_v<decltype(c), Summation<decltype(a), decltype(b)>> &&
           !std::is_same_v<decltype(c), decltype(d)>;
}());
static_assert([] {
    auto a = UNIQUE_ALGEBRA;
    auto b = UNIQUE_ALGEBRA;
    auto c = a - b;
    auto d = b - a;
    return std::is_same_v<decltype(c), Difference<decltype(a), decltype(b)>> &&
           !std::is_same_v<decltype(c), decltype(d)>;
}());
static_assert([] {
    auto a = UNIQUE_ALGEBRA;
    auto b = UNIQUE_ALGEBRA;
    auto c = a * b;
    auto d = b * a;
    return std::is_same_v<decltype(c), Product<decltype(a), decltype(b)>> &&
           !std::is_same_v<decltype(c), decltype(d)>;
}());
static_assert([] {
    auto a = UNIQUE_ALGEBRA;
    auto b = UNIQUE_ALGEBRA;
    auto c = a / b;
    auto d = b / a;
    return std::is_same_v<decltype(c), Quotient<decltype(a), decltype(b)>> &&
           !std::is_same_v<decltype(c), decltype(d)>;
}());

} // namespace rib::number
