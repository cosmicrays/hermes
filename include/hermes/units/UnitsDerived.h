#ifndef HERMES_UNITSDERIVED_H
#define HERMES_UNITSDERIVED_H

#include <array>

#include "UnitsBasic.h"
#include "UnitsFunctions.h"

namespace hermes { namespace units {
/**
 * \addtogroup Units
 * @{
 */

// Predefined derived units:
// doc: https://en.wikipedia.org/wiki/SI_derived_unit
constexpr QArea square_metre(1.0);
constexpr QForce newton(1.0);
constexpr QPressure pascal(1.0);
constexpr QEnergy joule(1.0);
constexpr QECharge coulomb(1.0);
constexpr QMField tesla(1.0);
constexpr QEPotential volt(1.0);
constexpr QFrequency hertz(1.0);
constexpr QPower watt(1.0);
constexpr QEResistance ohm(1.0);
constexpr QECapacitance farad(1.0);

// Physical constants
/* Definition of SI base units and constants used elsewhere in the code.
 Partly inherited from CRPropa3.
 Based on:
 - CODATA recommended values of the fundamental physical constants: 2006
    doi:10.1103/RevModPhys.80.633
 - Review of Particle Physics.
    M. Tanabashi et al. (Particle Data Group), Phys. Rev. D 98, 030001
 (2018) and 2019 update.
 - IAU 2012 Resolution B2, IAU 2015 Resolution B2
    https://www.iau.org/administration/resolutions/
*/
constexpr QECharge e_plus = 1.602176487e-19 * coulomb;
constexpr QSpeed c_light = 2.99792458e8 * metre / second;
constexpr auto c_squared = c_light * c_light;
constexpr QMass amu = 1.660538921e-27 * kilogram;
constexpr QMass m_proton = 1.67262158e-27 * kilogram;
constexpr QMass m_neutron = 1.67492735e-27 * kilogram;
constexpr QMass m_electron = 9.10938291e-31 * kilogram;
constexpr QAngularMomentum h_planck = 6.62606957e-34 * joule * second;
constexpr QAngularMomentum h_planck_bar =
    6.62606957e-34 * joule * second / (2 * pi);
constexpr auto k_boltzmann = 1.3806488e-23 * joule / kelvin;
constexpr auto mu0 = (4 * pi) * 1e-7 * newton / (ampere * ampere);
constexpr auto epsilon0 = 1.0 / mu0 / c_squared;

constexpr QLength r_electron =
    1.0 / (4 * pi * epsilon0) * (e_plus * e_plus) / (m_electron * c_squared);
constexpr QArea sigma_Thompson = 8.0 * pi / 3.0 * (r_electron * r_electron);
constexpr QNumber alpha_fine = mu0 / (4.0 * pi) * (e_plus * e_plus) * c_light /
                               h_planck_bar;  // Fine structure const.
constexpr auto Rydberg_const =
    m_electron * (e_plus * e_plus * e_plus * e_plus) /
    (8 * (epsilon0 * epsilon0) * (h_planck * h_planck * h_planck) * c_light);
constexpr QEnergy Ry = h_planck * c_light * Rydberg_const;

// Predefined time-derived units:
constexpr QTime minute = 60. * second;
constexpr QTime hour = 60. * minute;
constexpr QTime day = 24. * hour;
constexpr QTime week = 7. * day;
constexpr QTime year = 365.25 * day;

// Predefined frequency units
GENERATE_PREFIXED_UNITS(hertz)
GENERATE_SHORT_PREFIXED_UNITS(hertz, Hz)

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
constexpr QLength parsec = 648000.0 / (1 * pi) * astronomical_unit;
GENERATE_PREFIXED_UNITS(parsec)
GENERATE_SHORT_PREFIXED_UNITS(parsec, pc)

// Predefined area units:
constexpr QArea hectare = 1e4 * square_metre;
constexpr QArea barn = 1e-28 * square_metre;
GENERATE_PREFIXED_UNITS(barn)
GENERATE_SHORT_PREFIXED_UNITS(barn, barn)

// Predefined volume units:
constexpr QVolume metre3 = metre * metre * metre;
constexpr QVolume millimetre3 = millimetre * millimetre * millimetre;
constexpr QVolume centimetre3 = centimetre * centimetre * centimetre;
constexpr QVolume kilometre3 = kilometre * kilometre * kilometre;

// Predefined mixed units:
constexpr QAcceleration G = 9.80665 * metre / (second * second);
constexpr QForce poundforce = pound * G;
constexpr QForce kilopond = kilogram * G;
constexpr auto gauss = 1e-4 * tesla;
constexpr auto electronvolt = e_plus * volt;
GENERATE_PREFIXED_UNITS(electronvolt)
GENERATE_SHORT_PREFIXED_UNITS(electronvolt, eV)

constexpr QPressure bar = 100000 * pascal;
constexpr QPressure psi = pound * G / (inch * inch);

// Other elementary particles
constexpr QMass m_pi0 = 134.9770 * MeV / c_squared;

// Predefined angle units:
constexpr QAngle degree = (pi / 180.0) * radian;

// Direction quantity (l, b)
typedef std::array<QAngle, 2> QDirection;

/** @}*/
}}  // namespace hermes::units

#endif  // HERMES_UNITSDERIVED_H
