#include "hermes/integrators/PiZeroIntegrator.h"
#include "hermes/integrators/LOSIntegrationMethods.h"
#include "hermes/ProgressBar.h"
#include "hermes/Common.h"

#include <memory>
#include <functional>

#if _OPENMP
#include <omp.h>
#define OMP_SCHEDULE static,100
#endif

namespace hermes {

PiZeroIntegrator::PiZeroIntegrator(
	const std::shared_ptr<CosmicRayDensity> crdensity_,
	const std::shared_ptr<RingModelDensity> ngdensity_,
	const std::shared_ptr<DifferentialCrossSection> crossSec_) : 
	GammaIntegratorTemplate(), crdensity(crdensity_), ngdensity(ngdensity_), crossSec(crossSec_) {
}

PiZeroIntegrator::~PiZeroIntegrator() { }

void PiZeroIntegrator::initCacheTable(QEnergy Egamma, int N_x, int N_y, int N_z) {
	
	if (cacheStoragePresent)
		cacheStoragePresent = false;
	
	const QLength rBorder = 20_kpc;
        const QLength zBorder = 5_kpc;
	Vector3QLength spacing = Vector3QLength(
			2*rBorder / N_x,
			2*rBorder / N_y,
			2*zBorder / N_z);
	
	// init table
	cacheTable = std::make_shared<ICCacheTable>(ICCacheTable(
					Vector3QLength(-rBorder, -rBorder, -zBorder),
					N_x, N_y, N_z, spacing));
	
#if _OPENMP
	std::cout << "hermes::Integrator::InitCacheTable: Number of Threads: " << omp_get_max_threads() << std::endl;
#endif
	
	size_t grid_size = cacheTable->getGridSize();

	ProgressBar progressbar(grid_size);
	progressbar.start("Generate Cache Table");

#pragma omp parallel for schedule(OMP_SCHEDULE)
	for (size_t index = 0; index < grid_size; ++index) {
		Vector3QLength pos = static_cast<Vector3QLength>(cacheTable->positionFromIndex(index));
		cacheTable->get(index) = integrateOverEnergy(pos, Egamma);
#pragma omp critical(progressbarUpdate)
		progressbar.update();
	}

	cacheStoragePresent = true;
}

QPiZeroIntegral PiZeroIntegrator::getIOEfromCache(Vector3QLength pos_, QEnergy Egamma_) const {
		return cacheTable->interpolate(static_cast<Vector3d>(pos_));
}


QDifferentialIntensity PiZeroIntegrator::integrateOverLOS(
		QDirection direction) const {
	return integrateOverLOS(direction, 1_GeV);
}

QDifferentialIntensity PiZeroIntegrator::integrateOverLOS(
		QDirection direction_, QEnergy Egamma_) const {
	
	QDifferentialIntensity total_diff_flux(0.0);

	// Sum over rings	
	for (const auto &ring : *ngdensity) {

		// Debug:
		int ring_number = ring->getIndex();

		// Normalization-part	
		auto normI_f = [ring, this](const Vector3QLength &pos)
			{ return (ring->isInside(pos)) ?
				this->densityProfile(pos) : 0;};

		auto normIntegrand = [this, normI_f, direction_](const QLength &dist) {
	                return normI_f(getGalacticPosition(this->positionSun, dist, direction_)); };

		QColumnDensity normIntegrals = 
			gslQAGIntegration<QColumnDensity, QPDensity>(normIntegrand, 0, getMaxDistance(direction_), 500);
		
		// LOS is not crossing the current ring
		if(normIntegrals == QColumnDensity(0))
			continue;

		// Integral over emissivity
		auto losI_f = [ring, this](const Vector3QLength &pos, const QEnergy &Egamma_)
			{ return (ring->isInside(pos)) ?
				this->densityProfile(pos) * this->integrateOverEnergy(pos, Egamma_) : 0;};
//				this->densityProfile(pos) * QPiZeroIntegral(1.0) : 0;};
		
		auto losIntegrand = [this, losI_f, direction_, Egamma_](const QLength &dist) {
	                return losI_f(getGalacticPosition(this->positionSun, dist, direction_), Egamma_); };

		QDifferentialIntensity losIntegrals = 
			simpsonIntegration<QDifferentialFlux, QICOuterIntegral>(losIntegrand, 0, getMaxDistance(direction_), 500) / (4_pi*1_sr);
	
		// Finally, normalize LOS integrals	
		total_diff_flux += (ring->getHIColumnDensity(direction_) + 2 * X0Function(Vector3QLength(0)) *
					 ring->getCOIntensity(direction_)) / normIntegrals * losIntegrals;
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
	if (cacheStoragePresent)
		return getIOEfromCache(pos_, Egamma_);

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
                        	crdensity->getDensityPerEnergy(*itE, pos_) * deltaE * c_light;
        }
	
	return integral;
}

QPiZeroIntegral PiZeroIntegrator::integrateOverLogEnergy(Vector3QLength pos_, QEnergy Egamma_) const {

	QPiZeroIntegral integral(0);

	// TODO: optimization - E_min = E_gamma + m_pi^2c^4/(4E_gamma) 
	for (auto itE = crdensity->begin(); itE != crdensity->end(); ++itE) {
		integral += crossSec->getDiffCrossSection(*itE, Egamma_) * 
	                        crdensity->getDensityPerEnergy(*itE, pos_) * (*itE) * c_light;
        }

	return integral * log(crdensity->getEnergyScaleFactor());
}

} // namespace hermes 
