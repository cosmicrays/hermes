#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

#include <iostream>
#include "hermes/Units.h"

namespace py = pybind11;

namespace hermes {

template<typename Q>
Q toQuantity(py::object pyquantity) {
	py::object quantity = pyquantity.attr("decompose")();
	double value = (quantity.attr("value")).cast<double>();
	py::object unit = quantity.attr("unit");
	py::list bases = unit.attr("bases");
	py::list powers = unit.attr("powers");

	/*
	int index = 0;
	for(auto item : bases) {
		std::string u = (bases[index].attr("si").attr("to_string")()).cast<std::string>();
		double power = (powers[index]).cast<double>();
		std::cerr << u << "^" << power << std::endl;
		++index;
	}
	*/

	return static_cast<Q>(value);
}

    
template<typename Q>
void declare_quantity(py::module &m, std::string const & typestr, std::string const & suffix) {
    py::class_<Q>(m, typestr.c_str())
        .def(py::init<const double>())
	.def(py::self - py::self)
	.def(py::self + py::self)
	.def(py::self += py::self)
	.def(py::self -= py::self)
	.def(float() * py::self)
        .def(py::self * float())
	.def(py::self *= float())
	.def("__repr__",
		[suffix](const Q &q) {
	         return std::to_string(static_cast<double>(q)) + suffix.c_str(); })
	.def("__float__", [](const Q &q) { return static_cast<double>(q); });

    // coversion functions from astropy.units
    //m.def((std::string("to") + typestr).c_str(), template<> &toQuantity);
}

void init_units(py::module &m) {
    // |- basic
    declare_quantity<QNumber>(m, "QNumber", "_num");
    declare_quantity<QLength>(m, "QLength", "_m");
    declare_quantity<QTime>(m, "QTime", "_s");
    declare_quantity<QMass>(m, "QMass", "_kg");
    declare_quantity<QECurrent>(m, "QECurrent", "_A");
    declare_quantity<QTemperature>(m, "QTemperature", "_K");
    declare_quantity<QSubstance>(m, "QSubstance", "_mol");
    declare_quantity<QLIntensity>(m, "QLIntensity", "_cd");
    declare_quantity<QAngle>(m, "QAngle", "_rad");
    // |- derived
    declare_quantity<QArea>(m, "QArea", "_m2");
    declare_quantity<QVolume>(m, "QVolume", "_m3");
    declare_quantity<QForce>(m, "QForce", "_N");
    declare_quantity<QPressure>(m, "QPressure", "_Pa");
    declare_quantity<QEnergy>(m, "QEnergy", "_J");
    declare_quantity<QECharge>(m, "QECharge", "_C");
    declare_quantity<QMField>(m, "QMField", "_T");
    declare_quantity<QEPotential>(m, "QEPotential", "_V");
    declare_quantity<QFrequency>(m, "QFrequency", "_Hz");
    declare_quantity<QPower>(m, "QPower", "_W");
    declare_quantity<QEResistance>(m, "QEResistance", "_Ohm");
    declare_quantity<QECapacitance>(m, "QECapacitance", "_F");
    declare_quantity<QDispersionMeasure>(m, "QDispersionMeasure", "_m-2");
    declare_quantity<QRotationMeasure>(m, "QRotationMeasure", "_rad/m2");
    // |- direction
    py::class_<QDirection>(m, "QDirection")
	    .def(py::init<const QAngle &, const QAngle &>());

    // Angle
    m.attr("radian") = radian;

    // Length
    m.attr("metre") = metre;
    m.attr("centimetre") = centimetre;
    m.attr("parsec") = parsec;
    m.attr("kiloparsec") = kiloparsec;
    
    // Volume
    m.attr("metre3") = 1_m3;
    m.attr("centimetre3") = 1_cm3;

    // Energy
    m.attr("eV") = 1_eV;
    m.attr("GeV") = 1_GeV;
    m.attr("TeV") = 1_TeV;
}

}
