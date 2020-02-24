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
    
    // PID
    py::class_<PID, std::shared_ptr<PID>>(m, "PID")
	    .def(py::init<int, int>());
    m.attr("Electron") = PID(-1,0);    
    m.attr("Positron") = PID( 1,0);    
    m.attr("Proton")   = PID( 1,1);    
    m.attr("Helium")   = PID( 2,4);

    // charged gas density models
    py::class_<CosmicRayDensity, std::shared_ptr<CosmicRayDensity>>(m, "CosmicRayDensity")
	      .def("getDensityPerEnergy", &CosmicRayDensity::getDensityPerEnergy);
    py::class_<SimpleCRDensity, std::shared_ptr<SimpleCRDensity>, CosmicRayDensity>(m, "SimpleCRDensity")
	      .def(py::init<>())
	      .def("getDensityPerEnergy", &SimpleCRDensity::getDensityPerEnergy);
    py::class_<Sun08CRDensity, std::shared_ptr<Sun08CRDensity>, CosmicRayDensity>(m, "Sun08CRDensity")
	      .def(py::init<>())
	      .def("getDensityPerEnergy", &Sun08CRDensity::getDensityPerEnergy);
    py::class_<WMAP07CRDensity, std::shared_ptr<WMAP07CRDensity>, CosmicRayDensity>(m, "WMAP07CRDensity")
	      .def(py::init<>())
	      .def("getDensityPerEnergy", &WMAP07CRDensity::getDensityPerEnergy);
    py::class_<Dragon2DCRDensity, std::shared_ptr<Dragon2DCRDensity>, CosmicRayDensity>(m, "Dragon2DCRDensity")
	      .def(py::init<>())
	      .def(py::init<const std::vector<PID>>())
	      .def(py::init<const std::string, const std::vector<PID>>());
	      //.def("getDensityPerEnergy", &Dragon2DCRDensity::getDensityPerEnergy);
	      
}

}
