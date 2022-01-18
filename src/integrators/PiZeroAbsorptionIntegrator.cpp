#include "hermes/integrators/PiZeroAbsorptionIntegrator.h"

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

PiZeroAbsorptionIntegrator::PiZeroAbsorptionIntegrator(
    const std::shared_ptr<cosmicrays::CosmicRayDensity> &crDensity_,
    const std::shared_ptr<neutralgas::RingModel> &ngdensity_,
    const std::shared_ptr<interactions::DifferentialCrossSection> &crossSec_)
    : PiZeroIntegrator(crDensity_, ngdensity_, crossSec_) {
	setDescription("PiZeroAbsorptionIntegrator");
}

PiZeroAbsorptionIntegrator::PiZeroAbsorptionIntegrator(
    const std::vector<std::shared_ptr<cosmicrays::CosmicRayDensity>> &crList_,
    const std::shared_ptr<neutralgas::RingModel> &ngdensity_,
    const std::shared_ptr<interactions::DifferentialCrossSection> &crossSec_)
    : PiZeroIntegrator(crList_, ngdensity_, crossSec_) {
	setDescription("PiZeroAbsorptionIntegrator");
}

PiZeroAbsorptionIntegrator::~PiZeroAbsorptionIntegrator() { setDescription("PiZeroAbsorptionIntegrator"); }

QDiffIntensity PiZeroAbsorptionIntegrator::integrateOverLOS(const QDirection &direction) const {
	return integrateOverLOS(direction, 1_GeV);
}

QDiffIntensity PiZeroAbsorptionIntegrator::integrateOverLOS(const QDirection &direction_,
                                                            const QEnergy &Egamma_) const {
	QDiffIntensity total_diff_flux(0.0);

	auto gasType = ngdensity->getGasType();

	const auto K = absorptionCoefficient(Egamma_);

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
		auto los_f = [ring, gasType, this](const Vector3QLength &pos, const QEnergy &Egamma_) {
			return (ring->isInside(pos)) ? dProfile->getPDensity(gasType, pos) * this->integrateOverEnergy(pos, Egamma_)
			                             : 0;
		};
		auto losIntegrand = [this, los_f, direction_, Egamma_, K](const QLength &dist) {
			return los_f(getGalacticPosition(this->observerPosition, dist, direction_), Egamma_) * exp(-K * dist);
		};
		QDiffIntensity losIntegral =
		    simpsonIntegration<QDiffFlux, QGREmissivity>(losIntegrand, r_min, r_max, 500) / (4_pi * 1_sr);

		// Finally, normalize LOS integrals, separatelly for HI and CO
		total_diff_flux += ring->getColumnDensity(direction_) / normIntegral * losIntegral;
	}

	return total_diff_flux;
}

auto cmbPhotonField(const QEnergy &eps) {
	using hermes::units::expm1;
	constexpr auto K = 1. / (M_PI * M_PI) / pow<3>(h_planck_bar * c_light);
	constexpr auto cmbTemperature = 2.725_K;
	constexpr auto kT = k_boltzmann * cmbTemperature;
	return K * pow<2>(eps) / expm1(eps / kT);
}

QInverseLength PiZeroAbsorptionIntegrator::absorptionCoefficient(const QEnergy &Egamma_) const {
	auto integrand = [this, Egamma_](double eps) {
		return static_cast<double>(cmbPhotonField(QEnergy(eps)) *
		                           bwCrossSec->integratedOverTheta(Egamma_, QEnergy(eps)));
	};

	QEnergy epsMin = 1e-5_eV;
	QEnergy epsMax = 5e-3_eV;

	double abs_error = 0.0;  // disabled
	double rel_error = 1.0e-4;
	int key = GSL_INTEG_GAUSS51;  // GSL_INTEG_GAUSS15;
	double result = 0;
	double error = 0;

	gsl_function_pp<decltype(integrand)> Fp(integrand);
	gsl_function *F = static_cast<gsl_function *>(&Fp);

	gsl_integration_workspace *w = gsl_integration_workspace_alloc(GSL_LIMIT);
	gsl_integration_qag(F, static_cast<double>(epsMin), static_cast<double>(epsMax), abs_error, rel_error, GSL_LIMIT,
	                    key, w, &result, &error);
	gsl_integration_workspace_free(w);

	return QInverseLength(result);
}

}  // namespace hermes
