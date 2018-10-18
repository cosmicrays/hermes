#ifndef HERMES_UNITSLITERAS_H
#define HERMES_UNITSLITERALS_H

#include "UnitsDerived.h"

namespace hermes {

// Physical unit literals:
// -----------------------

constexpr QLength operator"" _m(long double x) { return static_cast<double>(x)*metre; }
constexpr QTime operator"" _s(long double x) { return QTime(x); };
constexpr QMass operator"" _kg(long double x) { return QMass(x); };

// Angular unit literals:
// ----------------------
constexpr long double operator"" _pi(long double x) 
    { return static_cast<double>(x) * pi; }
constexpr long double operator"" _pi(unsigned long long int x) 
    { return static_cast<double>(x) * pi; }

// Predefined angle units:
constexpr QAngle degree = static_cast<double>(2*pi / 360.0) * radian;

// literals for angle units
constexpr QAngle operator"" _rad(long double x) { return QAngle(x); };
constexpr QAngle operator"" _rad(unsigned long long int x) { return QAngle(static_cast<double>(x)); };
constexpr QAngle operator"" _deg(long double x) { return static_cast<double>(x)*degree; };
constexpr QAngle operator"" _deg(unsigned long long int x) { return static_cast<double>(x)*degree; };

// Conversion macro, which utilizes the string literals
#define ConvertTo(_x, _y) (_x).Convert(1.0_##_y)
#define ToSIValue(_x) (_x).getValue()

// literals for length units
constexpr QLength operator"" _mm(long double x) { return static_cast<double>(x)*millimetre; }
constexpr QLength operator"" _cm(long double x) { return static_cast<double>(x)*centimetre; }
constexpr QLength operator"" _km(long double x) { return static_cast<double>(x)*kilometre; }
constexpr QLength operator"" _mi(long double x) { return static_cast<double>(x)*mile; }
constexpr QLength operator"" _yd(long double x) { return static_cast<double>(x)*yard; }
constexpr QLength operator"" _ft(long double x) { return static_cast<double>(x)*foot; }
constexpr QLength operator"" _in(long double x) { return static_cast<double>(x)*inch; }
constexpr QLength operator"" _mm(unsigned long long int x) { return static_cast<double>(x)*millimetre; }
constexpr QLength operator"" _cm(unsigned long long int  x) { return static_cast<double>(x)*centimetre; }
constexpr QLength operator"" _m(unsigned long long int  x) { return static_cast<double>(x)*metre; }
constexpr QLength operator"" _km(unsigned long long int  x) { return static_cast<double>(x)*kilometre; }
constexpr QLength operator"" _mi(unsigned long long int  x) { return static_cast<double>(x)*mile; }
constexpr QLength operator"" _yd(unsigned long long int  x) { return static_cast<double>(x)*yard; }
constexpr QLength operator"" _ft(unsigned long long int  x) { return static_cast<double>(x)*foot; }
constexpr QLength operator"" _in(unsigned long long int  x) { return static_cast<double>(x)*inch; }

constexpr QLength operator"" _kpc(long double x) { return static_cast<double>(x)*kilo*parsec; }
constexpr QLength operator"" _kpc(unsigned long long int  x) { return static_cast<double>(x)*kilo*parsec; }

// literals for energy units
constexpr QEnergy operator"" _J(long double x) { return static_cast<double>(x) * joule; };

// literals for speed units
constexpr QSpeed operator"" _mps(long double x) { return QSpeed(x); };
constexpr QSpeed operator"" _miph(long double x) { return static_cast<double>(x)*mile / hour; };
constexpr QSpeed operator"" _kmph(long double x) { return static_cast<double>(x)*kilometre / hour; };
constexpr QSpeed operator"" _mps(unsigned long long int x) 
                                { return QSpeed(static_cast<long double>(x)); };
constexpr QSpeed operator"" _miph(unsigned long long int x) 
                                 { return static_cast<double>(x)*mile / hour; };
constexpr QSpeed operator"" _kmph(unsigned long long int x) 
                                 { return static_cast<double>(x)*kilometre / hour; };

// literal for frequency unit
constexpr QFrequency operator"" _Hz(long double x) { return QFrequency(x); };
constexpr QFrequency operator"" _Hz(unsigned long long int x) 
                                   { return QFrequency(static_cast<long double>(x)); };

// magnetic field strength
constexpr QMField operator"" _muG(long double x) { return static_cast<double>(x)*micro*gauss; }
constexpr QMField operator"" _muG(unsigned long long int  x) { return static_cast<double>(x)*micro*gauss; }

// literals for time units
constexpr QTime operator"" _min(long double x) { return static_cast<double>(x)*minute; };
constexpr QTime operator"" _h(long double x) { return static_cast<double>(x)*hour; };
constexpr QTime operator"" _day(long double x) { return static_cast<double>(x)*day; };
constexpr QTime operator"" _s(unsigned long long int x) { return QTime(static_cast<double>(x)); };
constexpr QTime operator"" _min(unsigned long long int x) { return static_cast<double>(x)*minute; };
constexpr QTime operator"" _h(unsigned long long int x) { return static_cast<double>(x)*hour; };
constexpr QTime operator"" _day(unsigned long long int x) { return static_cast<double>(x)*day; };

// literals for mass units
constexpr QMass operator"" _g(long double x) { return static_cast<double>(x)*gramme; };
constexpr QMass operator"" _t(long double x) { return static_cast<double>(x)*tonne; };
constexpr QMass operator"" _oz(long double x) { return static_cast<double>(x)*ounce; };
constexpr QMass operator"" _lb(long double x) { return static_cast<double>(x)*pound; };
constexpr QMass operator"" _st(long double x) { return static_cast<double>(x)*stone; };
constexpr QMass operator"" _kg(unsigned long long int x) { return QMass(static_cast<double>(x)); };
constexpr QMass operator"" _g(unsigned long long int x) { return static_cast<double>(x)*gramme; };
constexpr QMass operator"" _t(unsigned long long int x) { return static_cast<double>(x)*tonne; };
constexpr QMass operator"" _oz(unsigned long long int x) { return static_cast<double>(x)*ounce; };
constexpr QMass operator"" _lb(unsigned long long int x) { return static_cast<double>(x)*pound; };
constexpr QMass operator"" _st(unsigned long long int x) { return static_cast<double>(x)*stone; };

// literals for acceleration units
constexpr QAcceleration operator"" _mps2(long double x) { return QAcceleration(x); };
constexpr QAcceleration operator"" _mps2(unsigned long long int x) 
                                        { return QAcceleration(static_cast<double>(x)); };
constexpr QAcceleration operator"" _G(long double x) { return static_cast<double>(x)*G; };
constexpr QAcceleration operator"" _G(unsigned long long int x) { return static_cast<double>(x)*G; }

// literals for force units
constexpr QForce operator"" _N(long double x) { return QForce(x); };
constexpr QForce operator"" _N(unsigned long long int x) { return QForce(static_cast<double>(x)); };
constexpr QForce operator"" _lbf(long double x) { return static_cast<double>(x)*poundforce; };
constexpr QForce operator"" _lbf(unsigned long long int x) { return static_cast<double>(x)*poundforce; };
constexpr QForce operator"" _kp(long double x) { return static_cast<double>(x)*kilopond; };
constexpr QForce operator"" _kp(unsigned long long int x) { return static_cast<double>(x)*kilopond; };

// literals for pressure units
constexpr QPressure operator"" _Pa(long double x) { return QPressure(x); };
constexpr QPressure operator"" _Pa(unsigned long long int x) 
                                  { return QPressure(static_cast<double>(x)); };
constexpr QPressure operator"" _bar(long double x) { return static_cast<double>(x)*bar; };
constexpr QPressure operator"" _bar(unsigned long long int x) { return static_cast<double>(x)*bar; };
constexpr QPressure operator"" _psi(long double x) { return static_cast<double>(x)*psi; };
constexpr QPressure operator"" _psi(unsigned long long int x) { return static_cast<double>(x)*psi; };

} // namespace hermes

#endif // HERMES_UNITSLITERALS_H
