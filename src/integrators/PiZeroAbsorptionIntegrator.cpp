#include "hermes/integrators/PiZeroAbsorptionIntegrator.h"

#include <cmath>
#include <memory>
#include <mutex>
#include <stdexcept>

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
	const auto K = getAbsorptionCoefficient(Egamma_);
	return integrateOverLOSWithAttenuation(direction_, Egamma_, K);
}

QInverseLength PiZeroAbsorptionIntegrator::getAbsorptionCoefficient(const QEnergy &Egamma_) const {
	const double energyKey = static_cast<double>(Egamma_ / 1_GeV);
	if (!std::isfinite(energyKey) || Egamma_ <= QEnergy(0)) return absorptionCoefficient(Egamma_);
	std::lock_guard<std::mutex> lock(absorptionCoefficientCacheMutex);
	const auto cached = absorptionCoefficientCache.find(energyKey);
	if (cached != absorptionCoefficientCache.end()) return cached->second;

	const QInverseLength coefficient = absorptionCoefficient(Egamma_);
	absorptionCoefficientCache.emplace(energyKey, coefficient);
	return coefficient;
}

auto cmbPhotonField(const QEnergy &eps) {
	using hermes::units::expm1;
	const auto K = 1. / (M_PI * M_PI) / pow<3>(h_planck_bar * c_light);
	const auto cmbTemperature = 2.725_K;
	const auto kT = k_boltzmann * cmbTemperature;
	return K * pow<2>(eps) / expm1(eps / kT);
}

QInverseLength PiZeroAbsorptionIntegrator::absorptionCoefficient(const QEnergy &Egamma_) const {
	const double energyValue = static_cast<double>(Egamma_ / 1_GeV);
	if (!std::isfinite(energyValue) || Egamma_ <= QEnergy(0))
		throw std::invalid_argument("PiZeroAbsorptionIntegrator: gamma-ray energy must be positive and finite");

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

	std::unique_ptr<gsl_integration_workspace, decltype(&gsl_integration_workspace_free)> workspace(
	    gsl_integration_workspace_alloc(GSL_LIMIT), gsl_integration_workspace_free);
	if (!workspace)
		throw std::runtime_error(
		    "PiZeroAbsorptionIntegrator::absorptionCoefficient: could not allocate GSL workspace");

	int status = GSL_SUCCESS;
	{
		std::lock_guard<std::mutex> lock(detail::gslErrorHandlerMutex());
		detail::ScopedGslErrorHandlerOff disableGslAbort;
		status = gsl_integration_qag(F, static_cast<double>(epsMin), static_cast<double>(epsMax), abs_error, rel_error,
		                             GSL_LIMIT, key, workspace.get(), &result, &error);
	}
	detail::throwIfGslFailed(status, "PiZeroAbsorptionIntegrator::absorptionCoefficient");
	if (!std::isfinite(result) || result < 0.0)
		throw std::runtime_error("PiZeroAbsorptionIntegrator::absorptionCoefficient: invalid result");

	return QInverseLength(result);
}

}  // namespace hermes
