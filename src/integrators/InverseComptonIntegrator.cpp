#include "hermes/integrators/InverseComptonIntegrator.h"
#include "hermes/integrators/LOSIntegrationMethods.h"
#include "hermes/Common.h"
#include "hermes/ProgressBar.h"
#include "hermes/Signals.h"

#include <memory>
#include <functional>
#include <stdexcept>
#include <iostream>

#if _OPENMP
#include <omp.h>
#define OMP_SCHEDULE static,100
#endif

namespace hermes {

InverseComptonIntegrator::InverseComptonIntegrator(
	const std::shared_ptr<CosmicRayDensity> crdensity_,
	const std::shared_ptr<PhotonField> phdensity_,
	const std::shared_ptr<KleinNishina> crossSec_) : 
	crdensity(crdensity_), phdensity(phdensity_), crossSec(crossSec_), cacheStoragePresent(false) {
}

InverseComptonIntegrator::~InverseComptonIntegrator() { }

void InverseComptonIntegrator::initCacheTable(QEnergy Egamma, int N_x, int N_y, int N_z) {
	
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

	g_cancel_signal_flag = 0;
        sighandler_t old_sigint_handler = std::signal(SIGINT,
                        g_cancel_signal_callback);
        sighandler_t old_sigterm_handler = std::signal(SIGTERM,
                        g_cancel_signal_callback);

#pragma omp parallel for schedule(OMP_SCHEDULE)
	for (size_t index = 0; index < grid_size; ++index) {
		Vector3QLength pos = static_cast<Vector3QLength>(cacheTable->positionFromIndex(index));
		cacheTable->get(index) = integrateOverEnergy(pos, Egamma);
#pragma omp critical(progressbarUpdate)
		progressbar.update();
	}

	// set flag
	cacheStoragePresent = true;
	
	std::signal(SIGINT, old_sigint_handler);
        std::signal(SIGTERM, old_sigterm_handler);
        // Propagate signal to old handler.
        if (g_cancel_signal_flag > 0)
                raise(g_cancel_signal_flag);
	
}

bool InverseComptonIntegrator::isCacheTableEnabled() const {
	return cacheStoragePresent;
}

QICOuterIntegral InverseComptonIntegrator::getIOEfromCache(Vector3QLength pos_, QEnergy Egamma_) const {
		return cacheTable->interpolate(static_cast<Vector3d>(pos_));
}

QDifferentialIntensity InverseComptonIntegrator::integrateOverLOS(
		QDirection direction) const {
	return integrateOverLOS(direction, 1_GeV);
}

QDifferentialIntensity InverseComptonIntegrator::integrateOverLOS(
		QDirection direction_, QEnergy Egamma_) const {
	
	auto integrand = [this, direction_, Egamma_](const QLength &dist) {
		return this->integrateOverEnergy(
				getGalacticPosition(this->positionSun, dist, direction_),
				Egamma_
			); };

	return simpsonIntegration<QDifferentialFlux, QICOuterIntegral>(
			[integrand](QLength dist) {return integrand(dist);}, 0, getMaxDistance(direction_), 500) / (4_pi*1_sr);
}

QICOuterIntegral InverseComptonIntegrator::integrateOverEnergy(Vector3QLength pos_, QEnergy Egamma_) const {
	if (cacheStoragePresent)
		return getIOEfromCache(pos_, Egamma_);

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
			crdensity->getDensityPerEnergy(*itE, pos_) * c_light * deltaE;
        }
	
	return integral;
}

QICOuterIntegral InverseComptonIntegrator::integrateOverLogEnergy(Vector3QLength pos_, QEnergy Egamma_) const {

	QICOuterIntegral integral(0);

	for (auto itE = crdensity->begin(); itE != crdensity->end(); ++itE) {
		integral += integrateOverPhotonEnergy(pos_, Egamma_, (*itE)) * 
	                        crdensity->getDensityPerEnergy((*itE), pos_) * (*itE) * c_light;
        }

	return integral * log(crdensity->getEnergyScaleFactor());
}

QICInnerIntegral InverseComptonIntegrator::integrateOverPhotonEnergy(
		Vector3QLength pos_, QEnergy Egamma_, QEnergy Eelectron_) const {
	
	QICInnerIntegral integral(0);
	
	for (auto itE = phdensity->begin(); itE != phdensity->end(); ++itE) {
		integral += crossSec->getDiffCrossSection(Eelectron_, (*itE), Egamma_) *
				phdensity->getEnergyDensity(pos_, static_cast<int>(itE - phdensity->begin())) / (*itE);
	}

	return integral * log(phdensity->getEnergyScaleFactor());
}

} // namespace hermes 
