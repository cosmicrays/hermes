#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "hermes/Common.h"

namespace py = pybind11;

namespace hermes {

void init_common(py::module &m) {


    m.def("getDataPath", &getDataPath);

}

}
