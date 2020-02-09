// Following the idea from:
// https://benjaminjurke.com/content/articles/2015/compile-time-numerical-unit-dimension-checking/

#ifndef HERMES_UNITSQUANTITY_H
#define HERMES_UNITSQUANTITY_H

#include <ratio>
#include <cmath>
#include <ostream>

namespace hermes {
/**
 * \addtogroup Units
 * @{
 */

// Basic template class for Units
template<typename LengthDim,
	 typename TimeDim,
	 typename MassDim,
	 typename ECurrentDim,
	 typename TemperatureDim,
	 typename SubstanceDim,
	 typename LIntensityDim,
	 typename AngleDim,
	 typename SolidAngleDim>
class Quantity {
private:
    double value;

public:
    constexpr Quantity() : value(0.0) {}
    constexpr Quantity(int val) : value(static_cast<double>(val)) {}
    constexpr Quantity(double val) : value(val) {}
    constexpr Quantity(long double val) : value(static_cast<double>(val)) {}

    // The intrinsic operations for a quantity with a unit is addition and subtraction
    constexpr Quantity const& operator+=(const Quantity& rhs) {
        value += rhs.value;
        return *this;
    }
    constexpr Quantity const& operator-=(const Quantity& rhs) {
        value -= rhs.value;
        return *this;
    }

    // Returns the value of the quantity in multiples of the specified unit
    constexpr double convertTo(const Quantity& rhs) const {
        return value / rhs.value;
    }

    // returns the raw value of the quantity (should not be used)
    constexpr double getValue() const {
	return value;
    }
    
