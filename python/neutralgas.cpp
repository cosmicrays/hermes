#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "hermes/neutralgas/NeutralGasDensity.h"
#include "hermes/neutralgas/RingModelDensity.h"

namespace py = pybind11;

namespace hermes { namespace neutralgas {

void init(py::module &m) {
    
    py::module subm = m.def_submodule("neutralgas");
    subm.doc() = "neutral gas package";

    py::enum_<RingType>(subm, "RingType", py::arithmetic())
	    .value("HI", RingType::HI)
	    .value("CO", RingType::CO);

    // neutral gas density models
    py::class_<NeutralGasDensity, std::shared_ptr<NeutralGasDensity>>(subm, "NeutralGasDensity");
    py::class_<RingModelDensity, std::shared_ptr<RingModelDensity>, NeutralGasDensity>(subm, "RingModelDensity")
	      .def(py::init<RingType>(), py::arg("ring_type"))
	      .def("getEnabledRings", &RingModelDensity::getEnabledRings)
	      .def("setEnabledRings", &RingModelDensity::setEnabledRings)
	      .def("disableRingNo", &RingModelDensity::disableRingNo)
	      .def("enableRingNo", &RingModelDensity::enableRingNo)
	      .def("isRingEnabled", &RingModelDensity::isRingEnabled)
	      .def("getRingType", &RingModelDensity::getRingType)
	      .def("getRingNumber", &RingModelDensity::getRingNumber);
}

} // namespace neutralgas
} // namespace hermes
