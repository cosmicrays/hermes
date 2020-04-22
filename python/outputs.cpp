#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

#include "hermes/outputs/Output.h"
#include "hermes/outputs/HEALPix.h"

namespace py = pybind11;

namespace hermes { namespace outputs {
    
void init(py::module &m) {
    
    py::module subm = m.def_submodule("outputs");
    subm.doc() = "outputs package";

    // NOLINTNEXTLINE(bugprone-unused-raii)
    py::class_<Output, std::shared_ptr<Output>>(subm, "Output");
    py::class_<HEALPix, std::shared_ptr<HEALPix>, Output>(subm, "HEALPix")
	      .def(py::init<std::string>());
}

} // namespace outputs
} // namespace hermes
