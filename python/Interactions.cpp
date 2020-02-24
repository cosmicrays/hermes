#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "hermes/interactions/DifferentialCrossSection.h"
#include "hermes/interactions/KleinNishina.h"
#include "hermes/interactions/BremsstrahlungSimple.h"
#include "hermes/interactions/Kamae06.h"

namespace py = pybind11;

namespace hermes {

void init_interactions(py::module &m) {

    // charged gas density models
    py::class_<DifferentialCrossSection, std::shared_ptr<DifferentialCrossSection>>(m, "DifferentialCrossSection");
    py::class_<KleinNishina, std::shared_ptr<KleinNishina>, DifferentialCrossSection>(m, "KleinNishina")
	      .def(py::init<>())
	      .def("getDiffCrossSection",
		   (QDifferentialCrossSection (KleinNishina::*)(const QEnergy &, const QEnergy &, const QEnergy &) const)
		   &KleinNishina::getDiffCrossSection);
    py::class_<BremsstrahlungSimple, std::shared_ptr<BremsstrahlungSimple>, DifferentialCrossSection>(m, "BremsstrahlungSimple")
	      .def(py::init<>())
	      .def("getDiffCrossSection",
		   (QDifferentialCrossSection (BremsstrahlungSimple::*)(const QEnergy &, const QEnergy &, const QEnergy &) const)
		   &BremsstrahlungSimple::getDiffCrossSection);
    py::class_<Kamae06, std::shared_ptr<Kamae06>, DifferentialCrossSection>(m, "Kamae06")
	      .def(py::init<>());
}

}
