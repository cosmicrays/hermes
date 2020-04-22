#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "hermes/Common.h"
#include "hermes/HEALPixBits.h"

namespace py = pybind11;

namespace hermes {

void init_common(py::module &m) {

    m.def("getDataPath", &getDataPath);
    m.def("toGalCoord", &toGalCoord);
    m.def("fromGalCoord", &fromGalCoord);
    m.def("getLorentzFactor", &getLorentzFactor);
    m.def("intensityToTemperature", &intensityToTemperature);

    m.attr("UNSEEN") = UNSEEN;
    m.def("ang2pix_ring", &ang2pix_ring);
    m.def("pix2ang_ring", &pix2ang_ring);
}

} // namespace hermes
