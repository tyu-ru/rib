#pragma once

#include "Basic.hpp"

namespace rib::units::dim
{
/// Area [L^2]
using Area = ProductDim_t<Length, Length>;
/// Volume [L^3]
using Volume = ProductDim_t<Area, Length>;

static_assert(std::is_same_v<Area, Dimension<2>>);
static_assert(std::is_same_v<Volume, Dimension<3>>);
} // namespace rib::units::dim
