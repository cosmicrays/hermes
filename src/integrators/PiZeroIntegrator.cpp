#include "hermes/integrators/PiZeroIntegrator.h"
#include "hermes/Common.h"
#include "hermes/integrators/LOSIntegrationMethods.h"

#include <algorithm>
#include <functional>
#include <iterator>
#include <memory>
#include <mutex>
#include <numeric>
#include <thread>

namespace hermes {

PiZeroIntegrator::PiZeroIntegrator(
    const std::shared_ptr<cosmicrays::CosmicRayDensity> crDensity_,
    const std::shared_ptr<neutralgas::RingModel> ngdensity_,
    const std::shared_ptr<interactions::DifferentialCrossSection> crossSec_)
    : GammaIntegratorTemplate(),
      crList(std::vector<std::shared_ptr<cosmicrays::CosmicRayDensity>>{
	  crDensity_}),
      ngdensity(ngdensity_), crossSec(crossSec_) {}

PiZeroIntegrator::PiZeroIntegrator(
    const std::vector<std::shared_ptr<cosmicrays::CosmicRayDensity>> crList_,
    const std::shared_ptr<neutralgas::RingModel> ngdensity_,
    const std::shared_ptr<interactions::DifferentialCrossSection> crossSec_)
    : GammaIntegratorTemplate(), crList(crList_), ngdensity(ngdensity_),
      crossSec(crossSec_) {}

PiZeroIntegrator::~PiZeroIntegrator() {}

void PiZeroIntegrator::computeCacheInThread(std::size_t start, std::size_t end,
					    const QEnergy &Egamma,
					    std::shared_ptr<ProgressBar> &p) {
    for (std::size_t i = start; i < end; ++i) {
	auto pos =
	    static_cast<Vector3QLength>(cacheTable->positionFromIndex(i));
	cacheTable->get(i) = this->integrateOverEnergy(pos, Egamma);
	p->update();
    }
}

void PiZeroIntegrator::setupCacheTable(int N_x, int N_y, int N_z) {

    const QLength rBorder = 30_kpc;
    const QLength zBorder = 5_kpc;
    Vector3QLength spacing =
	Vector3QLength(2 * rBorder / N_x, 2 * rBorder / N_y, 2 * zBorder / N_z);

    // setup table
    cacheTable = std::make_shared<ICCacheTable>(ICCacheTable(
	Vector3QLength(-rBorder, -rBorder, -zBorder), N_x, N_y, N_z, spacing));
    cacheEnabled = true;
}

void PiZeroIntegrator::initCacheTable() {

    if (!cacheEnabled) {
	std::cout << "hermes::Integrator::initCacheTable: No cache table "
		     "present: use setupCacheTable"
		  << std::endl;
	return;
    }

    if (cacheTableInitialized)
	cacheTableInitialized = false;

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
    for (auto &c : job_chunks) {
	threads.push_back(std::thread(&PiZeroIntegrator::computeCacheInThread,
				      this, c.first, c.second, Egamma,
				      std::ref(progressbar)));
    }
    for (auto &t : threads) {
	t.join();
    }

    cacheTableInitialized = true;
}

QPiZeroIntegral PiZeroIntegrator::getIOEfromCache(Vector3QLength pos_,
						  QEnergy Egamma_) const {
    return cacheTable->interpolate(static_cast<Vector3d>(pos_));
}

QDiffIntensity PiZeroIntegrator::integrateOverLOS(QDirection direction) const {
    return integrateOverLOS(direction, 1_GeV);
}

QDiffIntensity PiZeroIntegrator::integrateOverLOS(QDirection direction_,
						  QEnergy Egamma_) const {

    QDiffIntensity total_diff_flux(0.0);

    // Sum over rings
    for (const auto &ring : *ngdensity) {
	// TODO(adundovi): this could be better
	if (!ngdensity->isRingEnabled(ring->getIndex()))
	    continue;

	// Normalization-part
	auto normI_f = [ring, this](const Vector3QLength &pos) {
	    return (ring->isInside(pos)) ? this->densityProfile(pos) : 0;
	};
	auto normIntegrand = [this, normI_f, direction_](const QLength &dist) {
	    return normI_f(
		getGalacticPosition(this->positionSun, dist, direction_));
	};
	QColumnDensity normIntegrals =
	    gslQAGIntegration<QColumnDensity, QPDensity>(
		normIntegrand, 0, getMaxDistance(direction_), 500);

	// LOS is not crossing the current ring at all
	if (normIntegrals == QColumnDensity(0))
	    continue;

	// Integral over emissivity
	auto losI_f = [ring, this](const Vector3QLength &pos,
				   const QEnergy &Egamma_) {
	    return (ring->isInside(pos))
		       ? this->densityProfile(pos) *
			     this->integrateOverEnergy(pos, Egamma_)
		       : 0;
	};
	auto losIntegrand = [this, losI_f, direction_,
			     Egamma_](const QLength &dist) {
	    return losI_f(
		getGalacticPosition(this->positionSun, dist, direction_),
		Egamma_);
	};
	QDiffIntensity losIntegrals =
	    simpsonIntegration<QDiffFlux, QGREmissivity>(
		losIntegrand, 0, getMaxDistance(direction_), 500) /
	    (4_pi * 1_sr);

	// Finally, normalize LOS integrals, separatelly for HI and CO
	if (ngdensity->getRingType() == neutralgas::RingType::HI) {
	    total_diff_flux += (ring->getHIColumnDensity(direction_)) /
			       normIntegrals * losIntegrals;
	}
	if (ngdensity->getRingType() == neutralgas::RingType::CO) {
	    total_diff_flux += (2 * X0Function(Vector3QLength(0)) *
				ring->getCOIntensity(direction_)) /
			       normIntegrals * losIntegrals;
	}
    }

    return total_diff_flux;
}

QPDensity PiZeroIntegrator::densityProfile(const Vector3QLength &pos) const {
    return QPDensity(1);
}

QRingX0Unit PiZeroIntegrator::X0Function(const Vector3QLength &pos) const {
    return 1.8e20 / (1_cm2 * 1_K * 1_km) * 1_s;
}

QPiZeroIntegral PiZeroIntegrator::integrateOverEnergy(Vector3QLength pos_,
						      QEnergy Egamma_) const {
    if (cacheTableInitialized)
	return getIOEfromCache(pos_, Egamma_);

    QPiZeroIntegral total(0);

    // TODO(adundovi): micro-optimization - E_min = E_gamma +
    // m_pi^2c^4/(4E_gamma)
    std::vector<QDiffCrossSection> diffCrossSectionVector;
    std::transform(crList[0]->beginAfterEnergy(Egamma_), crList[0]->end(),
		   std::back_inserter(diffCrossSectionVector),
		   [this, Egamma_](const QEnergy &E) -> QDiffCrossSection {
		       return crossSec->getDiffCrossSection(E, Egamma_);
		   });

    for (const auto &crDensity : crList) {
	auto pid_projectile = crDensity->getPID();

	std::vector<QPDensity> cosmicRayVector;
	std::transform(crDensity->beginAfterEnergy(Egamma_), crDensity->end(),
		       std::back_inserter(cosmicRayVector),
		       [crDensity, pos_](const QEnergy &E) -> QPDensity {
			   return crDensity->getDensityPerEnergy(E, pos_) * E;
		       });

	std::vector<QPiZeroIntegral> integral;
	std::transform(cosmicRayVector.begin(), cosmicRayVector.end(),
		       diffCrossSectionVector.begin(),
		       std::back_inserter(integral),
		       [](const QPDensity &a, const QDiffCrossSection &b) {
			   return a * b * c_light;
		       });
	// log-integration
	auto integralOverEnergy =
	    std::log(crDensity->getEnergyScaleFactor()) *
	    std::accumulate(integral.begin(), integral.end(),
			    QPiZeroIntegral(0));

	for (const auto &neutralGas : ngdensity->getAbundanceFractions()) {
	    auto pid_target = neutralGas.first;
	    auto f_target = neutralGas.second;
	    total += f_target * crossSec->getSigma(pid_projectile, pid_target) *
		     integralOverEnergy;
	}
    }
    return total;
}

} // namespace hermes
