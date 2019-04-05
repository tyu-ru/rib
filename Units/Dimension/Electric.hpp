#pragma once

#include "Basic.hpp"
#include "Geometria.hpp"
#include "Kinematics.hpp"

namespace rib::units::dim
{
/// Electric Charge [TI]
using ElectricCharge = ProductDim_t<Current, Time>;
/// Charge Density [L^-3TI]
using ChargeDensity = QuotientDim_t<ElectricCharge, Volume>;

static_assert(std::is_same_v<ElectricCharge, Dimension<0, 0, 1, 1>>);
static_assert(std::is_same_v<ChargeDensity, Dimension<-3, 0, 1, 1>>);

/// Current Density [L^-2I]
using CurrentDensity = QuotientDim_t<Current, Area>;
/// Electric Field [LMT^-3I^-1]
using ElectricField = QuotientDim_t<Force, ElectricCharge>;
/// Electric Potential [L^2MT^-3I^-1]
using ElectricPotential = ProductDim_t<ElectricField, Length>;
/// Voltage [L^2MT^-3I^-1]
using Voltage = ElectricPotential;

static_assert(std::is_same_v<CurrentDensity, Dimension<-2, 0, 0, 1>>);
static_assert(std::is_same_v<ElectricField, Dimension<1, 1, -3, -1>>);
static_assert(std::is_same_v<ElectricPotential, Dimension<2, 1, -3, -1>>);
static_assert(std::is_same_v<Voltage, Dimension<2, 1, -3, -1>>);

/// Electric Conductivity [L^-3M^-1T^3I^2]
using ElectricConductivity = QuotientDim_t<CurrentDensity, ElectricField>;
/// Electric Resistivity [L^3MT-3]
using ElectricResistivity = ReciprocalDim_t<ElectricConductivity>;

static_assert(std::is_same_v<ElectricConductivity, Dimension<-3, -1, 3, 2>>);
static_assert(std::is_same_v<ElectricResistivity, Dimension<3, 1, -3, -2>>);

/// Manetic Flux Density [MT^-2I^-1]
using MagneticFluxDensity = QuotientDim_t<Force, ProductDim_t<Current, Length>>;
/// Magnetic Flux [L^2MT^-2I^-1]
using MagneticFlux = ProductDim_t<MagneticFluxDensity, Area>;
/// Inductance [L^2MT^-2I^-2]
using Inductance = QuotientDim_t<MagneticFlux, Current>;

static_assert(std::is_same_v<MagneticFluxDensity, Dimension<0, 1, -2, -1>>);
static_assert(std::is_same_v<MagneticFlux, Dimension<2, 1, -2, -1>>);
static_assert(std::is_same_v<Inductance, Dimension<2, 1, -2, -2>>);

/// Electrostatic Capacity [L^-2M^-1,T^4I^2]
using ElectrostaticCapasity = QuotientDim_t<ElectricCharge, ElectricPotential>;

static_assert(std::is_same_v<ElectrostaticCapasity, Dimension<-2, -1, 4, 2>>);

} // namespace rib::units::dim
