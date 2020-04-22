#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "hermes/ParticleID.h"

namespace py = pybind11;

namespace hermes {

void init_particleid(py::module &m) {

    // PID
    py::class_<PID, std::shared_ptr<PID>>(m, "PID").def(py::init<int, int>());
    m.attr("Electron") = PID(-1, 0);
    m.attr("Positron") = PID(1, 0);
    m.attr("Proton") = PID(1, 1);
    m.attr("Helium") = PID(2, 4);
}

} // namespace hermes
