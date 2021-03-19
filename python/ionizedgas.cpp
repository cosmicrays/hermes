#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "hermes/ionizedgas/IonizedGasDensity.h"
#include "hermes/ionizedgas/HII_Cordes91.h"
#include "hermes/ionizedgas/NE2001Simple.h"
#include "hermes/ionizedgas/YMW16.h"
#include "hermes/neutralgas/RingModel.h"

namespace py = pybind11;

namespace hermes { namespace ionizedgas {

void init(py::module &m) {
	py::module subm = m.def_submodule("ionizedgas");
	subm.doc() = "ionized gas package";

	// charged gas density models
	py::class_<IonizedGasDensity, std::shared_ptr<IonizedGasDensity>>(
	    subm, "IonizedGasDensity")
	    .def("getDensity", &IonizedGasDensity::getDensity);
	py::class_<HII_Cordes91, std::shared_ptr<HII_Cordes91>, IonizedGasDensity>(
	    subm, "HII_Cordes91")
	    .def(py::init<>())
	    .def("getDensity", &IonizedGasDensity::getDensity);
	py::class_<NE2001Simple, std::shared_ptr<NE2001Simple>, IonizedGasDensity>(
	    subm, "NE2001Simple")
	    .def(py::init<>())
	    .def("getDensity", &IonizedGasDensity::getDensity);
	py::class_<YMW16, std::shared_ptr<YMW16>, IonizedGasDensity>(subm, "YMW16")
	    .def(py::init<>())
	    .def("getDensity",
	         [](const YMW16 &d, const Vector3QLength &v) -> double {
		         return static_cast<double>(d.getDensity(v));
	         });
}

}}  // namespace hermes::ionizedgas
