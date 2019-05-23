// Following the idea from:
// https://benjaminjurke.com/content/articles/2015/compile-time-numerical-unit-dimension-checking/

#ifndef HERMES_UNITSBASIC_H
#define HERMES_UNITSBASIC_H

#include <ratio>
#include <cmath>
#include <ostream>

namespace hermes {

// Basic template class for Units
template<typename LengthDim,
	 typename TimeDim,
	 typename MassDim,
	 typename ECurrentDim,
	 typename TemperatureDim,
	 typename SubstanceDim,
	 typename LIntensityDim,
	 typename AngleDim>
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
         typename N, typename J, typename A>
constexpr std::ostream& operator<<(std::ostream& lhs, const Quantity<l, t, m, I, T, N, J, A>& rhs) {
    lhs << rhs.getValue();
    return lhs;
}

// Standard arithmetic operators:
// ------------------------------

// Negation, unary operator (-X)
template<typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A>
constexpr Quantity<l, t, m, I, T, N, J, A> 
	operator-(const Quantity<l, t, m, I, T, N, J, A>& rhs) {
    return Quantity<l, t, m, I, T, N, J, A>(-1 * rhs.getValue());
}

// Addition (X+Y)
template<typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A>
constexpr Quantity<l, t, m, I, T, N, J, A> 
	operator+(const Quantity<l, t, m, I, T, N, J, A>& lhs,
		  const Quantity<l, t, m, I, T, N, J, A>& rhs) {
    return Quantity<l, t, m, I, T, N, J, A>(lhs.getValue() + rhs.getValue());
}

// Subtraction (X-Y) 
template<typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A>
constexpr Quantity<l, t, m, I, T, N, J, A> 
	operator-(const Quantity<l, t, m, I, T, N, J, A>& lhs,
		  const Quantity<l, t, m, I, T, N, J, A>& rhs) {
    return Quantity<l, t, m, I, T, N, J, A>(lhs.getValue() - rhs.getValue());
}

// Multiplication (X*Y)
template<typename l1, typename t1, typename m1, typename I1, typename T1,
	 typename N1, typename J1, typename A1,
         typename l2, typename t2, typename m2, typename I2, typename T2,
         typename N2, typename J2, typename A2>
constexpr Quantity<std::ratio_add<l1, l2>, std::ratio_add<t1, t2>, 
                   std::ratio_add<m1, m2>, std::ratio_add<I1, I2>,
                   std::ratio_add<T1, T2>, std::ratio_add<N1, N2>,
                   std::ratio_add<J1, J2>, std::ratio_add<A1, A2>> 
    operator*(const Quantity<l1, t1, m1, I1, T1, N1, J1, A1>& lhs,
	      const Quantity<l2, t2, m2, I2, T2, N2, J2, A2>& rhs) {
    return Quantity<std::ratio_add<l1, l2>, std::ratio_add<t1, t2>,
                    std::ratio_add<m1, m2>, std::ratio_add<I1, I2>,
                    std::ratio_add<T1, T2>, std::ratio_add<N1, N2>,
                    std::ratio_add<J1, J2>, std::ratio_add<A1, A2>>
                    (lhs.getValue()*rhs.getValue());
}

// Multiplication *=
template<typename l1, typename t1, typename m1, typename I1, typename T1,
	 typename N1, typename J1, typename A1,
         typename l2, typename t2, typename m2, typename I2, typename T2,
         typename N2, typename J2, typename A2>
constexpr Quantity<std::ratio_add<l1, l2>, std::ratio_add<t1, t2>, 
                   std::ratio_add<m1, m2>, std::ratio_add<I1, I2>,
                   std::ratio_add<T1, T2>, std::ratio_add<N1, N2>,
                   std::ratio_add<J1, J2>, std::ratio_add<A1, A2>> 
    operator*=(const Quantity<l1, t1, m1, I1, T1, N1, J1, A1>& lhs,
	       const Quantity<l2, t2, m2, I2, T2, N2, J2, A2>& rhs) {
    return Quantity<std::ratio_add<l1, l2>, std::ratio_add<t1, t2>,
                    std::ratio_add<m1, m2>, std::ratio_add<I1, I2>,
                    std::ratio_add<T1, T2>, std::ratio_add<N1, N2>,
                    std::ratio_add<J1, J2>, std::ratio_add<A1, A2>>
                    (lhs.getValue()*rhs.getValue());
}

// Multiplication with scalar (a*X, X*a)
template<typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A>
constexpr Quantity<l, t, m, I, T, N, J, A> 
    operator*(const double& lhs, const Quantity<l, t, m, I, T, N, J, A>& rhs) {
    return Quantity<l, t, m, I, T, N, J, A>(lhs*rhs.getValue());
}
template<typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A>
constexpr Quantity<l, t, m, I, T, N, J, A> 
    operator*(const Quantity<l, t, m, I, T, N, J, A>& lhs, const double& rhs) {
    return Quantity<l, t, m, I, T, N, J, A>(lhs.getValue()*rhs);
}

template<typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A>
constexpr Quantity<l, t, m, I, T, N, J, A> 
    operator*=(const Quantity<l, t, m, I, T, N, J, A>& lhs,
	       const double& rhs) {
    return Quantity<l, t, m, I, T, N, J, A>(lhs.getValue()*rhs);
}

// Division (X/Y)
template<typename l1, typename t1, typename m1, typename I1, typename T1,
	 typename N1, typename J1, typename A1,
         typename l2, typename t2, typename m2, typename I2, typename T2,
         typename N2, typename J2, typename A2>
constexpr Quantity<std::ratio_subtract<l1, l2>, std::ratio_subtract<t1, t2>, 
                   std::ratio_subtract<m1, m2>, std::ratio_subtract<I1, I2>,
                   std::ratio_subtract<T1, T2>, std::ratio_subtract<N1, N2>,
                   std::ratio_subtract<J1, J2>, std::ratio_subtract<A1, A2>> 
    operator/(const Quantity<l1, t1, m1, I1, T1, N1, J1, A1>& lhs,
              const Quantity<l2, t2, m2, I2, T2, N2, J2, A2>& rhs) {
    return Quantity<std::ratio_subtract<l1, l2>, std::ratio_subtract<t1, t2>,
                    std::ratio_subtract<m1, m2>, std::ratio_subtract<I1, I2>,
                    std::ratio_subtract<T1, T2>, std::ratio_subtract<N1, N2>,
                    std::ratio_subtract<J1, J2>, std::ratio_subtract<A1, A2>>
                    (lhs.getValue() / rhs.getValue());
}

// Division with scalar (a/X)
template<typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A>
constexpr Quantity<std::ratio_subtract<std::ratio<0>, l>, std::ratio_subtract<std::ratio<0>, t>,
		   std::ratio_subtract<std::ratio<0>, m>, std::ratio_subtract<std::ratio<0>, I>,
		   std::ratio_subtract<std::ratio<0>, T>, std::ratio_subtract<std::ratio<0>, N>,
                   std::ratio_subtract<std::ratio<0>, J>, std::ratio_subtract<std::ratio<0>, A>> 
    operator/(double x, const Quantity<l, t, m, I, T, N, J, A>& rhs) {
    return Quantity<std::ratio_subtract<std::ratio<0>, l>, std::ratio_subtract<std::ratio<0>, t>,
		    std::ratio_subtract<std::ratio<0>, m>, std::ratio_subtract<std::ratio<0>, I>,
		    std::ratio_subtract<std::ratio<0>, T>, std::ratio_subtract<std::ratio<0>, N>,
                    std::ratio_subtract<std::ratio<0>, J>, std::ratio_subtract<std::ratio<0>, A>> 
                    (x / rhs.getValue());
}

// Division with scalar (X/a)
template<typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A>
constexpr Quantity<l, t, m, I, T, N, J, A> 
    operator/(const Quantity<l, t, m, I, T, N, J, A>& lhs,
	      double rhs) {
    return Quantity<l, t, m, I, T, N, J, A>(lhs.getValue() / rhs);
}

template<typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A>
constexpr Quantity<l, t, m, I, T, N, J, A> 
    operator/=(const Quantity<l, t, m, I, T, N, J, A>& lhs,
	       const double& rhs) {
    return Quantity<l, t, m, I, T, N, J, A>(lhs.getValue() / rhs);
}


// Comparison operators for quantities:
// ------------------------------------
template<typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A>
constexpr bool operator==(const Quantity<l, t, m, I, T, N, J, A>& lhs,
			  const Quantity<l, t, m, I, T, N, J, A>& rhs) {
    return (lhs.getValue() == rhs.getValue());
}

template<typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A>
constexpr bool operator!=(const Quantity<l, t, m, I, T, N, J, A>& lhs,
			  const Quantity<l, t, m, I, T, N, J, A>& rhs) {
    return (lhs.getValue() != rhs.getValue());
}

template<typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A>
constexpr bool operator<=(const Quantity<l, t, m, I, T, N, J, A>& lhs,
			  const Quantity<l, t, m, I, T, N, J, A>& rhs) {
    return (lhs.getValue() <= rhs.getValue());
}

template<typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A>
constexpr bool operator>=(const Quantity<l, t, m, I, T, N, J, A>& lhs,
			  const Quantity<l, t, m, I, T, N, J, A>& rhs) {
    return (lhs.getValue() >= rhs.getValue());
}

template<typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A>
constexpr bool operator< (const Quantity<l, t, m, I, T, N, J, A>& lhs,
			  const Quantity<l, t, m, I, T, N, J, A>& rhs) {
    return (lhs.getValue()<rhs.getValue());
}

template<typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A>
constexpr bool operator> (const Quantity<l, t, m, I, T, N, J, A>& lhs,
			  const Quantity<l, t, m, I, T, N, J, A>& rhs) {
    return (lhs.getValue()>rhs.getValue());
}

template<typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A>
constexpr Quantity<l, t, m, I, T, N, J, A>
    fabs(const Quantity<l, t, m, I, T, N, J, A>& num) {
    return Quantity<l, t, m, I, T, N, J, A>(std::fabs(num.getValue()));
}

template<typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A>
constexpr Quantity<l, t, m, I, T, N, J, A>
    round(const Quantity<l, t, m, I, T, N, J, A>& num) {
    return Quantity<l, t, m, I, T, N, J, A>(std::round(num.getValue()));
}


// Computes the floating-point remainder of the division operation
template<typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A>
constexpr Quantity<l, t, m, I, T, N, J, A>
    fmod(const Quantity<l, t, m, I, T, N, J, A>& first, const Quantity<l, t, m, I, T, N, J, A>& second) {
    return Quantity<l, t, m, I, T, N, J, A>(std::fmod(first.getValue(), second.getValue()));
}

// sqrt()
template<typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A>
constexpr Quantity<std::ratio_divide<l, std::ratio<2>>, std::ratio_divide<t, std::ratio<2>>, 
                   std::ratio_divide<m, std::ratio<2>>, std::ratio_divide<I, std::ratio<2>>,
                   std::ratio_divide<T, std::ratio<2>>, std::ratio_divide<N, std::ratio<2>>, 
                   std::ratio_divide<J, std::ratio<2>>, std::ratio_divide<A, std::ratio<2>>> 
    sqrt(const Quantity<l, t, m, I, T, N, J, A>& num) {
    return Quantity<std::ratio_divide<l, std::ratio<2>>, std::ratio_divide<t, std::ratio<2>>, 
                    std::ratio_divide<m, std::ratio<2>>, std::ratio_divide<I, std::ratio<2>>,
                    std::ratio_divide<T, std::ratio<2>>, std::ratio_divide<N, std::ratio<2>>, 
                    std::ratio_divide<J, std::ratio<2>>, std::ratio_divide<A, std::ratio<2>>> 
                    (std::sqrt(num.getValue()));
}
constexpr double sqrt(double num) {
    return std::sqrt(num);
}

// pow() 
template<int power, typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A>
constexpr Quantity<std::ratio_multiply<l, std::ratio<power>>, std::ratio_multiply<t, std::ratio<power>>,
                   std::ratio_multiply<m, std::ratio<power>>, std::ratio_multiply<I, std::ratio<power>>,
                   std::ratio_multiply<T, std::ratio<power>>, std::ratio_multiply<N, std::ratio<power>>,
                   std::ratio_multiply<J, std::ratio<power>>, std::ratio_multiply<A, std::ratio<power>>>
 pow(const Quantity<l, t, m, I, T, N, J, A>& num) {
    return Quantity<std::ratio_multiply<l, std::ratio<power>>, std::ratio_multiply<t, std::ratio<power>>, 
                    std::ratio_multiply<m, std::ratio<power>>, std::ratio_multiply<I, std::ratio<power>>,
                    std::ratio_multiply<T, std::ratio<power>>, std::ratio_multiply<N, std::ratio<power>>, 
                    std::ratio_multiply<J, std::ratio<power>>, std::ratio_multiply<A, std::ratio<power>>> 
                    (std::pow(num.getValue(), power));
}

// squared()
template<typename l, typename t, typename m, typename I, typename T,
	 typename N, typename J, typename A>
constexpr Quantity<std::ratio_multiply<l, std::ratio<2>>, std::ratio_multiply<t, std::ratio<2>>,
                   std::ratio_multiply<m, std::ratio<2>>, std::ratio_multiply<I, std::ratio<2>>,
                   std::ratio_multiply<T, std::ratio<2>>, std::ratio_multiply<N, std::ratio<2>>,
                   std::ratio_multiply<J, std::ratio<2>>, std::ratio_multiply<A, std::ratio<2>>>
 squared(const Quantity<l, t, m, I, T, N, J, A>& num) {
    return Quantity<std::ratio_multiply<l, std::ratio<2>>, std::ratio_multiply<t, std::ratio<2>>, 
                    std::ratio_multiply<m, std::ratio<2>>, std::ratio_multiply<I, std::ratio<2>>,
                    std::ratio_multiply<T, std::ratio<2>>, std::ratio_multiply<N, std::ratio<2>>, 
                    std::ratio_multiply<J, std::ratio<2>>, std::ratio_multiply<A, std::ratio<2>>> 
                    (std::pow(num.getValue(), 2));
}

// Abbrevation for defining quantities and their squared values
#define QUANTITY_TYPE(_ldim, _tdim, _mdim, _Idim, _Tdim, _Ndim, _Jdim, _Adim, name) \
    typedef Quantity<std::ratio<_ldim>, std::ratio<_tdim>, std::ratio<_mdim>, \
		     std::ratio<_Idim>, std::ratio<_Tdim>, std::ratio<_Ndim>, \
		     std::ratio<_Jdim>, std::ratio<_Adim>> name; \
    typedef Quantity<std::ratio_multiply<std::ratio<_ldim>, std::ratio<2>>, std::ratio_multiply<std::ratio<_tdim>, std::ratio<2>>, \
                   std::ratio_multiply<std::ratio<_mdim>, std::ratio<2>>, std::ratio_multiply<std::ratio<_Idim>, std::ratio<2>>, \
                   std::ratio_multiply<std::ratio<_Tdim>, std::ratio<2>>, std::ratio_multiply<std::ratio<_Ndim>, std::ratio<2>>, \
                   std::ratio_multiply<std::ratio<_Jdim>, std::ratio<2>>, std::ratio_multiply<std::ratio<_Adim>, std::ratio<2>>> name ## Squared;

// Predefined (physical unit) quantity types:
// ------------------------------------------

// Replacement of "double" type
QUANTITY_TYPE(0, 0, 0, 0, 0, 0, 0, 0, QNumber);

// Basic physical quantity types
QUANTITY_TYPE(1, 0, 0, 0, 0, 0, 0, 0, QLength);
QUANTITY_TYPE(0, 1, 0, 0, 0, 0, 0, 0, QTime);
QUANTITY_TYPE(0, 0, 1, 0, 0, 0, 0, 0, QMass);
QUANTITY_TYPE(0, 0, 0, 1, 0, 0, 0, 0, QECurrent);
QUANTITY_TYPE(0, 0, 0, 0, 1, 0, 0, 0, QTemperature);
QUANTITY_TYPE(0, 0, 0, 0, 0, 1, 0, 0, QSubstance);
QUANTITY_TYPE(0, 0, 0, 0, 0, 0, 1, 0, QLIntensity);

// Angle type:
QUANTITY_TYPE(0, 0, 0, 0, 0, 0, 0, 1, QAngle);

// Predefined basic SI units:
constexpr QLength	metre(1.0);
constexpr QTime		second(1.0);
constexpr QMass		kilogram(1.0);
constexpr QECurrent	ampere(1.0);
constexpr QTemperature	kelvin(1.0);
constexpr QSubstance	mole(1.0);
constexpr QLIntensity	candela(1.0);
constexpr QAngle	radian(1.0);

// Math
constexpr double pi = 3.1415926535897932384626433832795;
constexpr double eulerGamma = 0.5772156649015328606065120900824;

inline double exp(const QNumber &num) {
    return std::exp(num.getValue());
}
inline double log(const QNumber &num) {
    return std::log(num.getValue());
}
inline double fabs(double num) {
    return std::fabs(num);
}

// Typesafe trigonometric operations
inline double sin(const QAngle &num) {
    return std::sin(num.getValue());
}
inline double cos(const QAngle &num) {
    return std::cos(num.getValue());
}
inline double tan(const QAngle &num) {
    return std::tan(num.getValue());
}
inline double cosh(const QNumber &num) {
    return std::cosh(num.getValue());
}
inline double tanh(const QNumber &num) {
    return std::tanh(num.getValue());
}
inline QAngle atan2(const QLength &lhs, const QLength &rhs) {
    return QAngle(std::atan2(lhs.getValue(), rhs.getValue()));
}

} // namespace hermes

#endif // HERMES_UNITSBASIC_H
