#pragma once

#include <utility>
#include <functional>
#include "Dimension.hpp"
#include "../Type/TypeTraits.hpp"

namespace rib::units
{

template <class Dimention, class V = double>
class Quantity
{
    static_assert(type::is_template_specialized_by_value_v<dim::Dimension, Dimention>);

public:
    using DimensionType = Dimention;
    using ValueType = V;

public:
    constexpr Quantity(const V& other) : val(other) {}
    constexpr Quantity() = default;
    constexpr Quantity(Quantity&&) = default;
    constexpr Quantity(const Quantity&) = default;
    constexpr Quantity& operator=(Quantity&&) = default;
    constexpr Quantity& operator=(const Quantity&) = default;
    ~Quantity() = default;

    constexpr const ValueType& value() const& { return val; }
    constexpr ValueType& value() & { return val; }
    constexpr const ValueType&& value() const&& { return std::move(val); }
    constexpr ValueType&& value() && { return std::move(val); }

private:
    ValueType val;
};

template <class D, class V>
inline constexpr auto makeQuantity(V&& v)
{
    return Quantity<D, std::decay_t<V>>{std::forward<V>(v)};
}

template <class D, class V1, class V2>
inline constexpr bool operator==(const Quantity<D, V1>& lhs, const Quantity<D, V2>& rhs)
{
    return lhs.value() == rhs.value();
}

template <class D, class V1, class V2>
inline constexpr auto operator+(const Quantity<D, V1>& lhs, const Quantity<D, V2>& rhs)
{
    return makeQuantity<D>(lhs.value() + rhs.value());
}

template <class D, class V1, class V2>
inline constexpr auto operator-(const Quantity<D, V1>& lhs, const Quantity<D, V2>& rhs)
{
    return makeQuantity<D>(lhs.value() - rhs.value());
}

template <class D1, class D2, class V1, class V2>
inline constexpr auto operator*(const Quantity<D1, V1>& lhs, const Quantity<D2, V2>& rhs)
{
    return makeQuantity<dim::ProductDim_t<D1, D2>>(lhs.value() * rhs.value());
}
template <class D1, class V1, class V2>
inline constexpr auto operator*(const Quantity<D1, V1>& lhs, const V2& rhs)
{
    return makeQuantity<D1>(lhs.value() * rhs);
}
template <class D1, class V1, class V2>
inline constexpr auto operator*(const V1& lhs, const Quantity<D1, V2>& rhs)
{
    return makeQuantity<D1>(lhs * rhs.value());
}

template <class D1, class D2, class V1, class V2>
inline constexpr auto operator/(const Quantity<D1, V1>& lhs, const Quantity<D2, V2>& rhs)
{
    return makeQuantity<dim::QuotientDim_t<D1, D2>>(lhs.value() / rhs.value());
}
template <class D1, class V1, class V2>
inline constexpr auto operator/(const Quantity<D1, V1>& lhs, const V2& rhs)
{
    return makeQuantity<D1>(lhs.value() / rhs);
}
template <class D1, class V1, class V2>
inline constexpr auto operator/(const V1& lhs, const Quantity<D1, V2>& rhs)
{
    return makeQuantity<dim::ReciprocalDim_t<D1>>(lhs / rhs.value());
}

static_assert([] {
    auto q = makeQuantity<dim::Dimension<>>(1);
    return std::is_same_v<decltype(q), Quantity<dim::Dimension<>, int>> && q.value() == 1;
}());

static_assert([] {
    auto q1 = makeQuantity<dim::Dimension<>>(1);
    auto q2 = q1;
    return q2.value() == 1 &&
           std::is_assignable_v<Quantity<dim::Dimension<>, int>, Quantity<dim::Dimension<>, int>> &&
           !std::is_assignable_v<Quantity<dim::Dimension<>, int>, Quantity<dim::Dimension<1>, int>>;
}());

static_assert([] {
    auto q0 = makeQuantity<dim::Dimension<>>(1);
    auto q1 = makeQuantity<dim::Dimension<>>(2l);
    auto q2 = q0 + q1;
    return !std::is_invocable_v<std::plus<>, Quantity<dim::Dimension<>>, Quantity<dim::Dimension<1>>> &&
           std::is_same_v<decltype(q2)::ValueType, std::invoke_result_t<std::plus<>, int, long>> &&
           q2 == makeQuantity<dim::Dimension<>>(3l);
}());

static_assert([] {
    auto q0 = makeQuantity<dim::Dimension<>>(1);
    auto q1 = makeQuantity<dim::Dimension<>>(2l);
    auto q2 = q0 - q1;
    return !std::is_invocable_v<std::minus<>, Quantity<dim::Dimension<>>, Quantity<dim::Dimension<1>>> &&
           std::is_same_v<decltype(q2)::ValueType, std::invoke_result_t<std::minus<>, int, long>> &&
           q2 == makeQuantity<dim::Dimension<>>(-1l);
}());

static_assert([] {
    auto q0 = makeQuantity<dim::Dimension<1>>(3);
    auto q1 = makeQuantity<dim::Dimension<1>>(2l);
    auto q2 = q0 * q1;
    return std::is_same_v<decltype(q2)::ValueType, std::invoke_result_t<std::multiplies<>, int, long>> &&
           q2 == makeQuantity<dim::Dimension<2>>(6l);
}());

static_assert([] {
    auto q0 = makeQuantity<dim::Dimension<1>>(6);
    auto q1 = makeQuantity<dim::Dimension<1>>(2l);
    auto q2 = q0 / q1;
    return std::is_same_v<decltype(q2)::ValueType, std::invoke_result_t<std::divides<>, int, long>> &&
           q2 == makeQuantity<dim::Dimension<>>(3l);
}());

} // namespace rib::units
