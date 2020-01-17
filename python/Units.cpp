#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

#include "hermes/Units.h"

namespace py = pybind11;

namespace hermes {
    
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
	            return std::to_string(q.getValue()) + suffix.c_str(); })
		.def("__float__", [](const Q &q) { return q.getValue(); });
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
    declare_quantity<QRotationMeasure>(m, "QRotationMeasure,", "_rad/m2");
}

}
