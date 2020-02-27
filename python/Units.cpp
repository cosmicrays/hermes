#include <pybind11/pybind11.h>

#include "hermes/Units.h"

#define PY_LONG_PREFIXED_UNITS(_NAME, _UNIT) \
	u.attr(_NAME) = _UNIT; \
	u.attr("yocto" _NAME) = yocto * _UNIT; \
	u.attr("zepto" _NAME) = zepto * _UNIT; \
	u.attr("atto" _NAME)  = atto  * _UNIT; \
	u.attr("femto" _NAME) = femto * _UNIT; \
	u.attr("pico" _NAME)  = pico  * _UNIT; \
	u.attr("nano" _NAME)  = nano  * _UNIT; \
	u.attr("micro" _NAME) = micro * _UNIT; \
	u.attr("milli" _NAME) = milli * _UNIT; \
	u.attr("centi" _NAME) = centi * _UNIT; \
	u.attr("deci" _NAME)  = deci  * _UNIT; \
	u.attr("hecto" _NAME) = hecto * _UNIT; \
	u.attr("kilo" _NAME)  = kilo  * _UNIT; \
	u.attr("mega" _NAME)  = mega  * _UNIT; \
	u.attr("giga" _NAME)  = giga  * _UNIT; \
	u.attr("tera" _NAME)  = tera  * _UNIT; \
	u.attr("peta" _NAME)  = peta  * _UNIT; \
	u.attr("exa" _NAME)   = exa   * _UNIT; \
	u.attr("zetta" _NAME) = zetta * _UNIT; \
	u.attr("yotta" _NAME) = yotta * _UNIT;

#define PY_SHORT_PREFIXED_UNITS(_NAME, _UNIT) \
	u.attr(_NAME) = _UNIT; \
	u.attr("y" _NAME) = yocto * _UNIT; \
	u.attr("z" _NAME) = zepto * _UNIT; \
	u.attr("a" _NAME) = atto  * _UNIT; \
	u.attr("f" _NAME) = femto * _UNIT; \
	u.attr("p" _NAME) = pico  * _UNIT; \
	u.attr("n" _NAME) = nano  * _UNIT; \
	u.attr("μ" _NAME) = micro * _UNIT; \
	u.attr("m" _NAME) = milli * _UNIT; \
	u.attr("c" _NAME) = centi * _UNIT; \
	u.attr("d" _NAME) = deci  * _UNIT; \
	u.attr("h" _NAME) = hecto * _UNIT; \
	u.attr("k" _NAME) = kilo  * _UNIT; \
	u.attr("M" _NAME) = mega  * _UNIT; \
	u.attr("G" _NAME) = giga  * _UNIT; \
	u.attr("T" _NAME) = tera  * _UNIT; \
	u.attr("P" _NAME) = peta  * _UNIT; \
	u.attr("E" _NAME) = exa   * _UNIT; \
	u.attr("Z" _NAME) = zetta * _UNIT; \
	u.attr("Y" _NAME) = yotta * _UNIT;

namespace py = pybind11;

namespace hermes {

void init_units(py::module &m) {

	py::module u = m.def_submodule("units");
	u.doc() = "HERMES units";

	// Angle
	u.attr("radian") = radian;
	u.attr("rad") = 1_rad;
	u.attr("deg") = 1_deg;
	u.def("radian2degree", &radian2degree);
	u.def("degree2radian", &degree2radian);

	// Length
	PY_LONG_PREFIXED_UNITS("metre", metre);
	PY_SHORT_PREFIXED_UNITS("m", metre);
	PY_LONG_PREFIXED_UNITS("parsec", parsec);
	PY_SHORT_PREFIXED_UNITS("pc", parsec);

	// Time
	u.attr("minute") = minute;
	u.attr("hour")   = hour;
	u.attr("day")    = day;
	u.attr("week")   = week;
	PY_LONG_PREFIXED_UNITS("year", year);
	PY_SHORT_PREFIXED_UNITS("yr", year);

	// Frequency
	PY_LONG_PREFIXED_UNITS("hertz", hertz);
	PY_SHORT_PREFIXED_UNITS("Hz", hertz);

	// Volume
	u.attr("metre3") = 1_m3;
	u.attr("m3") = 1_m3;
	u.attr("centimetre3") = 1_cm3;
	u.attr("cm3") = 1_cm3;

	// Energy
	PY_LONG_PREFIXED_UNITS("electronvolt", electronvolt);
	PY_SHORT_PREFIXED_UNITS("eV", electronvolt);
	u.attr("joule") = 1_J;
	u.attr("erg") = 1_erg;
	
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
	u.attr("e_plus")	= e_plus;
	u.attr("c_light")	= c_light;
	u.attr("amu")		= amu;
	u.attr("m_proton")	= m_proton;
	u.attr("m_neutron")	= m_neutron;
	u.attr("m_electron")	= m_electron;
	u.attr("h_planck")	= h_planck;
	u.attr("k_boltzmann")	= k_boltzmann;
	//u.attr("mu0")		= mu0;
	//u.attr("epsilon0")	= epsilon0;
}

}
