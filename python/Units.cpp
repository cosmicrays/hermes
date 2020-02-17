#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

#include <iostream>
#include <sstream>
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
  			std::ostringstream streamBuffer;
			streamBuffer << static_cast<double>(q);
	         return streamBuffer.str() + " " + suffix.c_str(); })
	.def("__float__", [](const Q &q) { return static_cast<double>(q); });

    // coversion functions from astropy.units
    //m.def((std::string("to") + typestr).c_str(), template<> &toQuantity);
}

void init_units(py::module &m) {
    // |- basic
    declare_quantity<QNumber>(m, "QNumber", "num");
    declare_quantity<QLength>(m, "QLength", "m");
    declare_quantity<QTime>(m, "QTime", "s");
    declare_quantity<QMass>(m, "QMass", "kg");
    declare_quantity<QECurrent>(m, "QECurrent", "A");
    declare_quantity<QTemperature>(m, "QTemperature", "K");
    declare_quantity<QSubstance>(m, "QSubstance", "mol");
    declare_quantity<QLIntensity>(m, "QLIntensity", "cd");
    declare_quantity<QAngle>(m, "QAngle", "rad");
    // |- derived
    declare_quantity<QArea>(m, "QArea", "m^2");
    declare_quantity<QVolume>(m, "QVolume", "m^3");
    declare_quantity<QForce>(m, "QForce", "N");
    declare_quantity<QPressure>(m, "QPressure", "Pa");
    declare_quantity<QEnergy>(m, "QEnergy", "J");
    declare_quantity<QECharge>(m, "QECharge", "C");
    declare_quantity<QMField>(m, "QMField", "T");
    declare_quantity<QEPotential>(m, "QEPotential", "V");
    declare_quantity<QFrequency>(m, "QFrequency", "Hz");
    declare_quantity<QPower>(m, "QPower", "_W");
    declare_quantity<QEResistance>(m, "QEResistance", "Ohm");
    declare_quantity<QECapacitance>(m, "QECapacitance", "F");
    declare_quantity<QDispersionMeasure>(m, "QDispersionMeasure", "m^-2");
    declare_quantity<QRotationMeasure>(m, "QRotationMeasure", "rad/m^2");
    declare_quantity<QDiffCrossSection>(m, "QDiffCrossSection", "m^2/J");
    // |- direction
    py::class_<QDirection>(m, "QDirection")
	    .def(py::init<const QAngle &, const QAngle &>());

    // Angle
    m.attr("radian") = radian;
    m.attr("rad") = 1_rad;
    m.attr("deg") = 1_deg;
    m.def("radian2degree", &radian2degree);
    m.def("degree2radian", &degree2radian);

    // Length
    m.attr("metre") = metre;
    m.attr("centimetre") = centimetre;
    m.attr("parsec") = parsec;
    m.attr("kiloparsec") = kiloparsec;
    
    // Volume
    m.attr("metre3") = 1_m3;
    m.attr("centimetre3") = 1_cm3;

    // Energy
    m.attr("joul") = 1_J;
    m.attr("eV") = 1_eV;
    m.attr("eV") = 1_erg;
    m.attr("keV") = 1_keV;
    m.attr("Mev") = 1_MeV;
    m.attr("GeV") = 1_GeV;
    m.attr("TeV") = 1_TeV;
    m.attr("PeV") = 1_PeV;
}

}
