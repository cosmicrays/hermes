#include "hermes/integrators/PiZeroNeutronDecayIntegrator.h"

#include <algorithm>
#include <functional>
#include <iterator>
#include <memory>
#include <mutex>
#include <numeric>
#include <thread>
#include <utility>

#include "hermes/Common.h"
#include "hermes/integrators/LOSIntegrationMethods.h"

namespace hermes {

PiZeroNeutronDecayIntegrator::PiZeroNeutronDecayIntegrator(
    const std::shared_ptr<cosmicrays::CosmicRayDensity> &crDensity_,
    const std::shared_ptr<neutralgas::RingModel> &ngdensity_,
    const std::shared_ptr<interactions::DifferentialCrossSection> &crossSec_)
    : PiZeroIntegrator(crDensity_, ngdensity_, crossSec_) {
	setDescription("PiZeroNeutronDecayIntegrator");
}

PiZeroNeutronDecayIntegrator::PiZeroNeutronDecayIntegrator(
    const std::vector<std::shared_ptr<cosmicrays::CosmicRayDensity>> &crList_,
    const std::shared_ptr<neutralgas::RingModel> &ngdensity_,
    const std::shared_ptr<interactions::DifferentialCrossSection> &crossSec_)
    : PiZeroIntegrator(crList_, ngdensity_, crossSec_) {
	setDescription("PiZeroNeutronDecayIntegrator");
}

PiZeroNeutronDecayIntegrator::~PiZeroNeutronDecayIntegrator() { setDescription("PiZeroNeutronDecayIntegrator"); }

QDiffIntensity PiZeroNeutronDecayIntegrator::integrateOverLOS(const QDirection &direction) const {
	return integrateOverLOS(direction, 1_GeV);
}

QDiffIntensity PiZeroNeutronDecayIntegrator::integrateOverLOS(const QDirection &direction_,
                                                              const QEnergy &Eneutron_) const {
	QDiffIntensity total_diff_flux(0.0);

	auto gasType = ngdensity->getGasType();

	const auto K = decayInverseLength(Eneutron_);

	// Sum over rings
	for (const auto &ring : *ngdensity) {
		// TODO: this could be better
		if (!ngdensity->isRingEnabled(ring->getIndex())) continue;

		/** Normalization-part **/
		// p_Theta_f(r) = profile(r) * Theta_in(r)
		auto p_Theta_f = [ring, gasType, this](const Vector3QLength &pos) {
			return (ring->isInside(pos)) ? dProfile->getPDensity(gasType, pos) : 0;
		};
		auto normIntegrand = [this, p_Theta_f, direction_](const QLength &dist) {
			return p_Theta_f(getGalacticPosition(this->observerPosition, dist, direction_));
		};

		// optimize LOS integration limits:
		// instead of 0 and getMaxDistance(dir)
		auto b = ring->getBoundaries();
		auto rho = observerPosition.getRho();
		QLength r_min = rho - b.second;
		if (r_min < 0_m) r_min = 0_m;
		QLength r_max = rho + b.second;
		if (r_max > getMaxDistance(direction_)) r_max = getMaxDistance(direction_);

		QColumnDensity normIntegral = simpsonIntegration<QColumnDensity, QPDensity>(normIntegrand, r_min, r_max, 200);

		// LOS is not crossing the current ring at all, skip
		if (normIntegral == QColumnDensity(0)) continue;

		// std::cerr << "normIntegral" << normIntegral << std::endl;

		/** LOS integral over emissivity **/
		// los_f = emissivity(r) * profile(r) * Theta_in(r)
		auto los_f = [ring, gasType, this](const Vector3QLength &pos, const QEnergy &Eneutron_) {
			return (ring->isInside(pos))
			           ? dProfile->getPDensity(gasType, pos) * this->integrateOverEnergy(pos, Eneutron_)
			           : 0;
		};
		auto losIntegrand = [this, los_f, direction_, Eneutron_, K](const QLength &dist) {
			return los_f(getGalacticPosition(this->observerPosition, dist, direction_), Eneutron_) * exp(-K * dist);
		};
		QDiffIntensity losIntegral =
		    simpsonIntegration<QDiffFlux, QGREmissivity>(losIntegrand, r_min, r_max, 500) / (4_pi * 1_sr);

		// Finally, normalize LOS integrals, separatelly for HI and CO
		total_diff_flux += ring->getColumnDensity(direction_) / normIntegral * losIntegral;
	}

	return total_diff_flux;
}

QInverseLength PiZeroNeutronDecayIntegrator::decayInverseLength(const QEnergy &Eneutron_) const {
	QTime decayTime = 878.4 * 1_s;
	QEnergy mn_c2 = m_neutron * c_squared;
	auto gamma_n = Eneutron_ / mn_c2;
	QInverseLength result = 1. / gamma_n / decayTime / c_light;
	return QInverseLength(result);
}

}  // namespace hermes
