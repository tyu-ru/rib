#pragma once

#include "Basic.hpp"

namespace rib::units::dim
{

/// Velocity [LT^-1]
using Velocity = QuotientDim_t<Length, Time>;
/// Acceleration [LT^-2]
using Acceleration = QuotientDim_t<Velocity, Time>;
/// Jerk [LT^-3]
using Jerk = QuotientDim_t<Acceleration, Time>;

static_assert(std::is_same_v<Velocity, Dimension<1, 0, -1>>);
static_assert(std::is_same_v<Acceleration, Dimension<1, 0, -2>>);
static_assert(std::is_same_v<Jerk, Dimension<1, 0, -3>>);

/// Momentum [LMT^-1]
using Momentum = ProductDim_t<Velocity, Mass>;
/// Force [LMT^-2]
using Force = QuotientDim_t<Momentum, Time>;
/// Power [L^2MT^-3]
using Power = ProductDim_t<Velocity, Force>;
/// Work [L^2MT^-2]
using Work = ProductDim_t<Force, Length>;
/// Energy [L^2MT^-2]
using Energy = Work;

static_assert(std::is_same_v<Momentum, Dimension<1, 1, -1>>);
static_assert(std::is_same_v<Force, Dimension<1, 1, -2>>);
static_assert(std::is_same_v<Work, Dimension<2, 1, -2>>);
static_assert(std::is_same_v<Power, Dimension<2, 1, -3>>);
static_assert(std::is_same_v<Energy, Dimension<2, 1, -2>>);

/// Angle []
using Angle = Dimensionless;
/// Angular Velocity [T^-1]
using AngularVelocity = QuotientDim_t<Angle, Time>;
/// Angular Acceleration [T^-2]
using AngularAcceleration = QuotientDim_t<AngularVelocity, Time>;

static_assert(std::is_same_v<Angle, Dimension<>>);
static_assert(std::is_same_v<AngularVelocity, Dimension<0, 0, -1>>);
static_assert(std::is_same_v<AngularAcceleration, Dimension<0, 0, -2>>);

/// Moment of Inertia [L^2M]
using MomentOfInertia = ProductDim_t<Length, Length, Mass>;
/// Angular Momentum [L^2MT^-1]
using AngluarMomentum = ProductDim_t<Length, Momentum>;
/// Moment of Force [L^2MT^-2]
using MomentOfForce = ProductDim_t<Length, Force>;

static_assert(std::is_same_v<MomentOfInertia, Dimension<2, 1>>);
static_assert(std::is_same_v<AngluarMomentum, Dimension<2, 1, -1>>);
static_assert(std::is_same_v<MomentOfForce, Dimension<2, 1, -2>>);

} // namespace rib::units::dim
