#include "hermes/integrators/PiZeroIntegrator.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <iterator>
#include <limits>
#include <memory>
#include <mutex>
#include <numeric>
#include <stdexcept>
#include <thread>

#include "hermes/Common.h"

namespace hermes {

namespace {

struct RingLOSIntegrals {
	QColumnDensity normalization;
	QDiffFlux emissivity;
};

RingLOSIntegrals integrateRingSegment(const std::function<std::pair<QPDensity, QGREmissivity>(QLength)> &integrand,
                                      const QLength &start, const QLength &stop, unsigned int steps) {
	if (steps < 2 || steps % 2 != 0)
		throw std::invalid_argument("integrateRingSegment: steps must be positive and even");
	if (stop <= start) return {QColumnDensity(0), QDiffFlux(0)};

	const QLength spacing = (stop - start) / steps;
	QPDensity normalizationSum(0);
	QGREmissivity emissivitySum(0);

	for (unsigned int i = 0; i <= steps; ++i) {
		const auto values = integrand(start + i * spacing);
		const double weight = (i == 0 || i == steps) ? 1.0 : ((i % 2 == 0) ? 2.0 : 4.0);
		normalizationSum += weight * values.first;
		emissivitySum += weight * values.second;
	}

	return {spacing * normalizationSum / 3.0, spacing * emissivitySum / 3.0};
}

}  // namespace

PiZeroIntegrator::PiZeroIntegrator(const std::shared_ptr<cosmicrays::CosmicRayDensity> &crDensity_,
                                   const std::shared_ptr<neutralgas::RingModel> &ngdensity_,
                                   const std::shared_ptr<interactions::DifferentialCrossSection> &crossSec_)
    : GammaIntegratorTemplate("PiZero"),
      crList(std::vector<std::shared_ptr<cosmicrays::CosmicRayDensity>>{crDensity_}),
      ngdensity(ngdensity_),
      crossSec(crossSec_),
      dProfile(std::make_unique<neutralgas::Nakanishi06>()),
      losIntegrationSteps(DefaultLOSIntegrationSteps) {}

PiZeroIntegrator::PiZeroIntegrator(const std::vector<std::shared_ptr<cosmicrays::CosmicRayDensity>> &crList_,
                                   const std::shared_ptr<neutralgas::RingModel> &ngdensity_,
                                   const std::shared_ptr<interactions::DifferentialCrossSection> &crossSec_)
    : GammaIntegratorTemplate("PiZero"),
      crList(crList_),
      ngdensity(ngdensity_),
      crossSec(crossSec_),
      dProfile(std::make_shared<neutralgas::Nakanishi06>()),
      losIntegrationSteps(DefaultLOSIntegrationSteps) {}

PiZeroIntegrator::~PiZeroIntegrator() {}

void PiZeroIntegrator::setLOSIntegrationSteps(unsigned int steps) {
	if (steps < 2) throw std::invalid_argument("PiZeroIntegrator: LOS integration steps must be at least 2");
	losIntegrationSteps = steps;
}

unsigned int PiZeroIntegrator::getLOSIntegrationSteps() const { return losIntegrationSteps; }

std::vector<PiZeroIntegrator::LOSSegment> PiZeroIntegrator::getRingLOSIntervals(const neutralgas::Ring &ring,
                                                                                const QDirection &direction) const {
	const auto boundaries = ring.getBoundaries();
	const double innerRadius = static_cast<double>(boundaries.first / 1_kpc);
	const double outerRadius = static_cast<double>(boundaries.second / 1_kpc);
	const double maxDistance = static_cast<double>(getMaxDistance(direction) / 1_kpc);

	if (!std::isfinite(innerRadius) || !std::isfinite(outerRadius) || innerRadius < 0.0 || outerRadius <= innerRadius)
		throw std::runtime_error("PiZeroIntegrator: invalid gas-ring boundaries");
	if (!std::isfinite(maxDistance) || maxDistance < 0.0)
		throw std::runtime_error("PiZeroIntegrator: invalid LOS integration limit");
	if (maxDistance == 0.0) return {};

	const double theta = static_cast<double>(direction[0] / 1_rad);
	const double phi = static_cast<double>(direction[1] / 1_rad);
	const double rayX = -std::sin(theta) * std::cos(phi);
	const double rayY = -std::sin(theta) * std::sin(phi);
	const double observerX = static_cast<double>(observerPosition.x / 1_kpc);
	const double observerY = static_cast<double>(observerPosition.y / 1_kpc);
	const double quadraticA = rayX * rayX + rayY * rayY;
	const double quadraticB = 2.0 * (observerX * rayX + observerY * rayY);

	std::vector<double> breakpoints = {0.0, maxDistance};
	auto appendCylinderIntersections = [&](double radius) {
		if (quadraticA <= 64.0 * std::numeric_limits<double>::epsilon()) return;

		const double quadraticC = observerX * observerX + observerY * observerY - radius * radius;
		double discriminant = quadraticB * quadraticB - 4.0 * quadraticA * quadraticC;
		const double discriminantScale = quadraticB * quadraticB + std::fabs(4.0 * quadraticA * quadraticC) + 1.0;
		const double discriminantTolerance = 64.0 * std::numeric_limits<double>::epsilon() * discriminantScale;
		if (discriminant < -discriminantTolerance) return;
		if (discriminant < 0.0) discriminant = 0.0;

		const double rootOffset = std::sqrt(discriminant);
		for (const double root :
		     {(-quadraticB - rootOffset) / (2.0 * quadraticA), (-quadraticB + rootOffset) / (2.0 * quadraticA)}) {
			if (root > 0.0 && root < maxDistance) breakpoints.push_back(root);
		}
	};

	appendCylinderIntersections(innerRadius);
	appendCylinderIntersections(outerRadius);
	std::sort(breakpoints.begin(), breakpoints.end());

	const double distanceTolerance = 64.0 * std::numeric_limits<double>::epsilon() * std::max(1.0, maxDistance);
	breakpoints.erase(std::unique(breakpoints.begin(), breakpoints.end(),
	                              [&](double a, double b) { return std::fabs(a - b) <= distanceTolerance; }),
	                  breakpoints.end());

	std::vector<LOSSegment> intervals;
	for (std::size_t i = 1; i < breakpoints.size(); ++i) {
		const double start = breakpoints[i - 1];
		const double stop = breakpoints[i];
		if (stop - start <= distanceTolerance) continue;

		const double midpoint = 0.5 * (start + stop);
		const double x = observerX + midpoint * rayX;
		const double y = observerY + midpoint * rayY;
		const double radiusSquared = x * x + y * y;
		if (radiusSquared > innerRadius * innerRadius && radiusSquared < outerRadius * outerRadius)
			intervals.emplace_back(start * 1_kpc, stop * 1_kpc);
	}

	return intervals;
}

void PiZeroIntegrator::setupCacheTable(int N_x, int N_y, int N_z) {
	const QLength rBorder = 35_kpc;
	const QLength zBorder = 5_kpc;
	Vector3QLength spacing = Vector3QLength(2 * rBorder / N_x, 2 * rBorder / N_y, 2 * zBorder / N_z);

	// setup table
	cacheTable = std::make_shared<tCacheTable>(
	    tCacheTable(Vector3QLength(-rBorder, -rBorder, -zBorder), N_x, N_y, N_z, spacing));
	cacheEnabled = true;
}

void PiZeroIntegrator::computeCacheInThread(std::size_t start, std::size_t end, const QEnergy &Egamma,
                                            std::shared_ptr<ProgressBar> &p) {
	for (std::size_t i = start; i < end; ++i) {
		auto pos = static_cast<Vector3QLength>(cacheTable->positionFromIndex(i));
		// TODO: remove std::cerr << "pos = " << pos.x / 1_kpc << ", " << pos.y
		// / 1_kpc << ", " << pos.z / 1_kpc << std::endl;
		cacheTable->get(i) = this->integrateOverEnergy(pos, Egamma);
		p->update();
	}
}

void PiZeroIntegrator::initCacheTable() {
	cacheTableInitialized = false;

	if (!cacheEnabled) {
		std::cout << "hermes::Integrator::initCacheTable: No cache table "
		             "present: use setupCacheTable"
		          << std::endl;
		return;
	}

	std::cout << "hermes::Integrator::initCacheTable: Number of Threads: " << getThreadsNumber() << std::endl;

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
		threads.push_back(std::thread(&PiZeroIntegrator::computeCacheInThread, this, c.first, c.second, Egamma,
		                              std::ref(progressbar)));
	}
	for (auto &t : threads) {
		t.join();
	}

