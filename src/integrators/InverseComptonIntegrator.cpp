#include "hermes/integrators/InverseComptonIntegrator.h"
#include "hermes/integrators/LOSIntegrationMethods.h"
#include "hermes/Common.h"

#include <memory>
#include <functional>

namespace hermes {

InverseComptonIntegrator::InverseComptonIntegrator(
	const std::shared_ptr<CosmicRayDensity> crdensity_,
	const std::shared_ptr<PhotonField> phdensity_,
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
	
	//QDifferentialFlux tolerance = 1e10; // / (1_GeV * 1_cm2 * 1_s); // sr^-1 

	return simpsonIntegration<QDifferentialFlux, QICOuterIntegral, QEnergy>(
			direction_,
			[this](Vector3QLength pos, QEnergy Egamma) {return this->integrateOverEnergy(pos, Egamma);},
			Egamma_);
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
	static const double scaling = phdensity->getEnergyScaleFactor();
	static const QEnergy E_start = phdensity->getStartEnergy();
	static const QEnergy E_end = phdensity->getEndEnergy();
	
	for (QEnergy E_ph = E_start; E_ph < E_end; E_ph = E_ph*scaling) {
		integral += crossSec->getDiffCrossSection(Eelectron_, E_ph, Egamma_) *
				phdensity->getEnergyDensity(pos_, E_ph) / E_ph;
	}

	return integral * log(scaling);
}

} // namespace hermes 
