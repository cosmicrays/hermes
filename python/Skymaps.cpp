#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "hermes/integrators/DispersionMeasureIntegrator.h"
#include "hermes/integrators/FreeFreeIntegrator.h"
#include "hermes/integrators/InverseComptonIntegrator.h"
#include "hermes/integrators/PiZeroIntegrator.h"
#include "hermes/integrators/RotationMeasureIntegrator.h"
#include "hermes/integrators/SynchroAbsorptionIntegrator.h"
#include "hermes/integrators/SynchroIntegrator.h"
#include "hermes/skymaps/DispersionMeasureSkymap.h"
#include "hermes/skymaps/GammaSkymap.h"
#include "hermes/skymaps/GammaSkymapRange.h"
#include "hermes/skymaps/RadioSkymap.h"
#include "hermes/skymaps/RadioSkymapRange.h"
#include "hermes/skymaps/RotationMeasureSkymap.h"
#include "hermes/skymaps/Skymap.h"
#include "hermes/skymaps/SkymapMask.h"
#include "hermes/skymaps/SkymapTemplate.h"

namespace py = pybind11;

namespace hermes {

template <typename SKYMAP, typename QPXL, typename QSTEP>
void declare_default_skymap_methods(py::class_<SKYMAP> c) {
	using IntegratorClass = IntegratorTemplate<QPXL, QSTEP>;
	using ParentClass = SkymapTemplate<QPXL, QSTEP>;

	c.def("getDescription", &SKYMAP::getDescription);
	c.def("getNpix", &SKYMAP::getNpix);
	c.def("setMask", &SKYMAP::setMask);
	c.def("getMask", &SKYMAP::getMask);
	c.def("getNside", &SKYMAP::getNside);
	c.def("convertToUnits", &SKYMAP::convertToUnits);
	c.def("getOutputUnits", &SKYMAP::getOutputUnits);
	c.def("getOutputUnitsAsString", &SKYMAP::getOutputUnitsAsString);
	c.def("getUnits", &SKYMAP::getUnits);
	c.def("setIntegrator",
	      [](SKYMAP &s, const std::shared_ptr<IntegratorClass> &i) {
		      s.setIntegrator(i);
	      });
	c.def("compute", &SKYMAP::compute);
	c.def("computePixel", &SKYMAP::computePixel);
	c.def("computePixelRange", &SKYMAP::computePixelRange);
	c.def("getPixel", &SKYMAP::getPixel);
	c.def("getMean", &SKYMAP::getMean);
	c.def("save", &SKYMAP::save);
	c.def("__getitem__", [](const SKYMAP &s, std::size_t i) -> QPXL {
		if (i >= s.size()) throw py::index_error();
		return s[i];
	});
	c.def("size", &SKYMAP::size);
	c.def("getUnmaskedPixelCount", &SKYMAP::getUnmaskedPixelCount);
	c.def("__len__", &SKYMAP::size);
	c.def(
	    "__iter__",
	    [](const SKYMAP &s) { return py::make_iterator(s.begin(), s.end()); },
	    py::keep_alive<
	        0, 1>() /* Essential: keep object alive while iterator exists */);
	c.def_buffer([](SKYMAP &s) -> py::buffer_info {
		// buffer protocol: https://docs.python.org/3/c-api/buffer.html
		return py::buffer_info(
		    s.data(),                                /* Pointer to buffer */
		    sizeof(QPXL),                            /* Size of one scalar */
		    py::format_descriptor<double>::format(), /* Python struct-style
		                        format descriptor */
		    1,                                       /* Number of dimensions */
		    {s.size()},                              /* Buffer dimensions */
		    {sizeof(QPXL)} /* Strides (in bytes) for each index */
		);
	});
}

void init_skymaps(py::module &m) {
	// DispersionMeasureSkymap
	py::class_<DispersionMeasureSkymap,
	           std::shared_ptr<DispersionMeasureSkymap>>
	    dmskymap(m, "DispersionMeasureSkymap", py::buffer_protocol());
	dmskymap.def(py::init<const std::size_t>(),
	             py::arg("nside"));  // constructor
	declare_default_skymap_methods<DispersionMeasureSkymap, QDispersionMeasure,
	                               QNumber>(dmskymap);

	// RotationMeasureSkymap
	py::class_<RotationMeasureSkymap, std::shared_ptr<RotationMeasureSkymap>>
	    rmskymap(m, "RotationMeasureSkymap", py::buffer_protocol());
	rmskymap.def(py::init<const std::size_t>(),
	             py::arg("nside"));  // constructor
	declare_default_skymap_methods<RotationMeasureSkymap, QRotationMeasure,
	                               QNumber>(rmskymap);

	// RadioSkymap
	py::class_<RadioSkymap, std::shared_ptr<RadioSkymap>> radioskymap(
	    m, "RadioSkymap", py::buffer_protocol());
	radioskymap.def(py::init<const std::size_t, const QFrequency &>(),
	                py::arg("nside"), py::arg("frequency"));  // constructor
	radioskymap.def("getFrequency", &RadioSkymap::getFrequency);
	declare_default_skymap_methods<RadioSkymap, QTemperature, QFrequency>(
	    radioskymap);

	// GammaSkymap
	py::class_<GammaSkymap, std::shared_ptr<GammaSkymap>> gammaskymap(
	    m, "GammaSkymap", py::buffer_protocol());
	gammaskymap.def(py::init<const std::size_t, const QEnergy &>(),
	                py::arg("nside"), py::arg("Egamma"));  // constructor
	gammaskymap.def("getEnergy", &GammaSkymap::getEnergy);
	declare_default_skymap_methods<GammaSkymap, QDiffIntensity, QEnergy>(
	    gammaskymap);

	// GammaSkymapRange
	py::class_<GammaSkymapRange, std::shared_ptr<GammaSkymapRange>>(
	    m, "GammaSkymapRange")
	    .def(py::init<std::size_t, const QEnergy &, const QEnergy &, int>(),
	         py::arg("nside"), py::arg("Emin"), py::arg("Emax"),
	         py::arg("E_steps"))
	    .def("setIntegrator",
	         [](GammaSkymapRange &s,
	            const std::shared_ptr<
	                IntegratorTemplate<QDiffIntensity, QEnergy>> &i) {
		         s.setIntegrator(i);
	         })
	    .def("setMask", &GammaSkymapRange::setMask)
	    .def("compute", &GammaSkymapRange::compute)
	    .def("save", &GammaSkymapRange::save)
	    .def("__getitem__",
	         [](const GammaSkymapRange &s, std::size_t i) -> GammaSkymap {
		         if (i >= s.size()) throw py::index_error();
		         return s[i];
	         })
	    .def("__len__", &GammaSkymapRange::size)
	    .def(
	        "__iter__",
	        [](const GammaSkymapRange &s) {
		        return py::make_iterator(s.begin(), s.end());
	        },
	        py::keep_alive<
	            0,
	            1>() /* Essential: keep object alive while iterator exists */);

	// Skymap Masks
	// NOLINTNEXTLINE(bugprone-unused-raii)
	py::class_<SkymapMask, std::shared_ptr<SkymapMask>>(m, "SkymapMask");
	py::class_<InvertMask, std::shared_ptr<InvertMask>, SkymapMask>(
	    m, "InvertMask")
	    .def(py::init<const std::shared_ptr<SkymapMask>>(), py::arg("mask"));
	py::class_<MaskList, std::shared_ptr<MaskList>, SkymapMask>(m, "MaskList")
	    .def(py::init<>())
	    .def("addMask", &MaskList::addMask);
	// in galactic coordinates: b=(-90_deg, 90_deg), l=(-180_deg, 180_deg)
	py::class_<RectangularWindow, std::shared_ptr<RectangularWindow>,
	           SkymapMask>(m, "RectangularWindow")
	    .def(py::init<const std::array<QAngle,2> &, const std::array<QAngle,2> &>(),
	         py::arg("latitude"), py::arg("longitude"));
	py::class_<CircularWindow, std::shared_ptr<CircularWindow>, SkymapMask>(
	    m, "CircularWindow")
	    .def(py::init<const QDirection &, const QAngle &>(), py::arg("centre"),
	         py::arg("aperature"));
}

}  // namespace hermes
