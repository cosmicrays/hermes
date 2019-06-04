#include "hermes/integrators/InverseComptonIntegrator.h"
#include "hermes/Common.h"

#include <memory>
#include <functional>

namespace hermes {

InverseComptonIntegrator::InverseComptonIntegrator(
	const std::shared_ptr<CosmicRayDensity> crdensity_,
	const std::shared_ptr<ISRF> phdensity_,
	const std::shared_ptr<KleinNishina> crossSec_) : 
	crdensity(crdensity_), phdensity(phdensity_), crossSec(crossSec_) {
}

InverseComptonIntegrator::~InverseComptonIntegrator() { }

QDifferentialFlux InverseComptonIntegrator::integrateOverLOS(
		QDirection direction) const {
	return integrateOverLOS(direction, 1_GeV);
}

QDifferentialFlux InverseComptonIntegrator::integrateOverLOS(
		QDirection direction_, QEnergy Egamma_) const {

	Vector3QLength positionSun(8.5_kpc, 0, 0);
	Vector3QLength pos(0.0);
	QDifferentialFlux total_diff_flux(0.0);
	QLength delta_d = 10.0_pc;

	// distance from the (spherical) galactic border in the given direction
	QLength maxDistance = distanceToGalBorder(positionSun, direction_);
	
	for(QLength dist = 0; dist <= maxDistance; dist += delta_d) {
		pos = getGalacticPosition(positionSun, dist, direction_);
		total_diff_flux += integrateOverEnergy(pos, Egamma_) * delta_d;
	}
	
	return total_diff_flux;
}

QICOuterIntegral InverseComptonIntegrator::integrateOverEnergy(Vector3QLength pos_, QEnergy Egamma_) const {
		if (crdensity->existsScaleFactor())
			return integrateOverLogEnergy(pos_, Egamma_);
		else
			return integrateOverSumEnergy(pos_, Egamma_);
}

QICOuterIntegral InverseComptonIntegrator::integrateOverSumEnergy(Vector3QLength pos_, QEnergy Egamma_) const {

	QICOuterIntegral integral(0);
	QEnergy deltaE;

	for (auto itE = std::next(crdensity->begin()); itE != crdensity->end(); ++itE) {
		deltaE = (*itE) - *std::prev(itE);
		integral += integrateOverPhotonEnergy(pos_, Egamma_, (*itE)) *
			crdensity->getDensityPerEnergy(*itE, pos_) * c_light / 4_pi * deltaE;
        }
	
	return integral;
}

QICOuterIntegral InverseComptonIntegrator::integrateOverLogEnergy(Vector3QLength pos_, QEnergy Egamma_) const {

	QICOuterIntegral integral(0);

	for (auto itE = crdensity->begin(); itE != crdensity->end(); ++itE) {
		integral += integrateOverPhotonEnergy(pos_, Egamma_, (*itE)) * 
	                        crdensity->getDensityPerEnergy(*itE, pos_) * (*itE) * c_light / 4_pi;
        }

	return integral * log(crdensity->getEnergyScaleFactor());
}

QICInnerIntegral InverseComptonIntegrator::integrateOverPhotonEnergy(
		Vector3QLength pos_, QEnergy Egamma_, QEnergy Eelectron_) const {
	
	QICInnerIntegral integral(0);
	double scaling = 1.1;
	
	for (QEnergy E_ph = 1e10_Hz * h_planck; E_ph < 1e16_Hz * h_planck; E_ph = E_ph*scaling) {
		integral += crossSec->getDiffCrossSection(Eelectron_, E_ph, Egamma_) *
				phdensity->getEnergyDensity(pos_, E_ph) / E_ph;
	}
	
	return integral * log(scaling);
}

} // namespace hermes 
