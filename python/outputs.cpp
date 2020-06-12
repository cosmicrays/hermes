#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include "hermes/outputs/CTAFormat.h"
#include "hermes/outputs/HEALPixFormat.h"
#include "hermes/outputs/Output.h"

namespace py = pybind11;

namespace hermes { namespace outputs {

void init(py::module &m) {
	py::module subm = m.def_submodule("outputs");
	subm.doc() = "outputs package";

	// NOLINTNEXTLINE(bugprone-unused-raii)
	py::class_<Output, std::shared_ptr<Output>>(subm, "Output");
	py::class_<HEALPixFormat, std::shared_ptr<HEALPixFormat>, Output>(
	    subm, "HEALPixFormat")
	    .def(py::init<std::string>());
	py::class_<CTAFormat, std::shared_ptr<CTAFormat>, Output>(
	    subm, "CTAFormat")
	    .def(py::init<std::string>());
}

}}  // namespace hermes::outputs
