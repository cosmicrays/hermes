#include "hermes/integrators/GenericIntegrator.h"
#include "hermes/Common.h"

#include <memory>

namespace hermes {

GenericIntegrator::GenericIntegrator(
	const std::shared_ptr<MagneticField> mfield_,
	const std::shared_ptr<CosmicRayDensity> crdensity_,
	const std::shared_ptr<GasDensity> gdensity_) : 
	mfield(mfield_), crdensity(crdensity_), gdensity(gdensity_) {

	intSynchro = std::make_shared<SynchroIntegrator>(
		SynchroIntegrator(mfield, crdensity));
	intFreeFree = std::make_shared<FreeFreeIntegrator>(
		FreeFreeIntegrator(gdensity));
}

GenericIntegrator::~GenericIntegrator() { }

QNumber GenericIntegrator::integrateOverLOS(
		QDirection direction) const {
	return integrateOverLOS(direction, 408_MHz);
}

QNumber GenericIntegrator::integrateOverLOS(
		QDirection direction_, QFrequency freq_) const {

	Vector3QLength positionSun(8.5_kpc, 0, 0);
	Vector3QLength pos(0.0);
	QIntensity total_intensity(0);
	QLength delta_d = 1.0_pc;

	QInverseLength opticalDepth(0);
	std::vector<QInverseLength> opticalDepthLOS;

	// distance from the (spherical) galactic border in the given direction
	QLength maxDistance = distanceToGalBorder(positionSun, direction_);
	
	for(QLength dist = delta_d; dist <= maxDistance; dist += delta_d) {
		pos = getGalacticPosition(positionSun, dist, direction_);
		opticalDepth += intFreeFree->absorptionCoefficient(pos, freq_);
		opticalDepthLOS.push_back(opticalDepth);
	}
	return QNumber(opticalDepth.getValue()/opticalDepthLOS.size()); //opticalDepthLOS[opticalDepthLOS.size()-1];
	//if (opticalDepthLOS[opticalDepthLOS.size()-1] > QNumber(1))
	//	std::cerr << opticalDepthLOS[opticalDepthLOS.size()-1] << std::endl;

	// TODO: implement sophisticated adaptive integration method :-)
	/*auto opticalDepthIter = opticalDepthLOS.begin();
	for(QLength dist = delta_d; dist <= maxDistance; dist += delta_d) {
		pos = getGalacticPosition(positionSun, dist, direction_);
	
		total_intensity += intSynchro->integrateOverEnergy(pos, freq_) / 4_pi * 
					exp((*opticalDepthIter) - opticalDepthLOS[opticalDepthLOS.size()-1]) * delta_d;
		++opticalDepthIter;
	}

	return intensityToTemperature(total_intensity, freq_);
	*/
}

} // namespace hermes 
