#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace hermes {

namespace cosmicrays { void init(py::module &); }
namespace chargedgas { void init(py::module &); }

void init_quantity(py::module &);
void init_units(py::module &);
void init_common(py::module &);
void init_particleid(py::module &);
void init_vectors(py::module &);
void init_outputs(py::module &);
void init_skymaps(py::module &);
void init_integrators(py::module &);
void init_magnetic_fields(py::module &);
void init_neutral_gas_density(py::module &);
void init_photon_field(py::module &);
void init_interactions(py::module &);

PYBIND11_MODULE(pyhermes, m) {
	m.doc() = "HERMES python binding";
	
	chargedgas::init(m);
	cosmicrays::init(m);

	init_quantity(m);
	init_units(m);
	init_common(m);
	init_particleid(m);
	init_vectors(m);
	init_outputs(m);
	init_skymaps(m);
	init_neutral_gas_density(m);
	init_photon_field(m);
	init_interactions(m);
	init_integrators(m);
	init_magnetic_fields(m);
}

}
