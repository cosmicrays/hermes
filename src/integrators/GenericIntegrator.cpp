#include "hermes/integrators/GenericIntegrator.h"
#include "hermes/Common.h"

#include <memory>

namespace hermes {

GenericIntegrator::GenericIntegrator(
	const std::shared_ptr<MagneticField> mfield_,
	const std::shared_ptr<CosmicRayDensity> crdensity_,
	const std::shared_ptr<ChargedGasDensity> cgdensity_, 
	const std::shared_ptr<RingModelDensity> ngdensity_) : 
	mfield(mfield_), crdensity(crdensity_), cgdensity(cgdensity_),
        ngdensity(ngdensity_) {

	intSynchro = std::make_shared<SynchroIntegrator>(
		SynchroIntegrator(mfield, crdensity));
	intFreeFree = std::make_shared<FreeFreeIntegrator>(
		FreeFreeIntegrator(cgdensity));
}

GenericIntegrator::~GenericIntegrator() { }

QNumber GenericIntegrator::integrateOverLOS(
		QDirection direction) const {
	return integrateOverLOS(direction, 408_MHz);
}

QNumber GenericIntegrator::integrateOverLOS(
		QDirection direction_, QFrequency freq_) const {

	double tot = 0;
	tot = static_cast<double>((ngdensity->operator[](5))->getHIColumnDensity(direction_));
	return tot;
}

} // namespace hermes 
