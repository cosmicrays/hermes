#include <pybind11/pybind11.h>

#include "hermes/Units.h"

#define PY_LONG_PREFIXED_UNITS(_NAME, _UNIT)                                   \
	subm.attr(_NAME) = _UNIT;                                              \
	subm.attr("yocto" _NAME) = yocto * (_UNIT);                            \
	subm.attr("zepto" _NAME) = zepto * (_UNIT);                            \
	subm.attr("atto" _NAME) = atto * (_UNIT);                              \
	subm.attr("femto" _NAME) = femto * (_UNIT);                            \
	subm.attr("pico" _NAME) = pico * (_UNIT);                              \
	subm.attr("nano" _NAME) = nano * (_UNIT);                              \
	subm.attr("micro" _NAME) = micro * (_UNIT);                            \
	subm.attr("milli" _NAME) = milli * (_UNIT);                            \
	subm.attr("centi" _NAME) = centi * (_UNIT);                            \
	subm.attr("deci" _NAME) = deci * (_UNIT);                              \
	subm.attr("hecto" _NAME) = hecto * (_UNIT);                            \
	subm.attr("kilo" _NAME) = kilo * (_UNIT);                              \
	subm.attr("mega" _NAME) = mega * (_UNIT);                              \
	subm.attr("giga" _NAME) = giga * (_UNIT);                              \
	subm.attr("tera" _NAME) = tera * (_UNIT);                              \
	subm.attr("peta" _NAME) = peta * (_UNIT);                              \
	subm.attr("exa" _NAME) = exa * (_UNIT);                                \
	subm.attr("zetta" _NAME) = zetta * (_UNIT);                            \
	subm.attr("yotta" _NAME) = yotta * (_UNIT);

#define PY_SHORT_PREFIXED_UNITS(_NAME, _UNIT)                                  \
	subm.attr(_NAME) = _UNIT;                                              \
	subm.attr("y" _NAME) = yocto * (_UNIT);                                \
	subm.attr("z" _NAME) = zepto * (_UNIT);                                \
	subm.attr("a" _NAME) = atto * (_UNIT);                                 \
	subm.attr("f" _NAME) = femto * (_UNIT);                                \
	subm.attr("p" _NAME) = pico * (_UNIT);                                 \
	subm.attr("n" _NAME) = nano * (_UNIT);                                 \
	subm.attr("μ" _NAME) = micro * (_UNIT);                                \
	subm.attr("m" _NAME) = milli * (_UNIT);                                \
	subm.attr("c" _NAME) = centi * (_UNIT);                                \
	subm.attr("d" _NAME) = deci * (_UNIT);                                 \
	subm.attr("h" _NAME) = hecto * (_UNIT);                                \
	subm.attr("k" _NAME) = kilo * (_UNIT);                                 \
	subm.attr("M" _NAME) = mega * (_UNIT);                                 \
	subm.attr("G" _NAME) = giga * (_UNIT);                                 \
	subm.attr("T" _NAME) = tera * (_UNIT);                                 \
	subm.attr("P" _NAME) = peta * (_UNIT);                                 \
	subm.attr("E" _NAME) = exa * (_UNIT);                                  \
	subm.attr("Z" _NAME) = zetta * (_UNIT);                                \
	subm.attr("Y" _NAME) = yotta * (_UNIT);

namespace py = pybind11;

namespace hermes {
namespace units {

void init(py::module &m) {

	py::module subm = m.def_submodule("units");
	subm.doc() = "units package";

	// Angle
	subm.attr("radian") = radian;
	subm.attr("rad") = 1_rad;
	subm.attr("deg") = 1_deg;
	subm.def("radian2degree", &radian2degree);
	subm.def("degree2radian", &degree2radian);

	// Length
	PY_LONG_PREFIXED_UNITS("metre", metre);
	PY_SHORT_PREFIXED_UNITS("m", metre);
	PY_LONG_PREFIXED_UNITS("parsec", parsec);
	PY_SHORT_PREFIXED_UNITS("pc", parsec);

	// Area
	PY_LONG_PREFIXED_UNITS("barn", barn);
	PY_SHORT_PREFIXED_UNITS("barn", barn);

	// Time
	subm.attr("minute") = minute;
	subm.attr("hour") = hour;
	subm.attr("day") = day;
	subm.attr("week") = week;
	PY_LONG_PREFIXED_UNITS("year", year);
	PY_SHORT_PREFIXED_UNITS("yr", year);

	// Frequency
	PY_LONG_PREFIXED_UNITS("hertz", hertz);
	PY_SHORT_PREFIXED_UNITS("Hz", hertz);

	// Volume
	subm.attr("metre3") = 1_m3;
	subm.attr("m3") = 1_m3;
	subm.attr("centimetre3") = 1_cm3;
	subm.attr("cm3") = 1_cm3;

	// Energy
	PY_LONG_PREFIXED_UNITS("electronvolt", electronvolt);
	PY_SHORT_PREFIXED_UNITS("eV", electronvolt);
	subm.attr("joule") = 1_J;
	subm.attr("erg") = 1_erg;

	// Temperature
	PY_LONG_PREFIXED_UNITS("kelvin", kelvin);
	PY_SHORT_PREFIXED_UNITS("K", kelvin);

	// Mass
	PY_LONG_PREFIXED_UNITS("gram", gram);
	PY_SHORT_PREFIXED_UNITS("g", gram);

	// Magnetic field
	PY_LONG_PREFIXED_UNITS("gauss", gauss);
	PY_SHORT_PREFIXED_UNITS("G", gauss);

	// Constants
	subm.attr("e_plus") = e_plus;
	subm.attr("c_light") = c_light;
	subm.attr("amu") = amu;
	subm.attr("m_proton") = m_proton;
	subm.attr("m_neutron") = m_neutron;
	subm.attr("m_electron") = m_electron;
	subm.attr("h_planck") = h_planck;
	subm.attr("k_boltzmann") = k_boltzmann;
	// subm.attr("mu0")		= mu0;
	// subm.attr("epsilon0")	= epsilon0;
}

} // namespace units
} // namespace hermes
