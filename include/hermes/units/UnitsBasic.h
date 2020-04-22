#ifndef HERMES_UNITSBASIC_H
#define HERMES_UNITSBASIC_H

#include <cmath>
#include <ostream>
#include <ratio>

#ifndef HERMES_UNITSDISABLE
#include "UnitsQClass.h"
#else
#include "UnitsSimple.h"
#endif

#include "UnitsSIPrefixes.h"

namespace hermes {
namespace units {
/**
 * \addtogroup Units
 * @{
 */

// Predefined basic SI units
constexpr QLength metre(1.0);
constexpr QTime second(1.0);
constexpr QMass gram(0.001);
constexpr QECurrent ampere(1.0);
constexpr QTemperature kelvin(1.0);
constexpr QSubstance mole(1.0);
constexpr QLIntensity candela(1.0);
constexpr QAngle radian(1.0);
constexpr QSolidAngle steradian(1.0);

// SI prefixes for basic SI untis
GENERATE_PREFIXED_UNITS(metre)
GENERATE_PREFIXED_UNITS(second)
GENERATE_PREFIXED_UNITS(gram)
GENERATE_PREFIXED_UNITS(ampere)
GENERATE_PREFIXED_UNITS(kelvin)
GENERATE_PREFIXED_UNITS(mole)
GENERATE_PREFIXED_UNITS(candela)

// symbols with prefixes for basic SI units
GENERATE_SHORT_PREFIXED_UNITS(metre, m)
GENERATE_SHORT_PREFIXED_UNITS(second, s)
GENERATE_SHORT_PREFIXED_UNITS(gram, g)
GENERATE_SHORT_PREFIXED_UNITS(ampere, a)
GENERATE_SHORT_PREFIXED_UNITS(kelvin, K)
GENERATE_SHORT_PREFIXED_UNITS(mole, mol)
GENERATE_SHORT_PREFIXED_UNITS(candela, cd)

// Math constants
#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif
constexpr double pi = M_PI;

#ifndef M_E
#define M_E 0.5772156649015328606065120900824
#endif
constexpr double eulerGamma = M_E;

/** @}*/
} // namespace units
} // namespace hermes

#endif // HERMES_UNITSBASIC_H