    // overload explicitly the typecast operator
    constexpr explicit operator double() const {
	return value;
    }
    constexpr explicit operator float() const {
	return static_cast<float>(value);
    }
    constexpr explicit operator int() const {
	return static_cast<int>(value);
    }
};

// print Quantity to ostream
template<typename l, typename t, typename m, typename I, typename T,
         typename N, typename J, typename A, typename SA>
constexpr std::ostream& operator<<(std::ostream& lhs, const Quantity<l, t, m, I, T, N, J, A, SA>& rhs) {
    lhs << static_cast<double>(rhs);
    return lhs;
}

// Standard arithmetic operators:
// ------------------------------

// Negation, unary operator (-X)
template<typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A, typename SA>
constexpr Quantity<l, t, m, I, T, N, J, A, SA> 
	operator-(const Quantity<l, t, m, I, T, N, J, A, SA>& rhs) {
    return Quantity<l, t, m, I, T, N, J, A, SA>(-1 * static_cast<double>(rhs));
}

// Addition (X+Y)
template<typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A, typename SA>
constexpr Quantity<l, t, m, I, T, N, J, A, SA> 
	operator+(const Quantity<l, t, m, I, T, N, J, A, SA>& lhs,
		  const Quantity<l, t, m, I, T, N, J, A, SA>& rhs) {
    return Quantity<l, t, m, I, T, N, J, A, SA>(static_cast<double>(lhs) + static_cast<double>(rhs));
}

// Subtraction (X-Y) 
template<typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A, typename SA>
constexpr Quantity<l, t, m, I, T, N, J, A, SA> 
	operator-(const Quantity<l, t, m, I, T, N, J, A, SA>& lhs,
		  const Quantity<l, t, m, I, T, N, J, A, SA>& rhs) {
    return Quantity<l, t, m, I, T, N, J, A, SA>(static_cast<double>(lhs) - static_cast<double>(rhs));
}

// Multiplication (X*Y)
template<typename l1, typename t1, typename m1, typename I1, typename T1,
	 typename N1, typename J1, typename A1, typename SA1,
         typename l2, typename t2, typename m2, typename I2, typename T2,
         typename N2, typename J2, typename A2, typename SA2>
constexpr Quantity<std::ratio_add<l1, l2>, std::ratio_add<t1, t2>, 
                   std::ratio_add<m1, m2>, std::ratio_add<I1, I2>,
                   std::ratio_add<T1, T2>, std::ratio_add<N1, N2>,
                   std::ratio_add<J1, J2>, std::ratio_add<A1, A2>,
		   std::ratio_add<SA1, SA2>> 
    operator*(const Quantity<l1, t1, m1, I1, T1, N1, J1, A1, SA1>& lhs,
	      const Quantity<l2, t2, m2, I2, T2, N2, J2, A2, SA2>& rhs) {
    return Quantity<std::ratio_add<l1, l2>, std::ratio_add<t1, t2>,
                    std::ratio_add<m1, m2>, std::ratio_add<I1, I2>,
                    std::ratio_add<T1, T2>, std::ratio_add<N1, N2>,
                    std::ratio_add<J1, J2>, std::ratio_add<A1, A2>,
		    std::ratio_add<SA1, SA2>> 
                    (static_cast<double>(lhs)*static_cast<double>(rhs));
}

// Multiplication *=
template<typename l1, typename t1, typename m1, typename I1, typename T1,
	 typename N1, typename J1, typename A1, typename SA1,
         typename l2, typename t2, typename m2, typename I2, typename T2,
         typename N2, typename J2, typename A2, typename SA2>
constexpr Quantity<std::ratio_add<l1, l2>, std::ratio_add<t1, t2>, 
                   std::ratio_add<m1, m2>, std::ratio_add<I1, I2>,
                   std::ratio_add<T1, T2>, std::ratio_add<N1, N2>,
                   std::ratio_add<J1, J2>, std::ratio_add<A1, A2>,
		   std::ratio_add<SA1, SA2>> 
    operator*=(const Quantity<l1, t1, m1, I1, T1, N1, J1, A1, SA1>& lhs,
	       const Quantity<l2, t2, m2, I2, T2, N2, J2, A2, SA2>& rhs) {
    return Quantity<std::ratio_add<l1, l2>, std::ratio_add<t1, t2>,
                    std::ratio_add<m1, m2>, std::ratio_add<I1, I2>,
                    std::ratio_add<T1, T2>, std::ratio_add<N1, N2>,
                    std::ratio_add<J1, J2>, std::ratio_add<A1, A2>,
		    std::ratio_add<SA1, SA2>>
                    (static_cast<double>(lhs))*static_cast<double>(rhs);
}

// Multiplication with scalar (a*X, X*a)
template<typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A, typename SA>
constexpr Quantity<l, t, m, I, T, N, J, A, SA> 
    operator*(const double& lhs, const Quantity<l, t, m, I, T, N, J, A, SA>& rhs) {
    return Quantity<l, t, m, I, T, N, J, A, SA>(lhs*static_cast<double>(rhs));
}
template<typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A, typename SA>
constexpr Quantity<l, t, m, I, T, N, J, A, SA> 
    operator*(const Quantity<l, t, m, I, T, N, J, A, SA>& lhs, const double& rhs) {
    return Quantity<l, t, m, I, T, N, J, A, SA>(static_cast<double>(lhs)*rhs);
}

template<typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A, typename SA>
constexpr Quantity<l, t, m, I, T, N, J, A, SA> 
    operator*=(const Quantity<l, t, m, I, T, N, J, A, SA>& lhs,
	       const double& rhs) {
    return Quantity<l, t, m, I, T, N, J, A, SA>(static_cast<double>(lhs)*rhs);
}

// Division (X/Y)
template<typename l1, typename t1, typename m1, typename I1, typename T1,
	 typename N1, typename J1, typename A1, typename SA1,
         typename l2, typename t2, typename m2, typename I2, typename T2,
         typename N2, typename J2, typename A2, typename SA2>
constexpr Quantity<std::ratio_subtract<l1, l2>, std::ratio_subtract<t1, t2>, 
                   std::ratio_subtract<m1, m2>, std::ratio_subtract<I1, I2>,
                   std::ratio_subtract<T1, T2>, std::ratio_subtract<N1, N2>,
                   std::ratio_subtract<J1, J2>, std::ratio_subtract<A1, A2>,
		   std::ratio_subtract<SA1, SA2>> 
    operator/(const Quantity<l1, t1, m1, I1, T1, N1, J1, A1, SA1>& lhs,
              const Quantity<l2, t2, m2, I2, T2, N2, J2, A2, SA2>& rhs) {
    return Quantity<std::ratio_subtract<l1, l2>, std::ratio_subtract<t1, t2>,
                    std::ratio_subtract<m1, m2>, std::ratio_subtract<I1, I2>,
                    std::ratio_subtract<T1, T2>, std::ratio_subtract<N1, N2>,
                    std::ratio_subtract<J1, J2>, std::ratio_subtract<A1, A2>,
		    std::ratio_subtract<SA1, SA2>>
                    (static_cast<double>(lhs) / static_cast<double>(rhs));
}

// Division with scalar (a/X)
template<typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A, typename SA>
constexpr Quantity<std::ratio_subtract<std::ratio<0>, l>, std::ratio_subtract<std::ratio<0>, t>,
		   std::ratio_subtract<std::ratio<0>, m>, std::ratio_subtract<std::ratio<0>, I>,
		   std::ratio_subtract<std::ratio<0>, T>, std::ratio_subtract<std::ratio<0>, N>,
                   std::ratio_subtract<std::ratio<0>, J>, std::ratio_subtract<std::ratio<0>, A>,
		   std::ratio_subtract<std::ratio<0>, SA>> 
    operator/(double x, const Quantity<l, t, m, I, T, N, J, A, SA>& rhs) {
    return Quantity<std::ratio_subtract<std::ratio<0>, l>, std::ratio_subtract<std::ratio<0>, t>,
		    std::ratio_subtract<std::ratio<0>, m>, std::ratio_subtract<std::ratio<0>, I>,
		    std::ratio_subtract<std::ratio<0>, T>, std::ratio_subtract<std::ratio<0>, N>,
                    std::ratio_subtract<std::ratio<0>, J>, std::ratio_subtract<std::ratio<0>, A>,
		    std::ratio_subtract<std::ratio<0>, SA>> 
                    (x / static_cast<double>(rhs));
}

// Division with scalar (X/a)
template<typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A, typename SA>
constexpr Quantity<l, t, m, I, T, N, J, A, SA> 
    operator/(const Quantity<l, t, m, I, T, N, J, A, SA>& lhs,
	      double rhs) {
    return Quantity<l, t, m, I, T, N, J, A, SA>(static_cast<double>(lhs) / rhs);
}

template<typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A, typename SA>
constexpr Quantity<l, t, m, I, T, N, J, A, SA> 
    operator/=(const Quantity<l, t, m, I, T, N, J, A, SA>& lhs,
	       const double& rhs) {
    return Quantity<l, t, m, I, T, N, J, A, SA>(static_cast<double>(lhs) / rhs);
}


// Comparison operators for quantities:
// ------------------------------------
template<typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A, typename SA>
constexpr bool operator==(const Quantity<l, t, m, I, T, N, J, A, SA>& lhs,
			  const Quantity<l, t, m, I, T, N, J, A, SA>& rhs) {
    return (static_cast<double>(lhs) == static_cast<double>(rhs));
}

template<typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A, typename SA>
constexpr bool operator!=(const Quantity<l, t, m, I, T, N, J, A, SA>& lhs,
			  const Quantity<l, t, m, I, T, N, J, A, SA>& rhs) {
    return (static_cast<double>(lhs) != static_cast<double>(rhs));
}

template<typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A, typename SA>
constexpr bool operator<=(const Quantity<l, t, m, I, T, N, J, A, SA>& lhs,
			  const Quantity<l, t, m, I, T, N, J, A, SA>& rhs) {
    return (static_cast<double>(lhs) <= static_cast<double>(rhs));
}

template<typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A, typename SA>
constexpr bool operator>=(const Quantity<l, t, m, I, T, N, J, A, SA>& lhs,
			  const Quantity<l, t, m, I, T, N, J, A, SA>& rhs) {
    return (static_cast<double>(lhs) >= static_cast<double>(rhs));
}

template<typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A, typename SA>
constexpr bool operator< (const Quantity<l, t, m, I, T, N, J, A, SA>& lhs,
			  const Quantity<l, t, m, I, T, N, J, A, SA>& rhs) {
    return (static_cast<double>(lhs)<static_cast<double>(rhs));
}

template<typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A, typename SA>
constexpr bool operator> (const Quantity<l, t, m, I, T, N, J, A, SA>& lhs,
			  const Quantity<l, t, m, I, T, N, J, A, SA>& rhs) {
    return (static_cast<double>(lhs)>static_cast<double>(rhs));
}

template<typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A, typename SA>
constexpr Quantity<l, t, m, I, T, N, J, A, SA>
    fabs(const Quantity<l, t, m, I, T, N, J, A, SA>& num) {
    return Quantity<l, t, m, I, T, N, J, A, SA>(std::fabs(static_cast<double>(num)));
}

template<typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A, typename SA>
constexpr Quantity<l, t, m, I, T, N, J, A, SA>
    floor(const Quantity<l, t, m, I, T, N, J, A, SA>& num) {
    return Quantity<l, t, m, I, T, N, J, A, SA>(std::floor(static_cast<double>(num)));
}

template<typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A, typename SA>
constexpr Quantity<l, t, m, I, T, N, J, A, SA>
    round(const Quantity<l, t, m, I, T, N, J, A, SA>& num) {
    return Quantity<l, t, m, I, T, N, J, A, SA>(std::round(static_cast<double>(num)));
}


// Computes the floating-point remainder of the division operation
template<typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A, typename SA>
constexpr Quantity<l, t, m, I, T, N, J, A, SA>
    fmod(const Quantity<l, t, m, I, T, N, J, A, SA>& first,
	 const Quantity<l, t, m, I, T, N, J, A, SA>& second) {
    return Quantity<l, t, m, I, T, N, J, A, SA>(std::fmod(first.getValue(), second.getValue()));
}

// sqrt()
template<typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A, typename SA>
constexpr Quantity<std::ratio_divide<l, std::ratio<2>>, std::ratio_divide<t, std::ratio<2>>, 
                   std::ratio_divide<m, std::ratio<2>>, std::ratio_divide<I, std::ratio<2>>,
                   std::ratio_divide<T, std::ratio<2>>, std::ratio_divide<N, std::ratio<2>>, 
                   std::ratio_divide<J, std::ratio<2>>, std::ratio_divide<A, std::ratio<2>>,
		   std::ratio_divide<SA, std::ratio<2>>>
    sqrt(const Quantity<l, t, m, I, T, N, J, A, SA>& num) {
    return Quantity<std::ratio_divide<l, std::ratio<2>>, std::ratio_divide<t, std::ratio<2>>, 
                    std::ratio_divide<m, std::ratio<2>>, std::ratio_divide<I, std::ratio<2>>,
                    std::ratio_divide<T, std::ratio<2>>, std::ratio_divide<N, std::ratio<2>>, 
                    std::ratio_divide<J, std::ratio<2>>, std::ratio_divide<A, std::ratio<2>>,
		    std::ratio_divide<SA, std::ratio<2>>> 
                    (std::sqrt(static_cast<double>(num)));
}
constexpr double sqrt(double num) {
    return std::sqrt(num);
}

// pow() 
template<int power, typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A, typename SA>
constexpr Quantity<std::ratio_multiply<l, std::ratio<power>>, std::ratio_multiply<t, std::ratio<power>>,
                   std::ratio_multiply<m, std::ratio<power>>, std::ratio_multiply<I, std::ratio<power>>,
                   std::ratio_multiply<T, std::ratio<power>>, std::ratio_multiply<N, std::ratio<power>>,
                   std::ratio_multiply<J, std::ratio<power>>, std::ratio_multiply<A, std::ratio<power>>,
		   std::ratio_multiply<SA, std::ratio<power>>>
 pow(const Quantity<l, t, m, I, T, N, J, A, SA>& num) {
    return Quantity<std::ratio_multiply<l, std::ratio<power>>, std::ratio_multiply<t, std::ratio<power>>, 
                    std::ratio_multiply<m, std::ratio<power>>, std::ratio_multiply<I, std::ratio<power>>,
                    std::ratio_multiply<T, std::ratio<power>>, std::ratio_multiply<N, std::ratio<power>>, 
                    std::ratio_multiply<J, std::ratio<power>>, std::ratio_multiply<A, std::ratio<power>>,
		    std::ratio_multiply<SA, std::ratio<power>>> 
                    (std::pow(static_cast<double>(num), power));
}

// squared()
template<typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A, typename SA>
constexpr Quantity<std::ratio_multiply<l, std::ratio<2>>, std::ratio_multiply<t, std::ratio<2>>,
                   std::ratio_multiply<m, std::ratio<2>>, std::ratio_multiply<I, std::ratio<2>>,
                   std::ratio_multiply<T, std::ratio<2>>, std::ratio_multiply<N, std::ratio<2>>,
                   std::ratio_multiply<J, std::ratio<2>>, std::ratio_multiply<A, std::ratio<2>>,
		   std::ratio_multiply<SA, std::ratio<2>>>
 squared(const Quantity<l, t, m, I, T, N, J, A, SA>& num) {
    return Quantity<std::ratio_multiply<l, std::ratio<2>>, std::ratio_multiply<t, std::ratio<2>>, 
                    std::ratio_multiply<m, std::ratio<2>>, std::ratio_multiply<I, std::ratio<2>>,
                    std::ratio_multiply<T, std::ratio<2>>, std::ratio_multiply<N, std::ratio<2>>, 
                    std::ratio_multiply<J, std::ratio<2>>, std::ratio_multiply<A, std::ratio<2>>,
		    std::ratio_multiply<SA, std::ratio<2>>> 
                    (static_cast<double>(num)*static_cast<double>(num));
}

// Abbrevation for defining quantities and their squared values
#define QUANTITY_TYPE(_ldim, _tdim, _mdim, _Idim, _Tdim, _Ndim, _Jdim, _Adim, _SAdim, name) \
    typedef Quantity<std::ratio<_ldim>, std::ratio<_tdim>, std::ratio<_mdim>, \
		     std::ratio<_Idim>, std::ratio<_Tdim>, std::ratio<_Ndim>, \
		     std::ratio<_Jdim>, std::ratio<_Adim>, std::ratio<_SAdim>> name; \
    typedef Quantity<std::ratio_multiply<std::ratio<_ldim>, std::ratio<2>>, std::ratio_multiply<std::ratio<_tdim>, std::ratio<2>>, \
                   std::ratio_multiply<std::ratio<_mdim>, std::ratio<2>>, std::ratio_multiply<std::ratio<_Idim>, std::ratio<2>>, \
                   std::ratio_multiply<std::ratio<_Tdim>, std::ratio<2>>, std::ratio_multiply<std::ratio<_Ndim>, std::ratio<2>>, \
                   std::ratio_multiply<std::ratio<_Jdim>, std::ratio<2>>, std::ratio_multiply<std::ratio<_Adim>, std::ratio<2>>, \
		   std::ratio_multiply<std::ratio<_SAdim>, std::ratio<2>>> name ## Squared;

// Predefined (physical unit) quantity types:
// ------------------------------------------

// Replacement of "double" type
QUANTITY_TYPE(0, 0, 0, 0, 0, 0, 0, 0, 0, QNumber);

// Basic physical quantity types
// ------------------------------------
//             l  t  m  I  T  N  J  A SA
QUANTITY_TYPE(1, 0, 0, 0, 0, 0, 0, 0, 0, QLength);
QUANTITY_TYPE(0, 1, 0, 0, 0, 0, 0, 0, 0, QTime);
QUANTITY_TYPE(0, 0, 1, 0, 0, 0, 0, 0, 0, QMass);
QUANTITY_TYPE(0, 0, 0, 1, 0, 0, 0, 0, 0, QECurrent);
QUANTITY_TYPE(0, 0, 0, 0, 1, 0, 0, 0, 0, QTemperature);
QUANTITY_TYPE(0, 0, 0, 0, 0, 1, 0, 0, 0, QSubstance);
QUANTITY_TYPE(0, 0, 0, 0, 0, 0, 1, 0, 0, QLIntensity);
QUANTITY_TYPE(0, 0, 0, 0, 0, 0, 0, 1, 0, QAngle);
QUANTITY_TYPE(0, 0, 0, 0, 0, 0, 0, 0, 1, QSolidAngle);

// Derived units types:
// ------------------------------------
//             l  t  m  I  T  N  J  A
QUANTITY_TYPE( 1,-2, 1, 0, 0, 0, 0, 0, 0, QForce);
QUANTITY_TYPE(-1,-2, 1, 0, 0, 0, 0, 0, 0, QPressure);
QUANTITY_TYPE( 2,-2, 1, 0, 0, 0, 0, 0, 0, QTorque);
QUANTITY_TYPE( 2,-2, 1, 0, 0, 0, 0, 0, 0, QEnergy);
QUANTITY_TYPE(-1,-2, 1, 0, 0, 0, 0, 0, 0, QEnergyDensity);
QUANTITY_TYPE( 0, 1, 0, 1, 0, 0, 0, 0, 0, QECharge);
QUANTITY_TYPE( 0,-2, 1,-1, 0, 0, 0, 0, 0, QMField);
QUANTITY_TYPE( 2,-3, 1,-1, 0, 0, 0, 0, 0, QEPotential);
QUANTITY_TYPE( 0,-1, 0, 0, 0, 0, 0, 0, 0, QFrequency);
QUANTITY_TYPE( 2,-3, 1, 0, 0, 0, 0, 0, 0, QPower);
QUANTITY_TYPE( 2,-2, 1,-2, 0, 0, 0, 0, 0, QEResistance);
QUANTITY_TYPE(-2, 4,-1, 2, 0, 0, 0, 0, 0, QECapacitance);
QUANTITY_TYPE( 2, 0, 0, 0, 0, 0, 0, 0, 0, QArea);
QUANTITY_TYPE( 3, 0, 0, 0, 0, 0, 0, 0, 0, QVolume);
QUANTITY_TYPE( 1,-1, 0, 0, 0, 0, 0, 0, 0, QSpeed);
QUANTITY_TYPE( 1,-2, 0, 0, 0, 0, 0, 0, 0, QAcceleration);
QUANTITY_TYPE(-3, 0, 0, 0, 0, 0, 0, 0, 0, QPDensity);
QUANTITY_TYPE(-2, 0, 0, 0, 0, 0, 0, 0, 0, QColumnDensity);
QUANTITY_TYPE(-2, 0, 0, 0, 0, 0, 0, 1, 0, QRotationMeasure);
QUANTITY_TYPE(-5, 2,-1, 0, 0, 0, 0, 0, 0, QPDensityPerEnergy);
QUANTITY_TYPE(-1,-2, 1, 0, 0, 0, 0, 0, 0, QEmissivity);
QUANTITY_TYPE( 0,-2, 1, 0, 0, 0, 0, 0, 0, QIntensity);
QUANTITY_TYPE(-4, 1,-1, 0, 0, 0, 0, 0, 0, QDifferentialFlux);
QUANTITY_TYPE(-4, 1,-1, 0, 0, 0, 0, 0,-1, QDifferentialIntensity);
QUANTITY_TYPE(-1, 0, 0, 0, 0, 0, 0, 0, 0, QInverseLength);
QUANTITY_TYPE( 0, 2,-1, 0, 0, 0, 0, 0, 0, QDiffCrossSection);
QUANTITY_TYPE(-2, 0, 0, 0, 0, 0, 0, 0, 0, QDispersionMeasure);
QUANTITY_TYPE( 1,-2, 1, 0, 0, 0, 0, 0, 0, QStoppingPower);

/** @}*/
} // namespace hermes

#endif // HERMES_UNITSBASIC_H
