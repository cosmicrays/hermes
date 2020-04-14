#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "hermes/integrators/IntegratorTemplate.h"
#include "hermes/integrators/LOSIntegrationMethods.h"
#include "hermes/integrators/DMIntegrator.h"
#include "hermes/integrators/RMIntegrator.h"
#include "hermes/integrators/SynchroIntegrator.h"
#include "hermes/integrators/FreeFreeIntegrator.h"
#include "hermes/integrators/SynchroAbsorptionIntegrator.h"
#include "hermes/integrators/PiZeroIntegrator.h"
#include "hermes/integrators/InverseComptonIntegrator.h"

#define PPCAT(A, B) A ## B
#define NEW_INTEGRATOR(_obj, _name, _class, _qpxl, _qstep) \
	using PPCAT(_class, ParentClass) = IntegratorTemplate<_qpxl, _qstep>; \
	py::class_<PPCAT(_class, ParentClass), std::shared_ptr<PPCAT(_class, ParentClass)>>(m, (std::string(_name) + std::string("Parent")).c_str()) \
		.def(py::init<>()); \
	py::class_<_class, PPCAT(_class, ParentClass), std::shared_ptr<_class>> _obj(m, _name, py::buffer_protocol());

namespace py = pybind11;

namespace hermes {

template<typename INTEGRATOR>
void declare_default_integrator_methods(py::class_<INTEGRATOR> c) {
	c.def("getSunPosition", &INTEGRATOR::getSunPosition);
	c.def("setSunPosition", &INTEGRATOR::setSunPosition);
	c.def("setupCacheTable", &INTEGRATOR::setupCacheTable);
}


void init_integrators(py::module &m) {
  
	// DMIntegrator
	NEW_INTEGRATOR(dmintegrator, "DMIntegrator", DMIntegrator, QDispersionMeasure, QNumber);
	dmintegrator.def(py::init<const std::shared_ptr<chargedgas::ChargedGasDensity> >());
	dmintegrator.def("getLOSProfile", &DMIntegrator::getLOSProfile);
	declare_default_integrator_methods<DMIntegrator>(dmintegrator);
	
	// RMIntegrator
	NEW_INTEGRATOR(rmintegrator, "RMIntegrator", RMIntegrator, QRotationMeasure, QNumber);
	rmintegrator.def(py::init<const std::shared_ptr<MagneticField>,
			    	  const std::shared_ptr<chargedgas::ChargedGasDensity> >());
	declare_default_integrator_methods<RMIntegrator>(rmintegrator);
	
	// FreeFreeIntegrator
	NEW_INTEGRATOR(ffintegrator, "FreeFreeIntegrator", FreeFreeIntegrator, QTemperature, QFrequency);
	ffintegrator.def(py::init<const std::shared_ptr<chargedgas::ChargedGasDensity> >());
	declare_default_integrator_methods<FreeFreeIntegrator>(ffintegrator);

	// SynchroIntegrator
	py::class_<SynchroIntegrator, FreeFreeIntegratorParentClass, std::shared_ptr<SynchroIntegrator>>
		synchrointegrator(m, "SynchroIntegrator", py::buffer_protocol());
	synchrointegrator.def(py::init<const std::shared_ptr<MagneticField>,
			    	       const std::shared_ptr<cosmicrays::CosmicRayDensity> >());
	declare_default_integrator_methods<SynchroIntegrator>(synchrointegrator);

	// SynchroAbsorption
	py::class_<SynchroAbsorptionIntegrator, FreeFreeIntegratorParentClass, std::shared_ptr<SynchroAbsorptionIntegrator>>
		synchroabsintegrator(m, "SynchroAbsorptionIntegrator", py::buffer_protocol());
	synchroabsintegrator.def(py::init<const std::shared_ptr<MagneticField>,
			    	 	  const std::shared_ptr<cosmicrays::CosmicRayDensity>,
				 	  const std::shared_ptr<chargedgas::ChargedGasDensity> >());
	declare_default_integrator_methods<SynchroAbsorptionIntegrator>(synchroabsintegrator);

	// InverseComptonIntegrator
	NEW_INTEGRATOR(icintegrator, "InverseComptonIntegrator", InverseComptonIntegrator, QDifferentialIntensity, QEnergy);
	icintegrator.def(py::init<const std::shared_ptr<cosmicrays::CosmicRayDensity>,
                	    	  const std::shared_ptr<PhotonField>,
                            	  const std::shared_ptr<DifferentialCrossSection>>());
	declare_default_integrator_methods<InverseComptonIntegrator>(icintegrator);
	icintegrator.def("integrateOverEnergy", &InverseComptonIntegrator::integrateOverEnergy);
	icintegrator.def("integrateOverPhotonEnergy", &InverseComptonIntegrator::integrateOverPhotonEnergy);

	// PiZeroIntegrator
	py::class_<PiZeroIntegrator, InverseComptonIntegratorParentClass,
		std::shared_ptr<PiZeroIntegrator>> pizerointegrator(m, "PiZeroIntegrator", py::buffer_protocol());
	pizerointegrator.def(py::init<const std::shared_ptr<cosmicrays::CosmicRayDensity>,
                		      const std::shared_ptr<RingModelDensity>,
                           	      const std::shared_ptr<DifferentialCrossSection>>());
	pizerointegrator.def(py::init<const std::vector<std::shared_ptr<cosmicrays::CosmicRayDensity>>,
                		      const std::shared_ptr<RingModelDensity>,
                           	      const std::shared_ptr<DifferentialCrossSection>>());
	declare_default_integrator_methods<PiZeroIntegrator>(pizerointegrator);

}

}
