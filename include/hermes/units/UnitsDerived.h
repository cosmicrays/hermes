#ifndef HERMES_UNITSDERIVED_H
#define HERMES_UNITSDERIVED_H

#include "UnitsBasic.h"
#include <array>

namespace hermes {
/**
 * \addtogroup Units
 * @{
 */

// Derived units types:
// ------------------------------------
//             l  t  m  I  T  N  J  A
QUANTITY_TYPE( 1,-2, 1, 0, 0, 0, 0, 0, QForce);
QUANTITY_TYPE(-1,-2, 1, 0, 0, 0, 0, 0, QPressure);
QUANTITY_TYPE( 2,-2, 1, 0, 0, 0, 0, 0, QTorque);
QUANTITY_TYPE( 2,-2, 1, 0, 0, 0, 0, 0, QEnergy);
QUANTITY_TYPE(-1,-2, 1, 0, 0, 0, 0, 0, QEnergyDensity);
QUANTITY_TYPE( 0, 1, 0, 1, 0, 0, 0, 0, QECharge);
QUANTITY_TYPE( 0,-2, 1,-1, 0, 0, 0, 0, QMField);
QUANTITY_TYPE( 2,-3, 1,-1, 0, 0, 0, 0, QEPotential);
QUANTITY_TYPE( 0,-1, 0, 0, 0, 0, 0, 0, QFrequency);
QUANTITY_TYPE( 2,-3, 1, 0, 0, 0, 0, 0, QPower);
QUANTITY_TYPE( 2,-2, 1,-2, 0, 0, 0, 0, QEResistance);
QUANTITY_TYPE(-2, 4,-1, 2, 0, 0, 0, 0, QECapacitance);
QUANTITY_TYPE( 2, 0, 0, 0, 0, 0, 0, 0, QArea);
QUANTITY_TYPE( 3, 0, 0, 0, 0, 0, 0, 0, QVolume);
QUANTITY_TYPE( 1,-1, 0, 0, 0, 0, 0, 0, QSpeed);
QUANTITY_TYPE( 1,-2, 0, 0, 0, 0, 0, 0, QAcceleration);
QUANTITY_TYPE(-3, 0, 0, 0, 0, 0, 0, 0, QPDensity);
QUANTITY_TYPE(-2, 0, 0, 0, 0, 0, 0, 0, QColumnDensity);
QUANTITY_TYPE(-2, 0, 0, 0, 0, 0, 0, 1, QRotationMeasure);
QUANTITY_TYPE(-5, 2,-1, 0, 0, 0, 0, 0, QPDensityPerEnergy);
QUANTITY_TYPE(-1,-2, 1, 0, 0, 0, 0, 0, QEmissivity);
QUANTITY_TYPE( 0,-2, 1, 0, 0, 0, 0, 0, QIntensity);
QUANTITY_TYPE(-4, 1,-1, 0, 0, 0, 0, 0, QDifferentialFlux);
QUANTITY_TYPE(-1, 0, 0, 0, 0, 0, 0, 0, QInverseLength);
QUANTITY_TYPE( 0, 2,-1, 0, 0, 0, 0, 0, QDiffCrossSection);
QUANTITY_TYPE(-2, 0, 0, 0, 0, 0, 0, 0, QDispersionMeasure);

// Predefined derived units:
// doc: https://en.wikipedia.org/wiki/SI_derived_unit
constexpr QArea		square_metre(1.0);
constexpr QForce	newton(1.0);
constexpr QPressure	pascal(1.0);
constexpr QEnergy	joule(1.0);
constexpr QECharge	coulomb(1.0);
constexpr QMField	tesla(1.0);
constexpr QEPotential	volt(1.0);
constexpr QFrequency	hertz(1.0);
constexpr QPower	watt(1.0);
constexpr QEResistance	ohm(1.0);
constexpr QECapacitance farad(1.0);

// SI prefixes
constexpr double yocto = 1e-24;
constexpr double zepto = 1e-21;
constexpr double atto  = 1e-18;
constexpr double femto = 1e-15;
constexpr double pico  = 1e-12;
constexpr double nano  = 1e-9;
constexpr double micro = 1e-6;
constexpr double milli  = 1e-3;
constexpr double centi = 1e-2;
constexpr double deci  = 1e-1;
constexpr double deca  = 1e1;
constexpr double hecto = 1e2;
constexpr double kilo  = 1e3;
constexpr double mega  = 1e6;
constexpr double giga  = 1e9;
constexpr double tera  = 1e12;
constexpr double peta  = 1e15;
constexpr double exa   = 1e18;
constexpr double zetta = 1e21;
constexpr double yotta = 1e24;  

#define GENERATE_PREFIXED_UNITS(_UNIT) \
	constexpr auto yocto##_UNIT = yocto * _UNIT; \
	constexpr auto zepto##_UNIT = zepto * _UNIT; \
	constexpr auto atto##_UNIT  = atto  * _UNIT; \
	constexpr auto femto##_UNIT = femto * _UNIT; \
	constexpr auto pico##_UNIT  = pico  * _UNIT; \
	constexpr auto nano##_UNIT  = nano  * _UNIT; \
	constexpr auto micro##_UNIT = micro * _UNIT; \
	constexpr auto milli##_UNIT = milli * _UNIT; \
	constexpr auto centi##_UNIT = centi * _UNIT; \
	constexpr auto deci##_UNIT  = deci  * _UNIT; \
	constexpr auto hecto##_UNIT = hecto * _UNIT; \
	constexpr auto kilo##_UNIT  = kilo  * _UNIT; \
	constexpr auto mega##_UNIT  = mega  * _UNIT; \
	constexpr auto giga##_UNIT  = giga  * _UNIT; \
	constexpr auto tera##_UNIT  = tera  * _UNIT; \
	constexpr auto peta##_UNIT  = peta  * _UNIT; \
	constexpr auto exa##_UNIT   = exa   * _UNIT; \
	constexpr auto zetta##_UNIT = zetta * _UNIT; \
	constexpr auto yotta##_UNIT = yotta * _UNIT;


// Physical constants
/* Definition of SI base units and constants used elsewhere in the code.
 Partly inherited from CRPropa3.
 Based on:
 - CODATA recommended values of the fundamental physical constants: 2006
        doi:10.1103/RevModPhys.80.633
 - IAU 2012 Resolution B2, IAU 2015 Resolution B2
        https://www.iau.org/administration/resolutions/
*/

constexpr auto e_plus		= 1.602176487e-19 * coulomb;
constexpr auto c_light		= 2.99792458e8 * metre / second;
constexpr auto c_squared 	= c_light * c_light;
constexpr auto amu 		= 1.660538921e-27 * kilogram;
constexpr auto m_proton 	= 1.67262158e-27 * kilogram;
constexpr auto m_neutron 	= 1.67492735e-27 * kilogram;
constexpr auto m_electron 	= 9.10938291e-31 * kilogram;
constexpr auto h_planck 	= 6.62606957e-34 * joule * second;
constexpr auto h_planck_bar 	= 6.62606957e-34 * joule * second / (2*pi);
constexpr auto k_boltzmann 	= 1.3806488e-23 * joule / kelvin;
constexpr auto mu0 		= (4*pi) * 1e-7 * newton / (ampere * ampere);
constexpr auto epsilon0 	= 1.0 / mu0 / c_squared;

// Predefined time-derived units:
constexpr QTime minute = 60. * second;
constexpr QTime hour = 60. * minute;
constexpr QTime day = 24. * hour;
constexpr QTime week = 7. * day;
constexpr QTime year = 365.25 * day;

// Predefined mass units:
constexpr QMass gramme = 0.001 * kilogram;
constexpr QMass tonne = 1000 * kilogram;
constexpr QMass ounce = 0.028349523125 * kilogram;
constexpr QMass pound = 16 * ounce;
constexpr QMass stone = 14 * pound;

// Predefined energy units:
constexpr QEnergy erg = 1e-7 * joule;

// Predefined length units:
GENERATE_PREFIXED_UNITS(metre)
constexpr QLength inch = 2.54 * 0.01 * metre;
constexpr QLength foot = 12 * inch;
constexpr QLength yard = 3 * foot;
constexpr QLength mile = 5280 * foot;

// Astronomical distances
constexpr QLength astronomical_unit = 149597870700.0 * metre;
constexpr QLength lightyear = year * c_light;
constexpr QLength parsec = 648000.0 / (1*pi) * astronomical_unit;
GENERATE_PREFIXED_UNITS(parsec)

// Predefined area units:
constexpr QArea hectare = 1e4 * square_metre;
constexpr QArea barn = 1e-28 * square_metre;

// Predefined volume units:
constexpr QVolume metre3 = metre*metre*metre;
constexpr QVolume centimetre3 = centimetre*centimetre*centimetre;

// Predefined mixed units:
constexpr QAcceleration G = 9.80665 *  metre / (second*second);
constexpr QForce poundforce = pound*G;
constexpr QForce kilopond = kilogram*G;
constexpr auto gauss = 1e-4 * tesla;
constexpr auto electronvolt = e_plus * volt;
constexpr QPressure bar = 100000 * pascal;
constexpr QPressure psi = pound*G / (inch*inch);

// Predefined angle units:
constexpr QAngle degree = static_cast<double>(2*pi / 360.0) * radian;

// Direction quantity (l, b)
typedef std::array<QAngle, 2> QDirection;

/** @}*/
} // namespace hermes

#endif // HERMES_UNITSDERIVED_H
