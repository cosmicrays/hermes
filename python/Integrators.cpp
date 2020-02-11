#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "hermes/integrators/Integrator.h"
#include "hermes/integrators/LOSIntegrationMethods.h"
#include "hermes/integrators/GenericIntegrator.h"
#include "hermes/integrators/DMIntegrator.h"
#include "hermes/integrators/RMIntegrator.h"
#include "hermes/integrators/SynchroIntegrator.h"
#include "hermes/integrators/FreeFreeIntegrator.h"
#include "hermes/integrators/SynchroAbsorptionIntegrator.h"
#include "hermes/integrators/PiZeroIntegrator.h"
#include "hermes/integrators/InverseComptonIntegrator.h"

namespace py = pybind11;

namespace hermes {

void init_integrators(py::module &m) {
    
    py::class_<DMIntegrator, std::shared_ptr<DMIntegrator>>(m, "DMIntegrator")
	      .def(py::init<const std::shared_ptr<ChargedGasDensity> >())
              .def("getSunPosition", &DMIntegrator::getSunPosition)
              .def("setSunPosition", &DMIntegrator::setSunPosition)
	      .def("getLOSProfile", &DMIntegrator::getLOSProfile);

    py::class_<RMIntegrator, std::shared_ptr<RMIntegrator>>(m, "RMIntegrator")
	      .def(py::init<const std::shared_ptr<MagneticField>,
			    const std::shared_ptr<ChargedGasDensity> >())
              .def("getSunPosition", &RMIntegrator::getSunPosition)
              .def("setSunPosition", &RMIntegrator::setSunPosition);
    
    py::class_<InverseComptonIntegrator, std::shared_ptr<InverseComptonIntegrator>>(m, "InverseComptonIntegrator")
	      .def(py::init<const std::shared_ptr<CosmicRayDensity>,
                	    const std::shared_ptr<PhotonField>,
                            const std::shared_ptr<KleinNishina>>())
              .def("getSunPosition", &InverseComptonIntegrator::getSunPosition)
              .def("setSunPosition", &InverseComptonIntegrator::setSunPosition)
	      .def("initCacheTable", &InverseComptonIntegrator::initCacheTable);
}

}
