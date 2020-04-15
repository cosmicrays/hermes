#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "hermes/neutralgas/NeutralGasDensity.h"
#include "hermes/neutralgas/RingModel.h"

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
    py::class_<RingModel, std::shared_ptr<RingModel>, NeutralGasDensity>(subm, "RingModel")
	      .def(py::init<RingType>(), py::arg("ring_type"))
	      .def("getEnabledRings", &RingModel::getEnabledRings)
	      .def("setEnabledRings", &RingModel::setEnabledRings)
	      .def("disableRingNo", &RingModel::disableRingNo)
	      .def("enableRingNo", &RingModel::enableRingNo)
	      .def("isRingEnabled", &RingModel::isRingEnabled)
	      .def("getRingType", &RingModel::getRingType)
	      .def("getRingNumber", &RingModel::getRingNumber);
}

} // namespace neutralgas
} // namespace hermes
