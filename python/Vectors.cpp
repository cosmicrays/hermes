#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include "hermes/Vector3.h"
#include "hermes/Vector3Quantity.h"

namespace py = pybind11;

namespace hermes {

void init_vectors(py::module &m) {
    // Vector quantities
    py::class_<Vector3QLength, std::shared_ptr<Vector3QLength>>(
	m, "Vector3QLength")
	.def(py::init<QLength, QLength, QLength>())
	.def(py::init<QLength>())
	.def("getX", &Vector3QLength::getX)
	.def("getY", &Vector3QLength::getY)
	.def("getZ", &Vector3QLength::getZ)
	.def("getValue", &Vector3QLength::getValue)
	.def("getR", &Vector3QLength::getR)
	.def("getTheta", &Vector3QLength::getTheta)
	.def("getPhi", &Vector3QLength::getPhi)
	.def("getAngleTo", &Vector3QLength::getAngleTo)
	.def(
	    "__eq__",
	    [](const Vector3QLength &self,
	       const Vector3QLength &other) -> bool { return self == other; },
	    py::is_operator())
	.def(
	    "__add__",
	    [](const Vector3QLength &self, const Vector3QLength &other)
		-> Vector3QLength { return self + other; },
	    py::is_operator())
	.def(
	    "__sub__",
	    [](const Vector3QLength &self, const Vector3QLength &other)
		-> Vector3QLength { return self - other; },
	    py::is_operator());
}

} // namespace hermes
