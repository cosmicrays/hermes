#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "hermes/interactions/DifferentialCrossSection.h"
#include "hermes/interactions/KleinNishina.h"

namespace py = pybind11;

namespace hermes {

void init_interactions(py::module &m) {

    // charged gas density models
    py::class_<DifferentialCrossSection, std::shared_ptr<DifferentialCrossSection>>(m, "DifferentialCrossSection");
	      //.def("getEnergyDensity", &PhotonField::getEnergyDensity);
    py::class_<KleinNishina, std::shared_ptr<KleinNishina>, DifferentialCrossSection>(m, "KleinNishina")
	      .def(py::init<>());
	      //.def("getEnergyDensity", &CMB::getEnergyDensity);
}

}
