#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "hermes/chargedGasDensity/ChargedGasDensity.h"
#include "hermes/chargedGasDensity/HII_Cordes91.h"
#include "hermes/chargedGasDensity/NE2001Simple.h"
#include "hermes/chargedGasDensity/YMW16.h"

#include "hermes/neutralGasDensity/RingModelDensity.h"

namespace py = pybind11;

namespace hermes {

void init_charged_gas_density(py::module &m) {

    // charged gas density models
    py::class_<ChargedGasDensity, std::shared_ptr<ChargedGasDensity>>(m, "ChargedGasDensity")
	      .def("getDensity", &ChargedGasDensity::getDensity);
    py::class_<HII_Cordes91, std::shared_ptr<HII_Cordes91>, ChargedGasDensity>(m, "HII_Cordes91")
	      .def(py::init<>())
	      .def("getDensity", &ChargedGasDensity::getDensity);
    py::class_<NE2001Simple, std::shared_ptr<NE2001Simple>, ChargedGasDensity>(m, "NE2001Simple")
	      .def(py::init<>())
	      .def("getDensity", &ChargedGasDensity::getDensity);
    py::class_<YMW16, std::shared_ptr<YMW16>, ChargedGasDensity>(m, "YMW16")
	      .def(py::init<>())
	      .def("getDensity", &ChargedGasDensity::getDensity);
}

}
