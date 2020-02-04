#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "hermes/magneticField/MagneticField.h"
#include "hermes/magneticField/MagneticFieldGrid.h"
#include "hermes/magneticField/WMAP07Field.h"
#include "hermes/magneticField/Sun08Field.h"
#include "hermes/magneticField/PT11Field.h"
#include "hermes/magneticField/JF12Field.h"

namespace py = pybind11;

namespace hermes {

void init_magnetic_fields(py::module &m) {

    py::class_<MagneticField, std::shared_ptr<MagneticField>>(m, "MagneticField")
	      .def("getField", &MagneticField::getField);
    
    py::class_<Sun08Field, std::shared_ptr<Sun08Field>, MagneticField>(m, "Sun08Field")
	      .def(py::init<>())
	      .def("getField", &MagneticField::getField);

    py::class_<PT11Field, std::shared_ptr<PT11Field>, MagneticField>(m, "PT11Field")
	      .def(py::init<>())
	      .def("getField", &MagneticField::getField);

    py::class_<JF12Field, std::shared_ptr<JF12Field>, MagneticField>(m, "JF12Field")
	      .def(py::init<>())
	      .def("getField", &MagneticField::getField);
}

}
