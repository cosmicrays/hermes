#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "hermes/neutralGasDensity/NeutralGasDensity.h"
#include "hermes/neutralGasDensity/RingModelDensity.h"

namespace py = pybind11;

namespace hermes {

void init_neutral_gas_density(py::module &m) {

    py::enum_<RingType>(m, "RingType", py::arithmetic())
	    .value("HI", RingType::HI)
	    .value("CO", RingType::CO);

    // neutral gas density models
    py::class_<NeutralGasDensity, std::shared_ptr<NeutralGasDensity>>(m, "NeutralGasDensity");
    py::class_<RingModelDensity, std::shared_ptr<RingModelDensity>, NeutralGasDensity>(m, "RingModelDensity")
	      .def(py::init<RingType>(), py::arg("ring_type"));
}

}
