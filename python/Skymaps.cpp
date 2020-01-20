#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "hermes/skymaps/Skymap.h"
#include "hermes/skymaps/SkymapMask.h"
#include "hermes/skymaps/SkymapTemplate.h"
#include "hermes/skymaps/GenericSkymap.h"
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

template<typename QPXL, typename QSTEP>
    void declare_skymap(py::module &m,  std::string const & classname) {
	using Class = SkymapTemplate<QPXL, QSTEP>;
	py::class_<Class, std::shared_ptr<Class>>(m, classname.c_str())
	       .def(py::init<const std::size_t>())
	       .def("setIntegrator", &Class::setIntegrator);
    }


void init_skymaps(py::module &m) {
    // buffer protocol: https://docs.python.org/3/c-api/buffer.html
    py::class_<DMSkymap, std::shared_ptr<DMSkymap>>(m, "DMSkymap", py::buffer_protocol())
	       .def(py::init<const std::size_t>(), py::arg("nside"))
	       .def("setOutputUnits", &DMSkymap::setOutputUnits)
	       .def("convertToUnits", &DMSkymap::setOutputUnits)
	       .def("getDescription", &DMSkymap::getDescription)
	       .def("setIntegrator", [](DMSkymap &s, std::shared_ptr<DMIntegrator> i) { s.setIntegrator(i); })
	       .def("compute", &DMSkymap::compute)
	       .def("save", &DMSkymap::save)
	       .def_buffer([](DMSkymap &s) -> py::buffer_info {
		        return py::buffer_info(
			        s.data(),          /* Pointer to buffer */
			        sizeof(QDispersionMeasure),   /* Size of one scalar */
            			py::format_descriptor<double>::format(), /* Python struct-style format descriptor */
            			1,                             /* Number of dimensions */
				{s.getSize()},                 /* Buffer dimensions */
				{sizeof(QDispersionMeasure)}             /* Strides (in bytes) for each index */
        		);
    		});
    
    py::class_<RMSkymap, std::shared_ptr<RMSkymap>>(m, "RMSkymap", py::buffer_protocol())
	       .def(py::init<const std::size_t>(), py::arg("nside"))
	       .def("getDescription", &RMSkymap::getDescription)
	       .def("setIntegrator", [](RMSkymap &s, std::shared_ptr<RMIntegrator> i) { s.setIntegrator(i); })
	       .def("compute", &RMSkymap::compute)
	       .def("save", &RMSkymap::save)
	       .def_buffer([](RMSkymap &s) -> py::buffer_info {
		        return py::buffer_info(
			        s.data(),          /* Pointer to buffer */
			        sizeof(QRotationMeasure),   /* Size of one scalar */
            			py::format_descriptor<double>::format(), /* Python struct-style format descriptor */
            			1,                             /* Number of dimensions */
				{s.getSize()},                 /* Buffer dimensions */
				{sizeof(QRotationMeasure)}             /* Strides (in bytes) for each index */
        		);
    		});

    py::class_<RadioSkymap, std::shared_ptr<RadioSkymap>>(m, "RadioSkymap", py::buffer_protocol())
	       .def(py::init<const std::size_t, const QFrequency>())
	       .def("getDescription", &RadioSkymap::getDescription)
	       .def("setIntegrator", &RadioSkymap::setIntegrator)
	       .def("compute", &RadioSkymap::compute)
	       .def_buffer([](RadioSkymap &s) -> py::buffer_info {
		        return py::buffer_info(
			        s.data(),
			        sizeof(QTemperature),
            			py::format_descriptor<double>::format(),
            			1,
				{s.getSize()},
				{sizeof(QTemperature)}
        		);
    		});

}

}
