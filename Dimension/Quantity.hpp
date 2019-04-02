#pragma once

#include <utility>
#include "Dimension.hpp"
#include "../Type/TypeTraits.hpp"
#include "../functional_util.hpp"

namespace rib::units
{

template <class Dimention, class Value = double>
class Quantity
{
    static_assert(type::is_template_specialized_by_value_v<dim::Dimension, Dimention>);

    template <class D, class V>
    friend class Quantity;

public:
    using DimensionType = Dimention;
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

static_assert(func::is_lessable_r_v<bool, Quantity<dim::Dimension<>, int>, Quantity<dim::Dimension<>, int>>);
static_assert(func::is_lessable_r_v<bool, Quantity<dim::Dimension<>, int>, Quantity<dim::Dimension<>, long>>);
static_assert(func::is_lessable_r_v<bool, Quantity<dim::Dimension<>, int>, Quantity<dim::Dimension<1>, int>> == false);

static_assert(Quantity<dim::Dimension<>, int>{1} < Quantity<dim::Dimension<>, long>{2});

} // namespace rib::units