	cacheTableInitialized = true;
}

QPiZeroIntegral PiZeroIntegrator::getIOEfromCache(const Vector3QLength &pos_, const QEnergy &Egamma_) const {
	return cacheTable->interpolate(static_cast<Vector3d>(pos_));
}

QDiffIntensity PiZeroIntegrator::integrateOverLOS(const QDirection &direction) const {
	return integrateOverLOS(direction, 1_GeV);
}

QDiffIntensity PiZeroIntegrator::integrateOverLOS(const QDirection &direction_, const QEnergy &Egamma_) const {
	return integrateOverLOSWithAttenuation(direction_, Egamma_, QInverseLength(0));
}

QDiffIntensity PiZeroIntegrator::integrateOverLOSWithAttenuation(
    const QDirection &direction_, const QEnergy &Egamma_, const QInverseLength &inverseAttenuationLength) const {
	const double inverseLengthPerKpc = static_cast<double>(inverseAttenuationLength * 1_kpc);
	if (!std::isfinite(inverseLengthPerKpc) || inverseAttenuationLength < QInverseLength(0))
		throw std::invalid_argument("PiZeroIntegrator: attenuation coefficient must be finite and non-negative");
	const bool attenuationEnabled = inverseAttenuationLength > QInverseLength(0);
	constexpr double attenuationCutoff = 40.0;
	constexpr double maximumOpticalDepthStep = 0.1;

	QDiffIntensity total_diff_flux(0.0);

	const auto gasType = ngdensity->getGasType();

	// Sum over rings
	for (const auto &ring : *ngdensity) {
		if (!ngdensity->isRingEnabled(ring->getIndex())) continue;

		const QColumnDensity ringColumnDensity = ring->getColumnDensity(direction_);
		if (ringColumnDensity == QColumnDensity(0)) continue;

		const auto intervals = getRingLOSIntervals(*ring, direction_);
		if (intervals.empty()) continue;

		double totalIntervalLength = 0.0;
		for (const auto &interval : intervals)
			totalIntervalLength += static_cast<double>((interval.second - interval.first) / 1_kpc);
		if (!(totalIntervalLength > 0.0) || !std::isfinite(totalIntervalLength))
			throw std::runtime_error("PiZeroIntegrator: invalid gas-ring LOS interval length");

		QColumnDensity normIntegral(0);
		QDiffFlux losIntegral(0);
		auto unattenuatedIntegrand = [this, gasType, direction_, Egamma_](const QLength &dist) {
			const auto position = getGalacticPosition(observerPosition, dist, direction_);
			const QPDensity profileDensity = dProfile->getPDensity(gasType, position);
			const QGREmissivity emissivity = profileDensity * integrateOverEnergy(position, Egamma_);
			return std::make_pair(profileDensity, emissivity);
		};
		auto normalizationIntegrand = [this, gasType, direction_](const QLength &dist) {
			const auto position = getGalacticPosition(observerPosition, dist, direction_);
			return std::make_pair(dProfile->getPDensity(gasType, position), QGREmissivity(0));
		};
		auto attenuatedIntegrand = [this, gasType, direction_, Egamma_, inverseAttenuationLength](
		                                 const QLength &dist) {
			const auto position = getGalacticPosition(observerPosition, dist, direction_);
			const QGREmissivity emissivity = dProfile->getPDensity(gasType, position) *
			                                integrateOverEnergy(position, Egamma_) *
			                                exp(-inverseAttenuationLength * dist);
			return std::make_pair(QPDensity(0), emissivity);
		};

		for (const auto &interval : intervals) {
			const double intervalLength = static_cast<double>((interval.second - interval.first) / 1_kpc);
			unsigned int intervalSteps =
			    static_cast<unsigned int>(std::ceil(losIntegrationSteps * intervalLength / totalIntervalLength));
			intervalSteps = std::max(2u, intervalSteps);
			if (intervalSteps % 2 != 0) ++intervalSteps;

			if (!attenuationEnabled) {
				const auto result =
				    integrateRingSegment(unattenuatedIntegrand, interval.first, interval.second, intervalSteps);
				normIntegral += result.normalization;
				losIntegral += result.emissivity;
				continue;
			}

			normIntegral +=
			    integrateRingSegment(normalizationIntegrand, interval.first, interval.second, intervalSteps).normalization;

			const double opticalDepthAtStart =
			    static_cast<double>(inverseAttenuationLength * interval.first);
			if (opticalDepthAtStart >= attenuationCutoff) continue;

			QLength activeStop = interval.second;
			const double opticalDepthAtStop = static_cast<double>(inverseAttenuationLength * interval.second);
			if (opticalDepthAtStop > attenuationCutoff)
				activeStop = attenuationCutoff / inverseAttenuationLength;
			if (activeStop <= interval.first) continue;

			const double activeLength = static_cast<double>((activeStop - interval.first) / 1_kpc);
			unsigned int activeSteps = static_cast<unsigned int>(
			    std::ceil(losIntegrationSteps * activeLength / totalIntervalLength));
			const double activeOpticalDepth =
			    static_cast<double>(inverseAttenuationLength * (activeStop - interval.first));
			const unsigned int opticalDepthSteps =
			    static_cast<unsigned int>(std::ceil(activeOpticalDepth / maximumOpticalDepthStep));
			activeSteps = std::max({2u, activeSteps, opticalDepthSteps});
			if (activeSteps % 2 != 0) ++activeSteps;

			losIntegral +=
			    integrateRingSegment(attenuatedIntegrand, interval.first, activeStop, activeSteps).emissivity;
		}

		const double normalizationValue = static_cast<double>(normIntegral);
		const double emissivityValue = static_cast<double>(losIntegral);
		if (!std::isfinite(normalizationValue) || !std::isfinite(emissivityValue))
			throw std::runtime_error("PiZeroIntegrator: non-finite LOS integral");
		if (normIntegral <= QColumnDensity(0)) continue;

		// The observed ring column fixes the normalization of the smooth
		// three-dimensional gas profile along this line of sight.
		total_diff_flux += ringColumnDensity / normIntegral * losIntegral / (4_pi * 1_sr);
	}

	return total_diff_flux;
}

