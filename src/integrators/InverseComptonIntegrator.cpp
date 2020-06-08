#include "hermes/integrators/InverseComptonIntegrator.h"

#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <thread>

#include "hermes/Common.h"
#include "hermes/Signals.h"
#include "hermes/integrators/LOSIntegrationMethods.h"

namespace hermes {

InverseComptonIntegrator::InverseComptonIntegrator(
    const std::shared_ptr<cosmicrays::CosmicRayDensity> &crdensity_,
    const std::shared_ptr<photonfields::PhotonField> &phdensity_,
    const std::shared_ptr<interactions::DifferentialCrossSection> &crossSec_)
    : GammaIntegratorTemplate("InverseCompton"),
      crdensity(crdensity_),
      phdensity(phdensity_),
      crossSec(crossSec_) {}

InverseComptonIntegrator::~InverseComptonIntegrator() {}

void InverseComptonIntegrator::computeCacheInThread(
    std::size_t start, std::size_t end, const QEnergy &Egamma,
    std::shared_ptr<ProgressBar> &p) {
	for (std::size_t i = start; i < end; ++i) {
		auto pos =
		    static_cast<Vector3QLength>(cacheTable->positionFromIndex(i));
		cacheTable->get(i) = this->integrateOverEnergy(pos, Egamma);
		p->update();
	}
}

void InverseComptonIntegrator::setupCacheTable(int N_x, int N_y, int N_z) {
	const QLength rBorder = 30_kpc;
	const QLength zBorder = 5_kpc;
	Vector3QLength spacing =
	    Vector3QLength(2 * rBorder / N_x, 2 * rBorder / N_y, 2 * zBorder / N_z);

	// setup table
	cacheTable = std::make_shared<ICCacheTable>(ICCacheTable(
	    Vector3QLength(-rBorder, -rBorder, -zBorder), N_x, N_y, N_z, spacing));
	cacheEnabled = true;
}

void InverseComptonIntegrator::initCacheTable() {
	if (!cacheEnabled) {
		std::cout << "hermes::Integrator::initCacheTable: No cache table "
		             "present: use setupCacheTable"
		          << std::endl;
		return;
	}

	if (cacheTableInitialized) cacheTableInitialized = false;

	std::cout << "hermes::Integrator::initCacheTable: Number of Threads: "
	          << getThreadsNumber() << std::endl;

	const QEnergy Egamma = skymapParameter;
	size_t grid_size = cacheTable->getGridSize();

	// Progressbar init
	auto progressbar = std::make_shared<ProgressBar>(ProgressBar(grid_size));
	auto progressbar_mutex = std::make_shared<std::mutex>();
	progressbar->setMutex(progressbar_mutex);
	progressbar->start("Generate Cache Table");

	auto job_chunks = getThreadChunks(grid_size);
	std::vector<std::thread> threads;
	threads.reserve(job_chunks.size());
	for (auto &c : job_chunks) {
		threads.push_back(
		    std::thread(&InverseComptonIntegrator::computeCacheInThread, this,
		                c.first, c.second, Egamma, std::ref(progressbar)));
	}
	for (auto &t : threads) {
		t.join();
	}

	cacheTableInitialized = true;
}

QGREmissivity InverseComptonIntegrator::getIOEfromCache(
    const Vector3QLength &pos_, const QEnergy &Egamma_) const {
	return cacheTable->interpolate(static_cast<Vector3d>(pos_));
}

QDiffIntensity InverseComptonIntegrator::integrateOverLOS(
    const QDirection &direction) const {
	return integrateOverLOS(direction, 1_GeV);
}

QDiffIntensity InverseComptonIntegrator::integrateOverLOS(
    const QDirection &direction_, const QEnergy &Egamma_) const {
	auto integrand = [this, direction_, Egamma_](const QLength &dist) {
		return this->integrateOverEnergy(
		    getGalacticPosition(getSunPosition(), dist, direction_), Egamma_);
	};

	return gslQAGIntegration<QDiffFlux, QGREmissivity>(
	           [integrand](QLength dist) { return integrand(dist); }, 0,
	           getMaxDistance(direction_), 500) /
	       (4_pi * 1_sr);
}

QGREmissivity InverseComptonIntegrator::integrateOverEnergy(
    const Vector3QLength &pos_, const QEnergy &Egamma_) const {
	if (cacheTableInitialized) return getIOEfromCache(pos_, Egamma_);

	if (crdensity->existsScaleFactor()) {
		return integrateOverLogEnergy(pos_, Egamma_);
	} else {
		return integrateOverSumEnergy(pos_, Egamma_);
	}
}

QGREmissivity InverseComptonIntegrator::integrateOverSumEnergy(
    const Vector3QLength &pos_, const QEnergy &Egamma_) const {
	QGREmissivity integral(0);
	QEnergy deltaE;

	for (auto itE = std::next(crdensity->begin()); itE != crdensity->end();
	     ++itE) {
		deltaE = (*itE) - *std::prev(itE);
		integral += integrateOverPhotonEnergy(pos_, Egamma_, (*itE)) *
		            crdensity->getDensityPerEnergy(*itE, pos_) * c_light *
		            deltaE;
	}

	return integral;
}

QGREmissivity InverseComptonIntegrator::integrateOverLogEnergy(
    const Vector3QLength &pos_, const QEnergy &Egamma_) const {
	QGREmissivity integral(0);

	for (auto itE = crdensity->begin(); itE != crdensity->end(); ++itE) {
		integral += integrateOverPhotonEnergy(pos_, Egamma_, (*itE)) *
		            crdensity->getDensityPerEnergy((*itE), pos_) * (*itE) *
		            c_light;
	}

	return integral * log(crdensity->getEnergyScaleFactor());
}

QICInnerIntegral InverseComptonIntegrator::integrateOverPhotonEnergy(
    const Vector3QLength &pos_, const QEnergy &Egamma_,
    const QEnergy &Eelectron_) const {
	QICInnerIntegral integral(0);

	auto integrand = [this, pos_, Egamma_, Eelectron_](
	                     const cosmicrays::CosmicRayDensity::iterator itE,
	                     const QEnergy &deltaE) {
		return crossSec->getDiffCrossSection(Eelectron_, (*itE), Egamma_) *
		       phdensity->getEnergyDensity(
		           pos_, static_cast<std::size_t>(itE - phdensity->begin())) /
		       pow<2>(*itE);
	};

	for (auto itE = std::next(phdensity->begin()); itE != phdensity->end();
	     ++itE) {
		auto itE_prev = std::prev(itE);
		QEnergy deltaE = (*itE) - (*itE_prev);
		QNumber xlog = log((*itE) / (*itE_prev));
		integral += (*itE) * integrand(itE, deltaE) * xlog;
	}

	return integral;
}

InverseComptonIntegrator::tLOSProfile
InverseComptonIntegrator::getLOSProfile(const QDirection &direction,
										const QEnergy &Egamma,
                                        int Nsteps) const {
	auto integrand = [this, direction, Egamma](const QLength &dist) {
		return this->integrateOverEnergy(
		    getGalacticPosition(getSunPosition(), dist, direction), Egamma);
	};

	QLength start = 0_m;
	QLength stop = getMaxDistance(direction);
	QLength delta_d = (stop - start) / Nsteps;

	tLOSProfile profile;

	for (QLength dist = start; dist <= stop; dist += delta_d) {
		profile.first.push_back(dist);
		profile.second.push_back(static_cast<double>(integrand(dist)));
	}

	return profile;
}

}  // namespace hermes
