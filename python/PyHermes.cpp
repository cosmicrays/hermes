#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace hermes {

namespace cosmicrays	{ void init(py::module &); }
namespace chargedgas	{ void init(py::module &); }
namespace interactions	{ void init(py::module &); }
namespace magneticfields{ void init(py::module &); }
namespace neutralgas 	{ void init(py::module &); }
namespace photonfields	{ void init(py::module &); }

void init_quantity(py::module &);
void init_units(py::module &);
void init_common(py::module &);
void init_particleid(py::module &);
void init_vectors(py::module &);
void init_outputs(py::module &);
void init_skymaps(py::module &);
void init_integrators(py::module &);

PYBIND11_MODULE(pyhermes, m) {
	m.doc() = "HERMES python binding";
	
	chargedgas::init(m);
	cosmicrays::init(m);
	interactions::init(m);
	magneticfields::init(m);
	neutralgas::init(m);
	photonfields::init(m);

	init_quantity(m);
	init_units(m);
	init_common(m);
	init_particleid(m);
	init_vectors(m);
	init_outputs(m);
	init_skymaps(m);
	init_integrators(m);
}

}
