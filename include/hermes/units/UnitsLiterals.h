#ifndef HERMES_UNITSLITERALS_H
#define HERMES_UNITSLITERALS_H

#ifndef HERMES_UNITSDISABLE
#include "UnitsBasic.h"
#else
#include "UnitsSimple.h"
#endif

namespace hermes {
/**
 * \addtogroup Units
 * @{
 */

#define BASELITERAL(_QUANTITY, _STRING) \
	constexpr _QUANTITY operator"" _STRING(long double x) { \
		return _QUANTITY(x); } \
	constexpr _QUANTITY operator"" _STRING(unsigned long long int x) { \
		return _QUANTITY(static_cast<double>(x)); }

#define LITERAL(_QUANTITY, _STRING, _UNIT) \
	constexpr _QUANTITY operator"" _STRING(long double x) { \
		return x*_UNIT; } \
	constexpr _QUANTITY operator"" _STRING(unsigned long long int x) { \
		return static_cast<double>(x)*_UNIT; }

// Conversion macro, which utilizes the string literals
#define ConvertTo(_x, _y) (_x).convertTo(1.0_##_y)
#define ToSIValue(_x) (_x).getValue()

// Constants:
// ---------------------	
LITERAL(long double, _pi, pi)

// Angular unit literals:
// ----------------------
BASELITERAL(QAngle, _rad)
LITERAL(QAngle, _deg, degree)
BASELITERAL(QSolidAngle, _sr)

// Physical unit literals:
// -----------------------

// literals for length units
BASELITERAL(QLength, _m)
LITERAL(QLength, _mm, millimetre)
LITERAL(QLength, _cm, centimetre)
LITERAL(QLength, _km, kilometre)
LITERAL(QLength, _mi, mile)
LITERAL(QLength, _yd, yard)
LITERAL(QLength, _in, foot)
LITERAL(QLength, _pc, parsec)
LITERAL(QLength, _kpc, kiloparsec)

// literals for area units
LITERAL(QArea, _m2, (metre*metre))
LITERAL(QArea, _cm2, (centimetre*centimetre))
LITERAL(QArea, _barn, barn)
LITERAL(QArea, _mbarn, milli*barn)

// literals for volume units
LITERAL(QVolume, _cm3, (centimetre3))
LITERAL(QVolume, _m3, (metre3))

// literals for energy units
LITERAL(QEnergy, _J, joule)
LITERAL(QEnergy, _erg, erg)
LITERAL(QEnergy, _eV, electronvolt)
LITERAL(QEnergy, _keV, kilo*electronvolt)
LITERAL(QEnergy, _MeV, mega*electronvolt)
LITERAL(QEnergy, _GeV, giga*electronvolt)
LITERAL(QEnergy, _TeV, tera*electronvolt)
LITERAL(QEnergy, _PeV, peta*electronvolt)
LITERAL(QEnergy, _EeV, exa*electronvolt)

// literals for speed units
BASELITERAL(QSpeed, _mps)
LITERAL(QSpeed, _miph, mile/hour)
LITERAL(QSpeed, _kmph, kilo*metre/hour)

// literal for frequency units
BASELITERAL(QFrequency, _Hz)
LITERAL(QFrequency, _kHz, kilo*hertz)
LITERAL(QFrequency, _MHz, mega*hertz)
LITERAL(QFrequency, _GHz, giga*hertz)
LITERAL(QFrequency, _THz, tera*hertz)

// literals for magnetic field strength units
BASELITERAL(QMField, _T)
LITERAL(QMField, _nG, nano*gauss)
LITERAL(QMField, _muG, micro*gauss)

// literals for temperature units
BASELITERAL(QTemperature, _K)
LITERAL(QTemperature, _muK, micro*kelvin)

// literals for time units
BASELITERAL(QTime, _s)
LITERAL(QTime, _ns, nano*second)
LITERAL(QTime, _ms, micro*second)
LITERAL(QTime, _min, minute)
LITERAL(QTime, _h, hour)
LITERAL(QTime, _day, day)

// literals for mass units
BASELITERAL(QMass, _kg)
LITERAL(QMass, _g, gram)
LITERAL(QMass, _t, tonne)
LITERAL(QMass, _oz, ounce)
LITERAL(QMass, _lb, pound)
LITERAL(QMass, _st, stone)

// literals for acceleration units
BASELITERAL(QAcceleration, _mps2)
LITERAL(QAcceleration, _G, G)

// literals for force units
BASELITERAL(QForce, _N)
LITERAL(QForce, _lbf, poundforce)
LITERAL(QForce, _kp, kilopond)

// literals for pressure units
BASELITERAL(QPressure, _Pa)
LITERAL(QPressure, _bar, bar)
LITERAL(QPressure, _psi, psi)

/** @}*/
} // namespace hermes

#endif // HERMES_UNITSLITERALS_H
