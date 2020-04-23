#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "hermes/magneticfields/JF12Field.h"
#include "hermes/magneticfields/MagneticField.h"
#include "hermes/magneticfields/MagneticFieldGrid.h"
#include "hermes/magneticfields/PT11Field.h"
#include "hermes/magneticfields/Sun08Field.h"
#include "hermes/magneticfields/WMAP07Field.h"

namespace py = pybind11;

namespace hermes { namespace magneticfields {

void init(py::module &m) {
	py::module subm = m.def_submodule("magneticfields");
	subm.doc() = "magnetic fields package";

	py::class_<MagneticField, std::shared_ptr<MagneticField>>(subm,
	                                                          "MagneticField")
	    .def("getField", &MagneticField::getField);

	py::class_<Sun08Field, std::shared_ptr<Sun08Field>, MagneticField>(
	    subm, "Sun08Field")
	    .def(py::init<>())
	    .def("getField", &MagneticField::getField);

	py::class_<PT11Field, std::shared_ptr<PT11Field>, MagneticField>(
	    subm, "PT11Field")
	    .def(py::init<>())
	    .def("getField", &MagneticField::getField);

	py::class_<JF12Field, std::shared_ptr<JF12Field>, MagneticField>(
	    subm, "JF12Field")
	    .def(py::init<>())
	    .def("getField", &MagneticField::getField);
}

}}  // namespace hermes::magneticfields
