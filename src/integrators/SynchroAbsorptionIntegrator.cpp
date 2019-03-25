#include "hermes/integrators/SynchroAbsorptionIntegrator.h"
#include "hermes/Common.h"

#include <memory>

namespace hermes {

SynchroAbsorptionIntegrator::SynchroAbsorptionIntegrator(
	const std::shared_ptr<MagneticField> mfield_,
	const std::shared_ptr<CosmicRayDensity> crdensity_,
	const std::shared_ptr<GasDensity> gdensity_) : 
	mfield(mfield_), crdensity(crdensity_), gdensity(gdensity_) {

	intSynchro = std::make_shared<SynchroIntegrator>(
		SynchroIntegrator(mfield, crdensity));
	intFreeFree = std::make_shared<FreeFreeIntegrator>(
		FreeFreeIntegrator(gdensity));
}

SynchroAbsorptionIntegrator::~SynchroAbsorptionIntegrator() { }

QInverseLength SynchroAbsorptionIntegrator::absorptionCoefficient(
	Vector3QLength pos_, QFrequency freq_) const {

	QTemperature T = 1e4_K;
	
	return intFreeFree->spectralEmissivity(pos_, freq_) *
		c_squared / (8_pi*h_planck*pow<3>(freq_)) *
		(exp(h_planck*freq_/(k_boltzmann*T)) - 1);
}

QTemperature SynchroAbsorptionIntegrator::integrateOverLOS(
		QDirection direction) const {
	return integrateOverLOS(direction, 408_MHz);
}

QTemperature SynchroAbsorptionIntegrator::integrateOverLOS(
		QDirection direction_, QFrequency freq_) const {

	Vector3QLength positionSun(8.5_kpc, 0, 0);
	Vector3QLength pos(0.0);
	QIntensity total_intensity(0);
	QLength delta_d = 10.0_pc;

	QNumber absorption(0);
	std::vector<QNumber> absorptionIntegrals;


	// distance from the (spherical) galactic border in the given direction
	QLength maxDistance = distanceToGalBorder(positionSun, direction_);
	
	for(QLength dist = 0; dist <= maxDistance; dist += delta_d) {
		pos = getGalacticPosition(positionSun, dist, direction_);
		absorption += absorptionCoefficient(pos, freq_) * delta_d;
		absorptionIntegrals.push_back(absorption);
	}


	// TODO: implement sophisticated adaptive integration method :-)
	auto i = 0;
	for(QLength dist = 0; dist <= maxDistance; dist += delta_d) {
		pos = getGalacticPosition(positionSun, dist, direction_);
		total_intensity += intSynchro->integrateOverEnergy(pos, freq_) * 
					exp(-absorptionIntegrals[i]) * delta_d;
		++i;
	}

	return intensityToTemperature(total_intensity / 4_pi, freq_);
}

} // namespace hermes 
