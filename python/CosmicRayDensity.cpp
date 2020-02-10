#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "hermes/cosmicRayDensity/CosmicRayDensity.h"
#include "hermes/cosmicRayDensity/SimpleCRDensity.h"
#include "hermes/cosmicRayDensity/Sun08CRDensity.h"
#include "hermes/cosmicRayDensity/WMAP07CRDensity.h"
#include "hermes/cosmicRayDensity/DragonCRDensity.h"

namespace py = pybind11;

namespace hermes {

void init_cosmic_ray_density(py::module &m) {

    // charged gas density models
    py::class_<CosmicRayDensity, std::shared_ptr<CosmicRayDensity>>(m, "CosmicRayDensity")
	      .def("getDensityPerEnergy", &CosmicRayDensity::getDensityPerEnergy);
    py::class_<SimpleCRDensity, std::shared_ptr<SimpleCRDensity>, CosmicRayDensity>(m, "SimpleCRDensity")
	      .def(py::init<>())
	      .def("getDensityPerEnergy", &SimpleCRDensity::getDensityPerEnergy);
    py::class_<Dragon2DCRDensity, std::shared_ptr<Dragon2DCRDensity>, CosmicRayDensity>(m, "Dragon2DCRDensity")
	      .def(py::init<>());
	      //.def("getDensityPerEnergy", &Dragon2DCRDensity::getDensityPerEnergy);
}

}
