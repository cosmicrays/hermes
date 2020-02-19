#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "hermes/skymaps/Skymap.h"
#include "hermes/skymaps/SkymapMask.h"
#include "hermes/skymaps/SkymapTemplate.h"
#include "hermes/skymaps/DMSkymap.h"
#include "hermes/skymaps/RMSkymap.h"
#include "hermes/skymaps/GammaSkymap.h"
#include "hermes/skymaps/GammaSkymapRange.h"
#include "hermes/skymaps/RadioSkymap.h"
#include "hermes/skymaps/RadioSkymapRange.h"

#include "hermes/integrators/DMIntegrator.h"
#include "hermes/integrators/RMIntegrator.h"
#include "hermes/integrators/SynchroIntegrator.h"
#include "hermes/integrators/FreeFreeIntegrator.h"
#include "hermes/integrators/SynchroAbsorptionIntegrator.h"
#include "hermes/integrators/PiZeroIntegrator.h"
#include "hermes/integrators/InverseComptonIntegrator.h"

namespace py = pybind11;

namespace hermes {

template<typename SKYMAP, typename QPXL, typename QSTEP>
    void declare_default_skymap_methods(py::class_<SKYMAP> c) {
	    using IntegratorClass = IntegratorTemplate<QPXL, QSTEP>;
	    using ParentClass = SkymapTemplate<QPXL, QSTEP>;

	    c.def("getDescription", &SKYMAP::getDescription);
	    c.def("getNpix", &SKYMAP::getNpix);
	    c.def("setMask", &SKYMAP::setMask);
	    c.def("getMask", &SKYMAP::getMask);
	    c.def("getNside", &SKYMAP::getNside);
	    c.def("convertToUnits", &SKYMAP::convertToUnits);
	    c.def("getPixelUnitsAsString", &SKYMAP::getPixelUnitsAsString);
	    c.def("getUnits", &SKYMAP::getUnits);
	    c.def("setIntegrator", [](SKYMAP &s, std::shared_ptr<IntegratorClass> i) { s.setIntegrator(i); });
	    c.def("compute", &SKYMAP::compute);
	    c.def("computePixel", &SKYMAP::computePixel);
	    c.def("computePixelRange", &SKYMAP::computePixelRange);
	    c.def("getPixel", &SKYMAP::getPixel);
	    c.def("save", &SKYMAP::save);
	    c.def("__getitem__", [](const SKYMAP &s, std::size_t i) -> QPXL {
            		if (i >= s.size()) throw py::index_error();
            			return s[i]; });
	    c.def_buffer([](SKYMAP &s) -> py::buffer_info {
			    // buffer protocol: https://docs.python.org/3/c-api/buffer.html
			    return py::buffer_info(
				    s.data(),          /* Pointer to buffer */
				    sizeof(QPXL),      /* Size of one scalar */
				    py::format_descriptor<double>::format(), /* Python struct-style format descriptor */
				    1,                 /* Number of dimensions */
				    {s.size()},     /* Buffer dimensions */
				    {sizeof(QPXL)}     /* Strides (in bytes) for each index */
				);
			});
    }


void init_skymaps(py::module &m) {

	// DMSkymap
	py::class_<DMSkymap, std::shared_ptr<DMSkymap>> dmskymap(m, "DMSkymap", py::buffer_protocol());
	dmskymap.def(py::init<const std::size_t>(), py::arg("nside")); // constructor
	declare_default_skymap_methods<DMSkymap, QDispersionMeasure, QNumber>(dmskymap);

	// RMSkymap
	py::class_<RMSkymap, std::shared_ptr<RMSkymap>> rmskymap(m, "RMSkymap", py::buffer_protocol());
	rmskymap.def(py::init<const std::size_t>(), py::arg("nside")); // constructor
	declare_default_skymap_methods<RMSkymap, QRotationMeasure, QNumber>(rmskymap);

	// RadioSkymap
	py::class_<RadioSkymap, std::shared_ptr<RadioSkymap>> radioskymap(m, "RadioSkymap", py::buffer_protocol());
	radioskymap.def(py::init<const std::size_t, const QFrequency &>(), py::arg("nside"), py::arg("frequency")); // constructor
	declare_default_skymap_methods<RadioSkymap, QTemperature, QFrequency>(radioskymap);

	//GammaSkymap
	py::class_<GammaSkymap, std::shared_ptr<GammaSkymap>> gammaskymap(m, "GammaSkymap", py::buffer_protocol());
	gammaskymap.def(py::init<const std::size_t, const QEnergy &>(), py::arg("nside"), py::arg("Egamma")); // constructor
	declare_default_skymap_methods<GammaSkymap, QDifferentialIntensity, QEnergy>(gammaskymap);

	// Skymap Masks
	py::class_<SkymapMask, std::shared_ptr<SkymapMask>>(m, "SkymapMask");
	// in galactic coordinates: b=(-90_deg, 90_deg), l=(-180_deg, 180_deg)
	py::class_<RectangularWindow, std::shared_ptr<RectangularWindow>, SkymapMask>(m, "RectangularWindow")
		.def(py::init<const QAngle &, const QAngle &, const QAngle &, const QAngle &>(),
				py::arg("lat_top"), py::arg("lat_bottom"),
				py::arg("long_left"), py::arg("long_right"));

}

}
