#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

#include "hermes/outputs/Output.h"
#include "hermes/outputs/FITSOutput.h"

namespace py = pybind11;

namespace hermes { namespace outputs {
    
void init(py::module &m) {
    
    py::module subm = m.def_submodule("outputs");
    subm.doc() = "outputs package";

    // Outputs
    py::class_<Output, std::shared_ptr<Output>>(subm, "Output");
    py::class_<FITSOutput, std::shared_ptr<FITSOutput>, Output>(subm, "FITSOutput")
	      .def(py::init<std::string>());
}

} // namespace outputs
} // namespace hermes
