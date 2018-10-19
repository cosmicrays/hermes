#include "hermes/integrators/RMIntegrator.h"

namespace hermes {

RMIntegrator::RMIntegrator(const std::shared_ptr<MagneticField> mfield) : mfield(mfield) {	
}

RMIntegrator::~RMIntegrator() { }

double RMIntegrator::integral(tDirection dir) {
	return 0.0;	
}

} // namespace hermes 
