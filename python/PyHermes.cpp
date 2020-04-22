#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace hermes {

void init_common(py::module &);
void init_integrators(py::module &);
void init_particleid(py::module &);
void init_quantity(py::module &);
void init_skymaps(py::module &);
void init_vectors(py::module &);

namespace cosmicrays {
void init(py::module &);
}
namespace chargedgas {
void init(py::module &);
}
namespace interactions {
void init(py::module &);
}
namespace magneticfields {
void init(py::module &);
}
namespace neutralgas {
void init(py::module &);
}
namespace outputs {
void init(py::module &);
}
namespace photonfields {
void init(py::module &);
}
namespace units {
void init(py::module &);
}

PYBIND11_MODULE(pyhermes, m) {
    m.doc() = "HERMES python binding";

    // main namespace
    init_common(m);
    init_integrators(m);
    init_particleid(m);
    init_quantity(m);
    init_skymaps(m);
    init_vectors(m);

    // submodules
    chargedgas::init(m);
    cosmicrays::init(m);
    interactions::init(m);
    magneticfields::init(m);
    neutralgas::init(m);
    outputs::init(m);
    photonfields::init(m);
    units::init(m);
}

} // namespace hermes
