#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "hermes/photonField/PhotonField.h"
#include "hermes/photonField/CMB.h"
#include "hermes/photonField/ISRF.h"

namespace py = pybind11;

namespace hermes {

void init_photon_field(py::module &m) {

    // charged gas density models
    py::class_<PhotonField, std::shared_ptr<PhotonField>>(m, "PhotonField");
	      //.def("getEnergyDensity", &PhotonField::getEnergyDensity);
    py::class_<CMB, std::shared_ptr<CMB>, PhotonField>(m, "CMB")
	      .def(py::init<>());
	      //.def("getEnergyDensity", &CMB::getEnergyDensity);
    py::class_<ISRF, std::shared_ptr<ISRF>, PhotonField>(m, "ISRF")
	      .def(py::init<>());
}

}
