#include <pybind11/pybind11.h>

#include <iostream>
#include <sstream>

#include "hermes/Units.h"

namespace py = pybind11;

namespace hermes {
	
struct AstroPy {
	py::object q;
	bool enabled = false;
};

template<typename Q>
void declare_quantity(py::module &m, std::string const & typestr, std::string const & suffix, AstroPy &apyu) {
    py::class_<Q>(m, typestr.c_str())
        .def(py::init<const double>())
	.def("__lt__",  [](const Q &self, const Q &other) -> bool { return self < other; }, py::is_operator()) 
	.def("__le__",  [](const Q &self, const Q &other) -> bool { return self <= other; }, py::is_operator()) 
	.def("__eq__",  [](const Q &self, const Q &other) -> bool { return self == other; }, py::is_operator())
	.def("__ne__",  [](const Q &self, const Q &other) -> bool { return self != other; }, py::is_operator())
	.def("__ge__",  [](const Q &self, const Q &other) -> bool { return self >= other; }, py::is_operator()) 
	.def("__gt__",  [](const Q &self, const Q &other) -> bool { return self > other; }, py::is_operator()) 
	.def("__neg__",  [](const Q &self) -> Q { return -1*self; }, py::is_operator())
	.def("__add__",  [](const Q &self, const Q &other) -> Q { return self + other; }, py::is_operator())
	.def("__sub__",  [](const Q &self, const Q &other) -> Q { return self - other; }, py::is_operator())
	.def("__truediv__",  [](const Q &self, const Q &other) -> double { return static_cast<double>(self / other); }, py::is_operator())
	.def("__mul__",  [](const Q &self, const int a) -> Q { return a * self; }, py::is_operator())
	.def("__mul__",  [](const Q &self, const double a) -> Q { return a * self; }, py::is_operator())
	.def("__rmul__",  [](const Q &self, const int a) -> Q { return a * self; }, py::is_operator())
	.def("__rmul__",  [](const Q &self, const double a) -> Q { return a * self; }, py::is_operator())
	.def("__truediv__",  [](const Q &self, const int a) -> Q { return self / a; }, py::is_operator())
	.def("__truediv__",  [](const Q &self, const double a) -> Q { return self / a; }, py::is_operator())
	.def("__abs__",  [](const Q &self) -> Q { return fabs(self); }, py::is_operator())
	.def("__repr__",
		[suffix](const Q &q) {
  			std::ostringstream streamBuffer;
			streamBuffer << std::scientific << static_cast<double>(q);
	         return streamBuffer.str() + " " + suffix.c_str(); })
	.def("__float__", [](const Q &q) { return static_cast<double>(q); })
	.def("toAstroPy",
		[apyu, suffix](const Q &q) {
			if (apyu.enabled)
				return apyu.q(static_cast<double>(q), suffix);
			throw std::runtime_error("Error: cannot use .toApy() without having astropy package present."); });
}

void init_quantity(py::module &m) {

	AstroPy apyu;	
	try {
		py::object astropyunits = py::module::import("astropy.units");
		apyu.q = astropyunits.attr("Quantity");
		apyu.enabled = true;
	} catch (std::exception e) {
		std::cerr << "No astropy.units package found. For advanced units features, please install astropy." << std::endl;
		apyu.enabled = false;
	}

	// |- basic
	declare_quantity<QNumber>(m, "QNumber", "num", apyu);
	declare_quantity<QLength>(m, "QLength", "m", apyu);
	declare_quantity<QTime>(m, "QTime", "s", apyu);
	declare_quantity<QMass>(m, "QMass", "kg", apyu);
	declare_quantity<QECurrent>(m, "QECurrent", "A", apyu);
	declare_quantity<QTemperature>(m, "QTemperature", "K", apyu);
	declare_quantity<QSubstance>(m, "QSubstance", "mol", apyu);
	declare_quantity<QLIntensity>(m, "QLIntensity", "cd", apyu);
	declare_quantity<QAngle>(m, "QAngle", "rad", apyu);
	declare_quantity<QSolidAngle>(m, "QSolidAngle", "sr", apyu);

	// |- derived
	declare_quantity<QAngularMomentum>(m, "QAngularMomentum", "kg m^2 s^-1", apyu);
	declare_quantity<QArea>(m, "QArea", "m^2", apyu);
	declare_quantity<QDifferentialCrossSection>(m, "QDiffCrossSection", "m^2 J^-1", apyu);
	declare_quantity<QDifferentialFlux>(m, "QDifferentialFlux", "J^-1 m^-2 s^-1", apyu);
	declare_quantity<QDifferentialIntensity>(m, "QDifferentialIntensity", "J^-1 m^-2 s^-1 sr^-1", apyu);
	declare_quantity<QDispersionMeasure>(m, "QDispersionMeasure", "m^-2", apyu);
	declare_quantity<QECapacitance>(m, "QECapacitance", "F", apyu);
	declare_quantity<QECharge>(m, "QECharge", "C", apyu);
	declare_quantity<QEPotential>(m, "QEPotential", "V", apyu);
	declare_quantity<QEResistance>(m, "QEResistance", "Ohm", apyu);
	declare_quantity<QEnergy>(m, "QEnergy", "J", apyu);
	declare_quantity<QEnergyDensity>(m, "QEnergyDensity", "J m^-3", apyu);
	declare_quantity<QEnergyOverTemperature>(m, "QEnergyOverTemperature", "J K^-1", apyu);
	declare_quantity<QForce>(m, "QForce", "N", apyu);
	declare_quantity<QFrequency>(m, "QFrequency", "Hz", apyu);
	declare_quantity<QMField>(m, "QMField", "T", apyu);
	declare_quantity<QPDensityPerEnergy>(m, "QPDensityPerEnergy", "m^-3 J^-1", apyu);
	declare_quantity<QPower>(m, "QPower", "W", apyu);
	//declare_quantity<QPressure>(m, "QPressure", "Pa", apyu);
	declare_quantity<QRotationMeasure>(m, "QRotationMeasure", "rad m^-2", apyu);
	declare_quantity<QSpeed>(m, "QSpeed", "m s^-1", apyu);
	declare_quantity<QVolume>(m, "QVolume", "m^3", apyu);
	
	// |- direction
	py::class_<QDirection>(m, "QDirection")
		.def(py::init<const QAngle &, const QAngle &>());

	// |- unnamed
	declare_quantity<QICOuterIntegral>(m, "QICOuterIntegral", "m^-5 kg^-1 s", apyu);
	declare_quantity<QICInnerIntegral>(m, "QICInnerIntegral", "m^-3 kg^-1 s^2", apyu);
}

}
