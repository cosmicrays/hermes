#ifndef HERMES_UNITSFUNCTIONS_H
#define HERMES_UNITSFUNCTIONS_H

#include <cmath>

#include "UnitsBasic.h"
#include "UnitsSIPrefixes.h"

namespace hermes {
/**
 * \addtogroup Units
 * @{
 */

inline double exp(const QNumber &num) {
    return std::exp(static_cast<double>(num));
}
inline double expm1(const QNumber &num) {
    return std::expm1(static_cast<double>(num));
}
inline double log(const QNumber &num) {
    return std::log(static_cast<double>(num));
}
inline double fabs(double num) {
    return std::fabs(num);
}

inline double floor(const QNumber &num) {
    return std::floor(static_cast<double>(num));
}

// Typesafe trigonometric operations
inline double sin(const QAngle &num) {
    return std::sin(static_cast<double>(num));
}
inline double cos(const QAngle &num) {
    return std::cos(static_cast<double>(num));
}
inline double tan(const QAngle &num) {
    return std::tan(static_cast<double>(num));
}
inline double cosh(const QNumber &num) {
    return std::cosh(static_cast<double>(num));
}
inline double tanh(const QNumber &num) {
    return std::tanh(static_cast<double>(num));
}
inline QAngle atan2(const QLength &lhs, const QLength &rhs) {
    return QAngle(std::atan2(static_cast<double>(lhs), static_cast<double>(rhs)));
}

constexpr QAngle radian2degree(const QAngle &a) {
	return (180.0 / pi) * a;
}
constexpr QAngle degree2radian(const QAngle &a) {
	return (pi / 180.0) * a;
}

template<typename Q>
int stepFunction(const Q &quantity) {
	if (quantity >= Q(0)) return 1;
	return 0;
}

/** @}*/
} // namespace hermes

#endif // HERMES_UNITSFUNCTIONS_H
