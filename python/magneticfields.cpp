#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "hermes/magneticfields/JF12.h"
#include "hermes/magneticfields/MagneticField.h"
#include "hermes/magneticfields/MagneticFieldGrid.h"
#include "hermes/magneticfields/PT11.h"
#include "hermes/magneticfields/Sun08.h"
#include "hermes/magneticfields/WMAP07.h"

namespace py = pybind11;

namespace hermes { namespace magneticfields {

void init(py::module &m) {
	py::module subm = m.def_submodule("magneticfields");
	subm.doc() = "magnetic fields package";

	py::class_<MagneticField, std::shared_ptr<MagneticField>>(subm,
	                                                          "MagneticField")
	    .def("getField", &MagneticField::getField);

	py::class_<Sun08, std::shared_ptr<Sun08>, MagneticField>(
	    subm, "Sun08")
	    .def(py::init<>())
	    .def("getField", &MagneticField::getField);

	py::class_<PT11, std::shared_ptr<PT11>, MagneticField>(
	    subm, "PT11")
	    .def(py::init<>())
	    .def("getField", &MagneticField::getField);

	py::class_<JF12, std::shared_ptr<JF12>, MagneticField>(
	    subm, "JF12")
	    .def(py::init<>())
	    .def("getField", &MagneticField::getField);
}

}}  // namespace hermes::magneticfields
