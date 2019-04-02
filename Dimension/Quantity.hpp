#pragma once

#include <utility>
#include "Dimension.hpp"
#include "../Operators.hpp"
#include "../Type/TypeTraits.hpp"
#include "../functional_util.hpp"

namespace rib::units
{

template <class Dimension, class Value = double>
class Quantity : operators::LessThanComparable<Quantity<Dimension, Value>>,
                 operators::EqualityComparable<Quantity<Dimension, Value>>
{
    static_assert(type::is_template_specialized_by_value_v<dim::Dimension, Dimension>);

    template <class D, class V>
    friend class Quantity;

public:
    using DimensionType = Dimension;
    using ValueType = Value;

public:
    constexpr Quantity() = default;
    explicit constexpr Quantity(const ValueType& other) : val(other) {}
    constexpr Quantity(const Quantity&) = default;
    constexpr Quantity(Quantity&&) = default;
    ~Quantity() = default;

    constexpr Quantity& operator=(const Quantity&) = default;
    constexpr Quantity& operator=(Quantity&&) = default;

    template <class V>
    constexpr Quantity& operator=(const Quantity<DimensionType, V>& rhs)
    {
        val = rhs.val;
        return *this;
    }
    template <class V>
    constexpr Quantity& operator=(Quantity<DimensionType, V>&& rhs)
    {
        val = std::move(rhs.val);
        return *this;
    }

    constexpr const ValueType& value() const& { return val; }
    constexpr ValueType& value() & { return val; }
    constexpr const ValueType&& value() const&& { return std::move(val); }
    constexpr ValueType&& value() && { return std::move(val); }

    template <class V>
    constexpr bool operator<(const Quantity<DimensionType, V>& rhs) const
    {
        return val < rhs.val;
    }
    template <class V>
    constexpr bool operator==(const Quantity<DimensionType, V>& rhs) const
    {
        return val == rhs.val;
    }

    template <class V>
    constexpr Quantity& operator+=(const Quantity<DimensionType, V>& rhs)
    {
        val += rhs.val;
        return *this;
    }
    template <class V>
    constexpr Quantity& operator-=(const Quantity<DimensionType, V>& rhs)
    {
        val -= rhs.val;
        return *this;
    }

    template <class V>
    constexpr Quantity& operator*=(const Quantity<dim::Dimension<>, V>& rhs)
    {
        val *= rhs.val;
        return *this;
    }
    template <class D, class V>
    constexpr Quantity& operator*=(Quantity<D, V>) = delete;
    template <class T>
    constexpr Quantity& operator*=(const T& rhs)
    {
        val *= rhs;
        return *this;
    }

    template <class V>
    constexpr Quantity& operator/=(const Quantity<dim::Dimension<>, V>& rhs)
    {
        val /= rhs.val;
        return *this;
    }
    template <class D, class V>
    constexpr Quantity& operator/=(Quantity<D, V>) = delete;
    template <class T>
    constexpr Quantity& operator/=(const T& rhs)
    {
        val /= rhs;
        return *this;
    }

private:
    ValueType val{};
};

static_assert(std::is_default_constructible_v<Quantity<dim::Dimension<>, int>>);
static_assert(std::is_copy_constructible_v<Quantity<dim::Dimension<>, int>>);
static_assert(std::is_move_constructible_v<Quantity<dim::Dimension<>, int>>);
static_assert(std::is_constructible_v<Quantity<dim::Dimension<>, int>, int>);
static_assert(std::is_constructible_v<Quantity<dim::Dimension<>, int>, int&>);
static_assert(std::is_constructible_v<Quantity<dim::Dimension<>, int>, int&&>);
static_assert(std::is_constructible_v<Quantity<dim::Dimension<>, double>, int>);
static_assert(std::is_constructible_v<Quantity<dim::Dimension<>, double>, int&>);
static_assert(std::is_constructible_v<Quantity<dim::Dimension<>, double>, int&&>);

static_assert(std::is_copy_assignable_v<Quantity<dim::Dimension<>, int>>);
static_assert(std::is_move_assignable_v<Quantity<dim::Dimension<>, int>>);
static_assert(std::is_assignable_v<Quantity<dim::Dimension<>, int>&, Quantity<dim::Dimension<>, double>>);

static_assert(std::is_assignable_v<Quantity<dim::Dimension<>, int>&, Quantity<dim::Dimension<1>, int>> == false);
static_assert(std::is_assignable_v<Quantity<dim::Dimension<>, int>&, int> == false);

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

} // namespace rib::units
