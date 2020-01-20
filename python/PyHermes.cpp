#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace hermes {

void init_units(py::module &);
void init_vectors(py::module &);
void init_outputs(py::module &);
void init_skymaps(py::module &);
void init_integrators(py::module &);
void init_magnetic_fields(py::module &);
void init_charged_gas_density(py::module &);

PYBIND11_MODULE(pyhermes, m) {
    m.doc() = "HERMES python binding";
    init_units(m);
    init_vectors(m);
    init_outputs(m);
    init_skymaps(m);
    init_charged_gas_density(m);
    init_integrators(m);
    init_magnetic_fields(m);
}

}
