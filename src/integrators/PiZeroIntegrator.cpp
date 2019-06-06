#include "hermes/integrators/PiZeroIntegrator.h"
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

	Vector3QLength positionSun(8.5_kpc, 0, 0);
	Vector3QLength pos(0.0);
	QDifferentialFlux total_diff_flux(0.0);
	QLength delta_d = 10.0_pc;


	std::vector<QColumnDensity> normIntegrals(
			ngdensity->getRingNumber(GasType::HI), QColumnDensity(0.0));
	std::vector<QDifferentialFlux> losIntegrals(
			ngdensity->getRingNumber(GasType::HI), QDifferentialFlux(0.0));

	//std::function<QPDensity(const QLength &r)> profile;
	//profile = simpleDensityDistribution;
	// distance from the (spherical) galactic border in the given direction
	QLength maxDistance = distanceToGalBorder(positionSun, direction_);
	
	for(QLength dist = 0; dist <= maxDistance; dist += delta_d) {
		pos = getGalacticPosition(positionSun, dist, direction_);

		for (const auto &ring : *ngdensity) {
			if(ring->isInside(pos)) {
				auto i = ring->getIndex();
				normIntegrals[i] += densityProfile(pos) * delta_d;
				losIntegrals[i] += densityProfile(pos) *
					integrateOverEnergy(pos, Egamma_) * delta_d;
			}

		} 
	}

	// normalize according to the ring density model	
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
