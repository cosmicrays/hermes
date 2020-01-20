#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

#include "hermes/outputs/Output.h"
#include "hermes/outputs/FITSOutput.h"

namespace py = pybind11;

namespace hermes {
    
void init_outputs(py::module &m) {
    
    // Outputs
    py::class_<Output, std::shared_ptr<Output>>(m, "Output");
    py::class_<FITSOutput, std::shared_ptr<FITSOutput>, Output>(m, "FITSOutput")
	      .def(py::init<std::string>());
}

}
