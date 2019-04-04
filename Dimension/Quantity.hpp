#pragma once

#include <utility>
#include "Dimension.hpp"
#include "../Operators.hpp"
#include "../Type/TypeTraits.hpp"
#include "../functional_util.hpp"
#include "../Number/Algebra.hpp"

namespace rib::units
{

/**
 * @brief Dimensioned quantity
 * @details restricts operations other than those shown below@n
 * add/sub : same dimension@n
 * compound mul/div : dimensionless quantity or scalar@n
 * mul/div : calculation result dimension is product/quotient dimensions(scalar is treated as dimensionless)
 * @tparam Dimension dim::Dimension
 * @tparam Value value type
 */
template <class Dimension, class Value = double>
class Quantity : operators::LessThanComparable<Quantity<Dimension, Value>>,
                 operators::EqualityComparable<Quantity<Dimension, Value>>
{
    static_assert(type::is_template_specialized_by_value_v<dim::Dimension, Dimension>);

    template <class D, class V>
    friend class Quantity;

public:
    /// Dimension Type
    using DimensionType = Dimension;
    /// Value Type
    using ValueType = Value;

public:
    /// default constructor (value is initialized by default)
    constexpr Quantity() = default;
    /// copy constructor
    constexpr Quantity(const Quantity&) = default;
    /// move constructor
    constexpr Quantity(Quantity&&) = default;
    /// copy value
    explicit constexpr Quantity(const ValueType& other) : val(other) {}
    /// move value
    explicit constexpr Quantity(ValueType&& other) : val(std::move(other)) {}
    /// value constructor
    template <class... Args>
    explicit constexpr Quantity(std::in_place_t, Args&&... args) : val(std::forward<Args>(args)...) {}
    /// destructor
    ~Quantity() = default;

    /// copy assignment
    constexpr Quantity& operator=(const Quantity&) = default;
    /// move assignment
    constexpr Quantity& operator=(Quantity&&) = default;
    /// copy assignment(different ValueType)
    template <class V>
    constexpr Quantity& operator=(const Quantity<DimensionType, V>& rhs)
    {
        val = rhs.val;
        return *this;
    }
    /// move assignment(different ValueType)
    template <class V>
    constexpr Quantity& operator=(Quantity<DimensionType, V>&& rhs)
    {
        val = std::move(rhs.val);
        return *this;
    }

    /// value accessor
    constexpr const ValueType& value() const& { return val; }
    /// value accessor
    constexpr ValueType& value() & { return val; }
    /// value accessor
    constexpr const ValueType&& value() const&& { return std::move(val); }
    /// value accessor
    constexpr ValueType&& value() && { return std::move(val); }

    /// less compare (other compare opetators(`>`, `<=`, `>=`) are defined by operators::LessThanComparable)
    template <class D, class V1, class V2>
    friend constexpr bool operator<(const Quantity<D, V1>& lhs, const Quantity<D, V2>& rhs);
    /// equality compare (not equality operator`!=` is defined by operators::EqualityComparable)
    template <class D, class V1, class V2>
    friend constexpr bool operator==(const Quantity<D, V1>& lhs, const Quantity<D, V2>& rhs);

    /// unary +
    constexpr Quantity operator+() const { return *this; }
    /// unary -
    constexpr Quantity operator-() const { return Quantity{-val}; }

    /// compound +
    template <class V>
    constexpr Quantity& operator+=(const Quantity<DimensionType, V>& rhs)
    {
        val += rhs.val;
        return *this;
    }
    /// compound -
    template <class V>
    constexpr Quantity& operator-=(const Quantity<DimensionType, V>& rhs)
    {
        val -= rhs.val;
        return *this;
    }

    /// compound * (scalar)
    template <class T>
    constexpr Quantity& operator*=(const T& rhs)
    {
        val *= rhs;
        return *this;
    }
    /// compound * (dimensionless)
    template <class V>
    constexpr Quantity& operator*=(const Quantity<dim::Dimension<>, V>& rhs)
    {
        val *= rhs.val;
        return *this;
    }
    /// [[deleted]] compound * (not same dimension)
    template <class D, class V>
    constexpr Quantity& operator*=(Quantity<D, V>) = delete;

    /// compound / (scalar)
    template <class T>
    constexpr Quantity& operator/=(const T& rhs)
    {
        val /= rhs;
        return *this;
    }
    /// compound / (dimensionless)
    template <class V>
    constexpr Quantity& operator/=(const Quantity<dim::Dimension<>, V>& rhs)
    {
        val /= rhs.val;
        return *this;
    }
    /// [[deleted]] compound / (not same dimension)
    template <class D, class V>
    constexpr Quantity& operator/=(Quantity<D, V>) = delete;

    /// binary +
    template <class D, class V1, class V2>
    friend constexpr auto operator+(const Quantity<D, V1>& lhs, const Quantity<D, V2>& rhs)
        -> Quantity<D, decltype(V1{} + V2{})>;
    /// binary -
    template <class D, class V1, class V2>
    friend constexpr auto operator-(const Quantity<D, V1>& lhs, const Quantity<D, V2>& rhs)
        -> Quantity<D, decltype(V1{} - V2{})>;

    /// binary *
    template <class D1, class D2, class V1, class V2>
    friend constexpr auto operator*(const Quantity<D1, V1>& lhs, const Quantity<D2, V2>& rhs)
        -> Quantity<dim::ProductDim_t<D1, D2>, decltype(V1{} * V2{})>;
    /// binary * (Quantity * scalar)
    template <class D, class V1, class V2>
    friend constexpr auto operator*(const Quantity<D, V1>& lhs, const V2& rhs)
        -> Quantity<D, decltype(V1{} * V2{})>;
    /// binary * (scalar * Quantity)
    template <class D, class V1, class V2>
    friend constexpr auto operator*(const V1& lhs, const Quantity<D, V2>& rhs)
        -> Quantity<D, decltype(V1{} * V2{})>;

    /// binary /
    template <class D1, class D2, class V1, class V2>
    friend constexpr auto operator/(const Quantity<D1, V1>& lhs, const Quantity<D2, V2>& rhs)
        -> Quantity<dim::QuotientDim_t<D1, D2>, decltype(V1{} / V2{})>;
    /// binary / (Quantity * scalar)
    template <class D, class V1, class V2>
    friend constexpr auto operator/(const Quantity<D, V1>& lhs, const V2& rhs)
        -> Quantity<D, decltype(V1{} / V2{})>;
    /// binary / (scalar * Quantity)
    template <class D, class V1, class V2>
    friend constexpr auto operator/(const V1& lhs, const Quantity<D, V2>& rhs)
        -> Quantity<dim::ReciprocalDim_t<D>, decltype(V1{} / V2{})>;

private:
    ValueType val{};
};

template <class D, class V1, class V2>
inline constexpr bool operator<(const Quantity<D, V1>& lhs, const Quantity<D, V2>& rhs)
{
    return lhs.val < rhs.val;
}
template <class D, class V1, class V2>
inline constexpr bool operator==(const Quantity<D, V1>& lhs, const Quantity<D, V2>& rhs)
{
    return lhs.val == rhs.val;
}
template <class D, class V1, class V2>
inline constexpr auto operator+(const Quantity<D, V1>& lhs, const Quantity<D, V2>& rhs)
    -> Quantity<D, decltype(V1{} + V2{})>
{
    return Quantity<D, decltype(V1{} + V2{})>{lhs.val + rhs.val};
}
template <class D, class V1, class V2>
inline constexpr auto operator-(const Quantity<D, V1>& lhs, const Quantity<D, V2>& rhs)
    -> Quantity<D, decltype(V1{} - V2{})>
{
    return Quantity<D, decltype(V1{} - V2{})>{lhs.val - rhs.val};
}

template <class D1, class D2, class V1, class V2>
inline constexpr auto operator*(const Quantity<D1, V1>& lhs, const Quantity<D2, V2>& rhs)
    -> Quantity<dim::ProductDim_t<D1, D2>, decltype(V1{} * V2{})>
{
    return Quantity<dim::ProductDim_t<D1, D2>, decltype(V1{} * V2{})>{lhs.val * rhs.val};
}
template <class D, class V1, class V2>
inline constexpr auto operator*(const Quantity<D, V1>& lhs, const V2& rhs)
    -> Quantity<D, decltype(V1{} * V2{})>
{
    return Quantity<D, decltype(V1{} * V2{})>{lhs.val * rhs};
}
template <class D, class V1, class V2>
inline constexpr auto operator*(const V1& lhs, const Quantity<D, V2>& rhs)
    -> Quantity<D, decltype(V1{} * V2{})>
{
    return Quantity<D, decltype(V1{} * V2{})>{lhs * rhs.val};
}

template <class D1, class D2, class V1, class V2>
inline constexpr auto operator/(const Quantity<D1, V1>& lhs, const Quantity<D2, V2>& rhs)
    -> Quantity<dim::QuotientDim_t<D1, D2>, decltype(V1{} / V2{})>
{
    return Quantity<dim::QuotientDim_t<D1, D2>, decltype(V1{} / V2{})>{lhs.val / rhs.val};
}
template <class D, class V1, class V2>
inline constexpr auto operator/(const Quantity<D, V1>& lhs, const V2& rhs)
    -> Quantity<D, decltype(V1{} / V2{})>
{
    return Quantity<D, decltype(V1{} / V2{})>{lhs.val / rhs};
}
template <class D, class V1, class V2>
inline constexpr auto operator/(const V1& lhs, const Quantity<D, V2>& rhs)
    -> Quantity<dim::ReciprocalDim_t<D>, decltype(V1{} / V2{})>
{
    return Quantity<dim::ReciprocalDim_t<D>, decltype(V1{} / V2{})>{lhs / rhs.val};
}

static_assert(std::is_default_constructible_v<Quantity<dim::Dimension<>, int>>);
static_assert(std::is_copy_constructible_v<Quantity<dim::Dimension<>, int>>);
static_assert(std::is_move_constructible_v<Quantity<dim::Dimension<>, int>>);
static_assert(std::is_constructible_v<Quantity<dim::Dimension<>, int>, int>);
static_assert(std::is_constructible_v<Quantity<dim::Dimension<>, int>, int&>);
static_assert(std::is_constructible_v<Quantity<dim::Dimension<>, int>, int&&>);
static_assert(std::is_constructible_v<Quantity<dim::Dimension<>, double>, int>);
static_assert(std::is_constructible_v<Quantity<dim::Dimension<>, double>, int&>);
static_assert(std::is_constructible_v<Quantity<dim::Dimension<>, double>, int&&>);
static_assert(std::is_constructible_v<Quantity<dim::Dimension<>, double>, std::in_place_t, int>);

static_assert(std::is_copy_assignable_v<Quantity<dim::Dimension<>, int>>);
static_assert(std::is_move_assignable_v<Quantity<dim::Dimension<>, int>>);
static_assert(std::is_assignable_v<Quantity<dim::Dimension<>, int>&, Quantity<dim::Dimension<>, double>>);

static_assert(std::is_assignable_v<Quantity<dim::Dimension<>, int>&, Quantity<dim::Dimension<1>, int>> == false);
static_assert(std::is_assignable_v<Quantity<dim::Dimension<>, int>&, int> == false);

static_assert(func::is_unary_plusable_r_v<Quantity<dim::Dimension<>>, Quantity<dim::Dimension<>>>);
static_assert(func::is_negateable_r_v<Quantity<dim::Dimension<>>, Quantity<dim::Dimension<>>>);

static_assert((+Quantity<dim::Dimension<>, int>{1}).value() == 1);
static_assert((-Quantity<dim::Dimension<>, int>{1}).value() == -1);

static_assert(func::is_lessable_v<Quantity<dim::Dimension<>, int>, Quantity<dim::Dimension<>, int>>);
static_assert(func::is_lessable_v<Quantity<dim::Dimension<>, int>, Quantity<dim::Dimension<>, long>>);
static_assert(func::is_lessable_v<Quantity<dim::Dimension<>, int>, Quantity<dim::Dimension<1>, int>> == false);

static_assert(func::is_less_equalable_v<Quantity<dim::Dimension<>, int>, Quantity<dim::Dimension<>, int>>);
static_assert(func::is_less_equalable_v<Quantity<dim::Dimension<>, int>, Quantity<dim::Dimension<>, long>>);
static_assert(func::is_less_equalable_v<Quantity<dim::Dimension<>, int>, Quantity<dim::Dimension<1>, int>> == false);

static_assert(func::is_greaterable_v<Quantity<dim::Dimension<>, int>, Quantity<dim::Dimension<>, int>>);
static_assert(func::is_greaterable_v<Quantity<dim::Dimension<>, int>, Quantity<dim::Dimension<>, long>>);
static_assert(func::is_greaterable_v<Quantity<dim::Dimension<>, int>, Quantity<dim::Dimension<1>, int>> == false);

static_assert(func::is_greater_equalable_v<Quantity<dim::Dimension<>, int>, Quantity<dim::Dimension<>, int>>);
static_assert(func::is_greater_equalable_v<Quantity<dim::Dimension<>, int>, Quantity<dim::Dimension<>, long>>);
static_assert(func::is_greater_equalable_v<Quantity<dim::Dimension<>, int>, Quantity<dim::Dimension<1>, int>> == false);

static_assert(Quantity<dim::Dimension<>, int>{1} < Quantity<dim::Dimension<>, long>{2});
static_assert(Quantity<dim::Dimension<>, int>{2} > Quantity<dim::Dimension<>, long>{1});
static_assert(Quantity<dim::Dimension<>, int>{1} <= Quantity<dim::Dimension<>, long>{1});
static_assert(Quantity<dim::Dimension<>, int>{1} <= Quantity<dim::Dimension<>, long>{2});
static_assert(Quantity<dim::Dimension<>, int>{1} >= Quantity<dim::Dimension<>, long>{1});
static_assert(Quantity<dim::Dimension<>, int>{2} >= Quantity<dim::Dimension<>, long>{1});

static_assert(func::is_equalable_v<Quantity<dim::Dimension<>, int>, Quantity<dim::Dimension<>, int>>);
static_assert(func::is_equalable_v<Quantity<dim::Dimension<>, int>, Quantity<dim::Dimension<>, long>>);
static_assert(func::is_equalable_v<Quantity<dim::Dimension<>, int>, Quantity<dim::Dimension<1>, int>> == false);

static_assert(func::is_not_equalable_v<Quantity<dim::Dimension<>, int>, Quantity<dim::Dimension<>, int>>);
static_assert(func::is_not_equalable_v<Quantity<dim::Dimension<>, int>, Quantity<dim::Dimension<>, long>>);
static_assert(func::is_not_equalable_v<Quantity<dim::Dimension<>, int>, Quantity<dim::Dimension<1>, int>> == false);

static_assert(Quantity<dim::Dimension<>, int>{1} == Quantity<dim::Dimension<>, long>{1});
static_assert(Quantity<dim::Dimension<>, int>{1} != Quantity<dim::Dimension<>, long>{2});

static_assert(func::is_compound_plusable_r_v<Quantity<dim::Dimension<>>&, Quantity<dim::Dimension<>>, Quantity<dim::Dimension<>>>);
static_assert(func::is_compound_plusable_r_v<Quantity<dim::Dimension<>, int>&, Quantity<dim::Dimension<>, int>, Quantity<dim::Dimension<>>>);
static_assert(func::is_compound_plusable_v<Quantity<dim::Dimension<>>, Quantity<dim::Dimension<1>>> == false);

static_assert(func::is_compound_minusable_r_v<Quantity<dim::Dimension<>>&, Quantity<dim::Dimension<>>, Quantity<dim::Dimension<>>>);
static_assert(func::is_compound_minusable_r_v<Quantity<dim::Dimension<>, int>&, Quantity<dim::Dimension<>, int>, Quantity<dim::Dimension<>>>);
static_assert(func::is_compound_minusable_v<Quantity<dim::Dimension<>>, Quantity<dim::Dimension<1>>> == false);

static_assert((Quantity<dim::Dimension<>, int>{1} += Quantity<dim::Dimension<>, long>{2}).value() == 3);
static_assert((Quantity<dim::Dimension<>, int>{1} -= Quantity<dim::Dimension<>, long>{2}).value() == -1);

static_assert(func::is_compound_multipliesable_r_v<Quantity<dim::Dimension<1>>&, Quantity<dim::Dimension<1>>, int>);
static_assert(func::is_compound_multipliesable_r_v<Quantity<dim::Dimension<1>>&, Quantity<dim::Dimension<1>>, Quantity<dim::Dimension<>>>);
static_assert(func::is_compound_multipliesable_v<Quantity<dim::Dimension<1>>, Quantity<dim::Dimension<1>>> == false);

static_assert(func::is_compound_dividesable_r_v<Quantity<dim::Dimension<1>>&, Quantity<dim::Dimension<1>>, int>);
static_assert(func::is_compound_dividesable_r_v<Quantity<dim::Dimension<1>>&, Quantity<dim::Dimension<1>>, Quantity<dim::Dimension<>>>);
static_assert(func::is_compound_dividesable_v<Quantity<dim::Dimension<1>>, Quantity<dim::Dimension<1>>> == false);

static_assert((Quantity<dim::Dimension<>, int>{2} *= 3).value() == 6);
static_assert((Quantity<dim::Dimension<>, int>{2} *= Quantity<dim::Dimension<>, int>{3}).value() == 6);
static_assert((Quantity<dim::Dimension<>, int>{6} /= 3).value() == 2);
static_assert((Quantity<dim::Dimension<>, int>{6} /= Quantity<dim::Dimension<>, int>{3}).value() == 2);

static_assert(func::is_plusable_r_v<Quantity<dim::Dimension<>, decltype(number::Algebra0{} + number::Algebra1{})>,
                                    Quantity<dim::Dimension<>, number::Algebra0>, Quantity<dim::Dimension<>, number::Algebra1>>);
static_assert(func::is_plusable_v<Quantity<dim::Dimension<1>>, Quantity<dim::Dimension<>>> == false);

static_assert(func::is_minusable_r_v<Quantity<dim::Dimension<>, decltype(number::Algebra0{} - number::Algebra1{})>,
                                     Quantity<dim::Dimension<>, number::Algebra0>, Quantity<dim::Dimension<>, number::Algebra1>>);
static_assert(func::is_minusable_v<Quantity<dim::Dimension<1>>, Quantity<dim::Dimension<>>> == false);

static_assert((Quantity<dim::Dimension<>, int>{1} + Quantity<dim::Dimension<>, int>{2}).value() == 3);
static_assert((Quantity<dim::Dimension<>, int>{1} - Quantity<dim::Dimension<>, int>{2}).value() == -1);

static_assert(func::is_multipliesable_r_v<Quantity<dim::Dimension<3>, decltype(number::Algebra0{} * number::Algebra1{})>,
                                          Quantity<dim::Dimension<1>, number::Algebra0>, Quantity<dim::Dimension<2>, number::Algebra1>>);
static_assert(func::is_multipliesable_r_v<Quantity<dim::Dimension<1>, decltype(number::Algebra0{} * number::Algebra1{})>,
                                          Quantity<dim::Dimension<1>, number::Algebra0>, number::Algebra1>);
static_assert(func::is_multipliesable_r_v<Quantity<dim::Dimension<1>, decltype(number::Algebra0{} * number::Algebra1{})>,
                                          number::Algebra0, Quantity<dim::Dimension<1>, number::Algebra1>>);

static_assert(func::is_dividesable_r_v<Quantity<dim::Dimension<-1>, decltype(number::Algebra0{} / number::Algebra1{})>,
                                       Quantity<dim::Dimension<1>, number::Algebra0>, Quantity<dim::Dimension<2>, number::Algebra1>>);
static_assert(func::is_dividesable_r_v<Quantity<dim::Dimension<1>, decltype(number::Algebra0{} / number::Algebra1{})>,
                                       Quantity<dim::Dimension<1>, number::Algebra0>, number::Algebra1>);
static_assert(func::is_dividesable_r_v<Quantity<dim::Dimension<-1>, decltype(number::Algebra0{} / number::Algebra1{})>,
                                       number::Algebra0, Quantity<dim::Dimension<1>, number::Algebra1>>);

static_assert((Quantity<dim::Dimension<>, int>{2} * Quantity<dim::Dimension<>, int>{3}).value() == 6);
static_assert((Quantity<dim::Dimension<>, int>{2} * 3).value() == 6);
static_assert((3 * Quantity<dim::Dimension<>, int>{2}).value() == 6);
static_assert((Quantity<dim::Dimension<>, int>{6} / Quantity<dim::Dimension<>, int>{3}).value() == 2);
static_assert((Quantity<dim::Dimension<>, int>{6} / 3).value() == 2);
static_assert((6 / Quantity<dim::Dimension<>, int>{3}).value() == 2);

} // namespace rib::units
