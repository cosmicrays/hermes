#ifndef HERMES_UNITSDERIVED_H
#define HERMES_UNITSDERIVED_H

#include "UnitsBasic.h"
#include "UnitsFunctions.h"

#include <array>

namespace hermes {
/**
 * \addtogroup Units
 * @{
 */

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
constexpr QLength r_electron	= 1.0/(4*pi*epsilon0) * pow<2>(e_plus) / (m_electron*c_squared);
constexpr auto sigma_Thompson 	= (8.0*pi/3.0) * pow<2>(r_electron); 
constexpr auto alpha_fine	= mu0 / (4.0*pi) * pow<2>(e_plus) * c_light / h_planck_bar; // Fine structure const.

// Predefined time-derived units:
constexpr QTime minute = 60. * second;
constexpr QTime hour = 60. * minute;
constexpr QTime day = 24. * hour;
constexpr QTime week = 7. * day;
constexpr QTime year = 365.25 * day;

// Predefined mass units:
constexpr QMass tonne = 1000 * kilogram;
constexpr QMass ounce = 0.028349523125 * kilogram;
constexpr QMass pound = 16 * ounce;
constexpr QMass stone = 14 * pound;

// Predefined energy units:
constexpr QEnergy erg = 1e-7 * joule;

// Predefined length units:
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
constexpr QVolume kilometre3 = kilometre*kilometre*kilometre;

// Predefined mixed units:
constexpr QAcceleration G = 9.80665 *  metre / (second*second);
constexpr QForce poundforce = pound*G;
constexpr QForce kilopond = kilogram*G;
constexpr auto gauss = 1e-4 * tesla;
constexpr auto electronvolt = e_plus * volt;
constexpr QPressure bar = 100000 * pascal;
constexpr QPressure psi = pound*G / (inch*inch);

// Predefined angle units:
constexpr QAngle degree = (pi / 180.0) * radian;

// Direction quantity (l, b)
typedef std::array<QAngle, 2> QDirection;

/** @}*/
} // namespace hermes

#endif // HERMES_UNITSDERIVED_H
