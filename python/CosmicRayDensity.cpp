#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "hermes/cosmicRayDensity/CosmicRayDensity.h"
#include "hermes/cosmicRayDensity/DummyCRDensity.h"
#include "hermes/cosmicRayDensity/SimpleCRDensity.h"
#include "hermes/cosmicRayDensity/Sun08CRDensity.h"
#include "hermes/cosmicRayDensity/WMAP07CRDensity.h"
#include "hermes/cosmicRayDensity/DragonCRDensity.h"

namespace py = pybind11;

namespace hermes {

void init_cosmic_ray_density(py::module &m) {
    
    // charged gas density models
    py::class_<CosmicRayDensity, std::shared_ptr<CosmicRayDensity>>(m, "CosmicRayDensity")
	      .def("getDensityPerEnergy", &CosmicRayDensity::getDensityPerEnergy)
	      .def("getEnergyAxis", &CosmicRayDensity::getEnergyAxis);
    py::class_<DummyCRDensity, std::shared_ptr<DummyCRDensity>, CosmicRayDensity>(m, "DummyCRDensity")
	      .def(py::init<>())
	      .def(py::init<const PID &, const QEnergy &, const QEnergy &, int>(),
				py::arg("PID"), py::arg("E_min"), py::arg("E_max"), py::arg("steps"));
    py::class_<SimpleCRDensity, std::shared_ptr<SimpleCRDensity>, CosmicRayDensity>(m, "SimpleCRDensity")
	      .def(py::init<>())
	      .def(py::init<const PID &, const QEnergy &, const QEnergy &, int>(),
				py::arg("PID"), py::arg("E_min"), py::arg("E_max"), py::arg("steps"));
    py::class_<Sun08CRDensity, std::shared_ptr<Sun08CRDensity>, CosmicRayDensity>(m, "Sun08CRDensity")
	      .def(py::init<>());
    py::class_<WMAP07CRDensity, std::shared_ptr<WMAP07CRDensity>, CosmicRayDensity>(m, "WMAP07CRDensity")
	      .def(py::init<>());
    py::class_<Dragon2DCRDensity, std::shared_ptr<Dragon2DCRDensity>, CosmicRayDensity>(m, "Dragon2DCRDensity")
	      .def(py::init<const PID &>())
	      .def(py::init<const std::vector<PID> &>())
	      .def(py::init<const std::string, const std::vector<PID> &>())
	      .def("getDensityPerEnergy", (QPDensityPerEnergy (Dragon2DCRDensity::*)
			      (const QEnergy &, const Vector3QLength &) const)
			      &Dragon2DCRDensity::getDensityPerEnergy);
	      
}

}
