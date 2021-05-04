#include "hermes/Common.h"
#include "hermes/Version.h"
#include "hermes/HEALPixBits.h"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <string>

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
	m.def("nside2npix", &nside2npix);
	m.def("nside2order", &nside2order);
	m.def("loc2pix", &loc2pix);

    m.attr("__version__") = std::string(g_GIT_DESC);
}

}  // namespace hermes
