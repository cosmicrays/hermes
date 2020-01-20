#include "hermes/integrators/PiZeroIntegrator.h"
#include "hermes/integrators/LOSIntegrationMethods.h"
#include "hermes/Common.h"

#include <memory>
#include <functional>

namespace hermes {

PiZeroIntegrator::PiZeroIntegrator(
	const std::shared_ptr<CosmicRayDensity> crdensity_,
	const std::shared_ptr<RingModelDensity> ngdensity_,
	const std::shared_ptr<DifferentialCrossSection> crossSec_) : 
	crdensity(crdensity_), ngdensity(ngdensity_), crossSec(crossSec_) {
}

PiZeroIntegrator::~PiZeroIntegrator() { }

QDifferentialFlux PiZeroIntegrator::integrateOverLOS(
		QDirection direction) const {
	return integrateOverLOS(direction, 1_GeV);
}

QDifferentialFlux PiZeroIntegrator::integrateOverLOS(
		QDirection direction_, QEnergy Egamma_) const {

	QDifferentialFlux tolerance = 1e9; // / (1_GeV * 1_cm2 * 1_s); // sr^-1 

	std::vector<QColumnDensity> normIntegrals(
			ngdensity->getRingNumber(GasType::HI), QColumnDensity(0.0));
	
	for (const auto &ring : *ngdensity) {
		auto normI_f = [ring, this](const Vector3QLength &pos)
			{ return (ring->isInside(pos)) ?
				this->densityProfile(pos) : 0;};

		auto integrand = [this, normI_f, direction_](const QLength &dist) {
	                return normI_f(getGalacticPosition(this->positionSun, dist, direction_)); };

		normIntegrals[ring->getIndex()] =
			simpsonIntegration<QColumnDensity, QPDensity>(integrand, 0, getMaxDistance(direction_), 150);
	}

	std::vector<QDifferentialFlux> losIntegrals(
			ngdensity->getRingNumber(GasType::HI), QDifferentialFlux(0.0));
	
	for (const auto &ring : *ngdensity) {
		auto losI_f = [ring, this](const Vector3QLength &pos, const QEnergy &Egamma_)
			{ return (ring->isInside(pos)) ?
				this->densityProfile(pos)*this->integrateOverEnergy(pos, Egamma_) : 0;};
		
		auto integrand = [this, losI_f, direction_, Egamma_](const QLength &dist) {
	                return losI_f(getGalacticPosition(this->positionSun, dist, direction_), Egamma_); };

		losIntegrals[ring->getIndex()] =
			simpsonIntegration<QDifferentialFlux, QICOuterIntegral>(integrand, 0, getMaxDistance(direction_), 200);
	}
	
	// normalize according to the ring density model	
	QDifferentialFlux total_diff_flux(0.0);
	for(const auto &ring : *ngdensity) {
		if(normIntegrals[ring->getIndex()] == QColumnDensity(0))
			continue;
		total_diff_flux +=
			(ring->getHIColumnDensity(direction_) +
			 2 * X0Function(Vector3QLength(0)) *
			 ring->getCOIntensity(direction_)) /
			 normIntegrals[ring->getIndex()] *
			 losIntegrals[ring->getIndex()];
	}
	
	return total_diff_flux;
}

QPDensity PiZeroIntegrator::densityProfile(const Vector3QLength &pos) const {
	return QPDensity(1);
}

QRingX0Unit PiZeroIntegrator::X0Function(const Vector3QLength &pos) const {
	return 1.8e20 / (1_cm2*1_K*1_km) * 1_s;
}

QPiZeroIntegral PiZeroIntegrator::integrateOverEnergy(Vector3QLength pos_, QEnergy Egamma_) const {
		if (crdensity->existsScaleFactor())
			return integrateOverLogEnergy(pos_, Egamma_);
		else
			return integrateOverSumEnergy(pos_, Egamma_);
}

QPiZeroIntegral PiZeroIntegrator::integrateOverSumEnergy(Vector3QLength pos_, QEnergy Egamma_) const {

	QPiZeroIntegral integral(0);
	QEnergy deltaE;

	// TODO: optimization - E_min = E_gamma + m_pi^2c^4/(4E_gamma) 
	for (auto itE = std::next(crdensity->begin()); itE != crdensity->end(); ++itE) {
		deltaE = (*itE) - *std::prev(itE);
		integral += crossSec->getDiffCrossSection(*itE, Egamma_) *
                        	crdensity->getDensityPerEnergy(*itE, pos_) * deltaE * c_light / 4_pi;
        }
	
	return integral;
}

QPiZeroIntegral PiZeroIntegrator::integrateOverLogEnergy(Vector3QLength pos_, QEnergy Egamma_) const {

	QPiZeroIntegral integral(0);

	// TODO: optimization - E_min = E_gamma + m_pi^2c^4/(4E_gamma) 
	for (auto itE = crdensity->begin(); itE != crdensity->end(); ++itE) {
		integral += crossSec->getDiffCrossSection(*itE, Egamma_) * 
	                        crdensity->getDensityPerEnergy(*itE, pos_) * (*itE) * c_light / 4_pi;
        }

	return integral * log(crdensity->getEnergyScaleFactor());
}

} // namespace hermes 
