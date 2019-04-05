#pragma once

#include <type_traits>

namespace rib::units::dim
{

/// Dimension of Quantity
template <int L = 0, int M = 0, int T = 0, int I = 0, int Theta = 0, int N = 0>
struct Dimension
{
};

/// multiplication Dimensions
template <class...>
struct ProductDim;
template <int... Args1, int... Args2>
struct ProductDim<Dimension<Args1...>, Dimension<Args2...>>
{
    using type = Dimension<Args1 + Args2...>;
};
template <int... Args1, int... Args2, class... Dim>
struct ProductDim<Dimension<Args1...>, Dimension<Args2...>, Dim...>
{
    using type = typename ProductDim<Dimension<Args1 + Args2...>, Dim...>::type;
};
template <class... Dim>
using ProductDim_t = typename ProductDim<Dim...>::type;

/// divide dimension
template <class...>
struct QuotientDim;
template <int... Args1, int... Args2>
struct QuotientDim<Dimension<Args1...>, Dimension<Args2...>>
{
    using type = Dimension<Args1 - Args2...>;
};
template <class Dim1, class Dim2>
using QuotientDim_t = typename QuotientDim<Dim1, Dim2>::type;

/// reciprocal Dimension
template <class>
struct ReciprocalDim;
template <int... Args>
struct ReciprocalDim<Dimension<Args...>>
{
    using type = Dimension<-Args...>;
};
template <class Dim>
using ReciprocalDim_t = typename ReciprocalDim<Dim>::type;

static_assert(std::is_same_v<ProductDim_t<Dimension<>, Dimension<>>,
                             Dimension<>>);
static_assert(std::is_same_v<ProductDim_t<Dimension<>, Dimension<>, Dimension<>>,
                             Dimension<>>);
static_assert(std::is_same_v<ProductDim_t<Dimension<1>, Dimension<1, 1>>,
                             Dimension<2, 1>>);
static_assert(std::is_same_v<ProductDim_t<Dimension<1>, Dimension<1, 1>, Dimension<1, 1, 1>>,
                             Dimension<3, 2, 1>>);
static_assert(std::is_same_v<QuotientDim_t<Dimension<1>, Dimension<1, 1>>,
                             Dimension<0, -1>>);
static_assert(std::is_same_v<ReciprocalDim_t<Dimension<1, 1>>,
                             Dimension<-1, -1>>);

} // namespace rib::units::dim
