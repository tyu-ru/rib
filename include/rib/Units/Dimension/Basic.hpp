#pragma once

#include "Dimension.hpp"

namespace rib::units::dim
{

/// DimensionLess [1]
using Dimensionless =
    Dimension<0, 0, 0, 0, 0, 0>;
/// Length [L]
using Length =
    Dimension<1, 0, 0, 0, 0, 0>;
/// Mass [M]
using Mass =
    Dimension<0, 1, 0, 0, 0, 0>;
/// Time [T]
using Time =
    Dimension<0, 0, 1, 0, 0, 0>;
/// Current [I]
using Current =
    Dimension<0, 0, 0, 1, 0, 0>;
/// Temperature [Theta]
using Temperature =
    Dimension<0, 0, 0, 0, 1, 0>;
/// Amount of Substance [N]
using Amount =
    Dimension<0, 0, 0, 0, 0, 1>;

} // namespace rib::units::dim