#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "hermes/chargedgas/ChargedGasDensity.h"
#include "hermes/chargedgas/HII_Cordes91.h"
#include "hermes/chargedgas/NE2001Simple.h"
#include "hermes/chargedgas/YMW16.h"

#include "hermes/neutralGasDensity/RingModelDensity.h"

namespace py = pybind11;

namespace hermes { namespace chargedgas {

void init(py::module &m) {

    py::module subm = m.def_submodule("chargedgas");
    subm.doc() = "charged gas package";

    // charged gas density models
    py::class_<ChargedGasDensity, std::shared_ptr<ChargedGasDensity>>(subm, "ChargedGasDensity")
	      .def("getDensity", &ChargedGasDensity::getDensity);
    py::class_<HII_Cordes91, std::shared_ptr<HII_Cordes91>, ChargedGasDensity>(subm, "HII_Cordes91")
	      .def(py::init<>())
	      .def("getDensity", &ChargedGasDensity::getDensity);
    py::class_<NE2001Simple, std::shared_ptr<NE2001Simple>, ChargedGasDensity>(subm, "NE2001Simple")
	      .def(py::init<>())
	      .def("getDensity", &ChargedGasDensity::getDensity);
    py::class_<YMW16, std::shared_ptr<YMW16>, ChargedGasDensity>(subm, "YMW16")
	      .def(py::init<>())
	      .def("getDensity",
			[](const YMW16 &d, const Vector3QLength &v) -> double { return static_cast<double>(d.getDensity(v)); });
}

} // namespace chargedgas
} // namespace hermes
