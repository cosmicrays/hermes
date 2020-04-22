#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "hermes/cosmicrays/CosmicRayDensity.h"
#include "hermes/cosmicrays/Dragon2D.h"
#include "hermes/cosmicrays/Dragon3D.h"
#include "hermes/cosmicrays/DummyCRDensity.h"
#include "hermes/cosmicrays/SimpleCRDensity.h"
#include "hermes/cosmicrays/Sun08CRDensity.h"
#include "hermes/cosmicrays/WMAP07CRDensity.h"

namespace py = pybind11;

namespace hermes {
namespace cosmicrays {

void init(py::module &m) {

	py::module subm = m.def_submodule("cosmicrays");
	subm.doc() = "cosmic rays package";

	// charged gas density models
	py::class_<CosmicRayDensity, std::shared_ptr<CosmicRayDensity>>(
	    subm, "CosmicRayDensity")
	    .def("getDensityPerEnergy", &CosmicRayDensity::getDensityPerEnergy)
	    .def("getEnergyAxis", &CosmicRayDensity::getEnergyAxis);
	py::class_<DummyCRDensity, std::shared_ptr<DummyCRDensity>,
		   CosmicRayDensity>(subm, "DummyCRDensity")
	    .def(py::init<>())
	    .def(py::init<const PID &, const QEnergy &, const QEnergy &, int>(),
		 py::arg("PID"), py::arg("E_min"), py::arg("E_max"),
		 py::arg("steps"));
	py::class_<SimpleCRDensity, std::shared_ptr<SimpleCRDensity>,
		   CosmicRayDensity>(subm, "SimpleCRDensity")
	    .def(py::init<>())
	    .def(py::init<const PID &, const QEnergy &, const QEnergy &, int>(),
		 py::arg("PID"), py::arg("E_min"), py::arg("E_max"),
		 py::arg("steps"));
	py::class_<Sun08CRDensity, std::shared_ptr<Sun08CRDensity>,
		   CosmicRayDensity>(subm, "Sun08CRDensity")
	    .def(py::init<>());
	py::class_<WMAP07CRDensity, std::shared_ptr<WMAP07CRDensity>,
		   CosmicRayDensity>(subm, "WMAP07CRDensity")
	    .def(py::init<>());
	py::class_<Dragon2D, std::shared_ptr<Dragon2D>, CosmicRayDensity>(
	    subm, "Dragon2D")
	    .def(py::init<const PID &>())
	    .def(py::init<const std::vector<PID> &>())
	    .def(py::init<const std::string, const std::vector<PID> &>())
	    .def("getDensityPerEnergy",
		 static_cast<QPDensityPerEnergy(Dragon2D::*)(
		     const QEnergy &, const Vector3QLength &) const>(&
		     Dragon2D::getDensityPerEnergy));
}

} // namespace cosmicrays
} // namespace hermes
