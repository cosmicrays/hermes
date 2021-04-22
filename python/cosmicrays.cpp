#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "hermes/cosmicrays/CosmicRayDensity.h"
#include "hermes/cosmicrays/Dragon2D.h"
#include "hermes/cosmicrays/Dragon3D.h"
#include "hermes/cosmicrays/DummyCR.h"
#include "hermes/cosmicrays/SimpleCR.h"
#include "hermes/cosmicrays/Sun08.h"
#include "hermes/cosmicrays/WMAP07.h"

namespace py = pybind11;

namespace hermes { namespace cosmicrays {

void init(py::module &m) {
	py::module subm = m.def_submodule("cosmicrays");
	subm.doc() = "cosmic rays package";

	// ionized gas density models
	py::class_<CosmicRayDensity, std::shared_ptr<CosmicRayDensity>>(
	    subm, "CosmicRayDensity")
	    .def("getDensityPerEnergy", &CosmicRayDensity::getDensityPerEnergy)
	    .def("getEnergyAxis", &CosmicRayDensity::getEnergyAxis);
	py::class_<DummyCR, std::shared_ptr<DummyCR>,
	           CosmicRayDensity>(subm, "DummyCR")
	    .def(py::init<>())
	    .def(py::init<const PID &, const QEnergy &, const QEnergy &, int>(),
	         py::arg("PID"), py::arg("E_min"), py::arg("E_max"),
	         py::arg("steps"));
	py::class_<SimpleCR, std::shared_ptr<SimpleCR>,
	           CosmicRayDensity>(subm, "SimpleCR")
	    .def(py::init<>())
	    .def(py::init<const PID &, const QEnergy &, const QEnergy &, int>(),
	         py::arg("PID"), py::arg("E_min"), py::arg("E_max"),
	         py::arg("steps"));
	py::class_<Sun08, std::shared_ptr<Sun08>,
	           CosmicRayDensity>(subm, "Sun08")
	    .def(py::init<>());
	py::class_<WMAP07, std::shared_ptr<WMAP07>,
	           CosmicRayDensity>(subm, "WMAP07")
	    .def(py::init<>());
	py::class_<Dragon2D, std::shared_ptr<Dragon2D>, CosmicRayDensity>(
	    subm, "Dragon2D")
	    .def(py::init<const PID &>())
	    .def(py::init<const std::vector<PID> &>())
	    .def(py::init<const std::string, const std::vector<PID> &>())
	    .def("getDensityPerEnergy",
	         static_cast<QPDensityPerEnergy (Dragon2D::*)(
	             const QEnergy &, const Vector3QLength &) const>(
	             &Dragon2D::getDensityPerEnergy));
}

}}  // namespace hermes::cosmicrays
