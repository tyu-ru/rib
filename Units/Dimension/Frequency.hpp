#pragma once

#include "Basic.hpp"

namespace rib::units::dim
{

/// Frequency [T^-1]
using Frequency = ReciprocalDim_t<Time>;
static_assert(std::is_same_v<Frequency, Dimension<0, 0, -1>>);

/// Wave Number [L^-1]
using WaveNumber = ReciprocalDim_t<Length>;
static_assert(std::is_same_v<WaveNumber, Dimension<-1>>);

} // namespace rib::units::dim