QPiZeroIntegral PiZeroIntegrator::integrateOverEnergy(const Vector3QLength &pos_, const QEnergy &Egamma_) const {
	if (cacheTableInitialized) {
		return getIOEfromCache(pos_, Egamma_);
	}

	QPiZeroIntegral total(0);

	// TODO(adundovi): micro-optimization - E_min = E_gamma +
	// m_pi^2c^4/(4E_gamma)
	std::vector<QEnergy> energies;
	energies.insert(energies.begin(), crList[0]->beginAfterEnergy(Egamma_), crList[0]->end());

	for (const auto &crDensity : crList) {
		auto pid_projectile = crDensity->getPID();

		std::vector<QPDensity> cosmicRayVector;
		std::transform(
		    crDensity->beginAfterEnergy(Egamma_), crDensity->end(), std::back_inserter(cosmicRayVector),
		    [crDensity, pos_](const QEnergy &E) -> QPDensity { return crDensity->getDensityPerEnergy(E, pos_) * E; });

		std::vector<QPiZeroIntegral> integral;
		std::transform(cosmicRayVector.begin(), cosmicRayVector.end(), energies.begin(), std::back_inserter(integral),
		               [&](const QPDensity &n, const QEnergy &E) {
			               QPiZeroIntegral value(0);
			               for (const auto &neutralGas : ngdensity->getAbundanceFractions()) {
				               auto pid_target = neutralGas.first;
				               auto f_target = neutralGas.second;
				               value += c_light * n * f_target *
				                        crossSec->getDiffCrossSection(pid_projectile, pid_target, E, Egamma_);
			               }
			               return value;
		               });
		// log-integration
		total += std::log(crDensity->getEnergyScaleFactor()) *
		         std::accumulate(integral.begin(), integral.end(), QPiZeroIntegral(0));
	}
	return total;
}

}  // namespace hermes
