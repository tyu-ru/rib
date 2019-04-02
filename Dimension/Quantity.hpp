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
    constexpr Quantity() = default;
    explicit constexpr Quantity(const V& other) : val(other) {}
    constexpr Quantity(const Quantity&) = default;
    constexpr Quantity(Quantity&&) = default;
    ~Quantity() = default;

    constexpr Quantity& operator=(const Quantity&) = default;
    constexpr Quantity& operator=(Quantity&&) = default;

    template <class V2>
    constexpr Quantity& operator=(const Quantity<DimensionType, V2>& rhs)
    {
        val = rhs.val;
        return *this;
    }
    template <class V2>
    constexpr Quantity& operator=(Quantity<DimensionType, V2>&& rhs)
    {
        val = std::move(rhs.val);
        return *this;
    }

    constexpr const ValueType& value() const& { return val; }
    constexpr ValueType& value() & { return val; }
    constexpr const ValueType&& value() const&& { return std::move(val); }
    constexpr ValueType&& value() && { return std::move(val); }

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

} // namespace rib::units
