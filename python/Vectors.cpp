#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

#include "hermes/Vector3.h"
#include "hermes/Vector3Quantity.h"

namespace py = pybind11;

namespace hermes {
    
void init_vectors(py::module &m) {
    // Vector quantities
    py::class_<Vector3QLength, std::shared_ptr<Vector3QLength>>(m, "Vector3QLength")
	      .def(py::init<QLength, QLength, QLength>());
}

}
