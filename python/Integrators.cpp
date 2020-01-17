#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "hermes/integrators/Integrator.h"
#include "hermes/integrators/LOSIntegrationMethods.h"
#include "hermes/integrators/GenericIntegrator.h"
#include "hermes/integrators/RMIntegrator.h"
#include "hermes/integrators/SynchroIntegrator.h"
#include "hermes/integrators/FreeFreeIntegrator.h"
#include "hermes/integrators/SynchroAbsorptionIntegrator.h"
#include "hermes/integrators/PiZeroIntegrator.h"
#include "hermes/integrators/InverseComptonIntegrator.h"

namespace py = pybind11;

namespace hermes {

void init_integrators(py::module &m) {

    py::class_<RMIntegrator, std::shared_ptr<RMIntegrator>>(m, "RMIntegrator")
	      .def(py::init<std::shared_ptr<MagneticField>,
			    std::shared_ptr<ChargedGasDensity> >());
}

}
