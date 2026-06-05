#include "hermes/integrators/DarkMatterIntegrator.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <mutex>
#include <stdexcept>

#include "hermes/integrators/LOSIntegrationMethods.h"

namespace hermes {

namespace {

auto cmbPhotonField(const QEnergy &eps) {
	using hermes::units::expm1;
	const auto K = 1. / (M_PI * M_PI) / pow<3>(h_planck_bar * c_light);
	const auto cmbTemperature = 2.725_K;
	const auto kT = k_boltzmann * cmbTemperature;
	return K * pow<2>(eps) / expm1(eps / kT);
}

bool sameEnergy(const QEnergy &a, const QEnergy &b) {
	if (a == b) return true;

	const double aValue = static_cast<double>(a);
	const double bValue = static_cast<double>(b);
	const double scale = std::max(std::fabs(aValue), std::fabs(bValue));
	if (scale == 0.) return true;

	return std::fabs(aValue - bValue) <= 1e-12 * scale;
}

void validateDarkMatterInputs(const std::shared_ptr<darkmatter::DarkMatterSpectrum> &spectrum,
                              const std::shared_ptr<darkmatter::GalacticProfile> &profile, DarkMatterProcess process,
                              const QAnnihilationCrossSection &sigmaV, double symmetryFactor,
                              const QDecayTime &decayTime) {
	if (!spectrum) throw std::invalid_argument("hermes::DarkMatterIntegrator: spectrum must not be null");
	if (!profile) throw std::invalid_argument("hermes::DarkMatterIntegrator: profile must not be null");
	if (process == DarkMatterProcess::annihilation) {
		const double sigmaVValue = static_cast<double>(sigmaV / (units::centimetre3 / units::second));
		if (!std::isfinite(sigmaVValue) || sigmaV <= QAnnihilationCrossSection(0))
			throw std::invalid_argument("hermes::DarkMatterIntegrator: sigmaV must be positive for annihilation");
		if (!std::isfinite(symmetryFactor) || symmetryFactor < 0.)
			throw std::invalid_argument(
			    "hermes::DarkMatterIntegrator: symmetryFactor must be non-negative for annihilation");
		return;
	}

	const double decayTimeValue = static_cast<double>(decayTime / units::second);
	if (!std::isfinite(decayTimeValue) || decayTime <= QDecayTime(0))
		throw std::invalid_argument("hermes::DarkMatterIntegrator: decayTime must be positive for decay");
}

}  // namespace

DarkMatterIntegrator::DarkMatterIntegrator(const std::shared_ptr<darkmatter::DarkMatterSpectrum> &spectrum,
                                           const std::shared_ptr<darkmatter::GalacticProfile> &profile,
                                           bool includeAbsorption)
    : DarkMatterIntegrator(spectrum, profile, DarkMatterProcess::annihilation, 3e-26_cm3 / 1_s, 0.5, 1e28_s,
                           includeAbsorption) {}

DarkMatterIntegrator::DarkMatterIntegrator(const std::shared_ptr<darkmatter::DarkMatterSpectrum> &spectrum,
                                           const std::shared_ptr<darkmatter::GalacticProfile> &profile,
                                           const QAnnihilationCrossSection &sigmaV, double symmetryFactor,
                                           bool includeAbsorption)
    : DarkMatterIntegrator(spectrum, profile, DarkMatterProcess::annihilation, sigmaV, symmetryFactor, 1e28_s,
                           includeAbsorption) {}

DarkMatterIntegrator::DarkMatterIntegrator(const std::shared_ptr<darkmatter::DarkMatterSpectrum> &spectrum,
                                           const std::shared_ptr<darkmatter::GalacticProfile> &profile,
                                           const QDecayTime &decayTime, bool includeAbsorption)
    : DarkMatterIntegrator(spectrum, profile, DarkMatterProcess::decay, 3e-26_cm3 / 1_s, 0.5, decayTime,
                           includeAbsorption) {}

DarkMatterIntegrator::DarkMatterIntegrator(const std::shared_ptr<darkmatter::DarkMatterSpectrum> &spectrum,
                                           const std::shared_ptr<darkmatter::GalacticProfile> &profile,
                                           DarkMatterProcess process, const QAnnihilationCrossSection &sigmaV,
                                           double symmetryFactor, const QDecayTime &decayTime, bool includeAbsorption)
    : GammaIntegratorTemplate("DarkMatter"),
      spectrum(spectrum),
      profile(profile),
      process(process),
      sigmaV(sigmaV),
      symmetryFactor(symmetryFactor),
      decayTime(decayTime),
      absorptionEnabled(includeAbsorption),
      bwCrossSec(std::make_shared<interactions::BreitWheeler>()) {
	validateDarkMatterInputs(spectrum, profile, process, sigmaV, symmetryFactor, decayTime);
}

DarkMatterIntegrator::~DarkMatterIntegrator() {}

QDiffIntensity DarkMatterIntegrator::integrateOverLOS(const QDirection &direction) const { return QDiffIntensity(0); }

QDiffIntensity DarkMatterIntegrator::integrateOverLOS(const QDirection &direction_, const QEnergy &Egamma_) const {
	try {
		const auto K = absorptionEnabled ? getAbsorptionCoefficient(Egamma_) : QInverseLength(0);
		auto integrand = [this, direction_, Egamma_, K](const QLength &dist) {
			return this->spectralEmissivity(getGalacticPosition(getObsPosition(), dist, direction_), Egamma_) *
			       exp(-K * dist);
		};

		return gslQAGSIntegration<QDiffFlux, QGREmissivity>([integrand](QLength dist) { return integrand(dist); }, 0,
		                                                    getMaxDistance(direction_), 500) /
		       (4_pi * 1_sr);
	} catch (const std::exception &) {
		return QDiffIntensity(0);
	}
}

QGREmissivity DarkMatterIntegrator::spectralEmissivity(const Vector3QLength &pos, QEnergy Egamma) const {
	const auto restMassEnergy = spectrum->getRestMassEnergy();
	if (restMassEnergy <= QEnergy(0)) return QGREmissivity(0);

	const auto dNdE = spectrum->getParticlesPerEnergy(Egamma);
	if (dNdE <= QInverseEnergy(0)) return QGREmissivity(0);

	const auto density = profile->getMassDensity(pos.getR());
	const double densityValue = static_cast<double>(density / (1_kg / 1_m3));
	if (!std::isfinite(densityValue) || densityValue < 0.) return QGREmissivity(0);

	if (process == DarkMatterProcess::decay) {
		return pow<2>(c_light) / (decayTime * restMassEnergy) * dNdE * density;
	}

	return symmetryFactor * sigmaV * pow<4>(c_light) / pow<2>(restMassEnergy) * dNdE * pow<2>(density);
}

QInverseLength DarkMatterIntegrator::absorptionCoefficient(const QEnergy &Egamma_) const {
	if (Egamma_ <= QEnergy(0)) return QInverseLength(0);

	auto integrand = [this, Egamma_](double eps) {
		return static_cast<double>(cmbPhotonField(QEnergy(eps)) *
		                           bwCrossSec->integratedOverTheta(Egamma_, QEnergy(eps)));
	};

	QEnergy epsMin = 1e-5_eV;
	QEnergy epsMax = 5e-3_eV;

	double abs_error = 0.0;
	double rel_error = 1.0e-4;
	int key = GSL_INTEG_GAUSS51;
	double result = 0;
	double error = 0;

	gsl_function_pp<decltype(integrand)> Fp(integrand);
	gsl_function *F = static_cast<gsl_function *>(&Fp);

	std::unique_ptr<gsl_integration_workspace, decltype(&gsl_integration_workspace_free)> workspace_ptr(
	    gsl_integration_workspace_alloc(GSL_LIMIT), gsl_integration_workspace_free);
	if (!workspace_ptr)
		throw std::runtime_error("DarkMatterIntegrator::absorptionCoefficient: could not allocate GSL workspace");

	int status = GSL_SUCCESS;
	{
		std::lock_guard<std::mutex> lock(detail::gslErrorHandlerMutex());
		detail::ScopedGslErrorHandlerOff disableGslAbort;
		status = gsl_integration_qag(F, static_cast<double>(epsMin), static_cast<double>(epsMax), abs_error, rel_error,
		                             GSL_LIMIT, key, workspace_ptr.get(), &result, &error);
	}
	detail::throwIfGslFailed(status, "DarkMatterIntegrator::absorptionCoefficient");
	if (!std::isfinite(result))
		throw std::runtime_error("DarkMatterIntegrator::absorptionCoefficient: non-finite result");

	return QInverseLength(result);
}

QInverseLength DarkMatterIntegrator::getAbsorptionCoefficient(const QEnergy &Egamma_) const {
	for (const auto &entry : absorptionCoefficientTable) {
		if (sameEnergy(entry.first, Egamma_)) return entry.second;
	}

	return absorptionCoefficient(Egamma_);
}

void DarkMatterIntegrator::precomputeAbsorptionCoefficients(const std::vector<QEnergy> &energies) {
	std::vector<std::pair<QEnergy, QInverseLength>> table;
	table.reserve(energies.size());

	for (const auto &energy : energies) {
		table.push_back({energy, absorptionCoefficient(energy)});
	}

	absorptionCoefficientTable.swap(table);
}

void DarkMatterIntegrator::clearAbsorptionCoefficientTable() { absorptionCoefficientTable.clear(); }

std::size_t DarkMatterIntegrator::getAbsorptionCoefficientTableSize() const {
	return absorptionCoefficientTable.size();
}

DarkMatterProcess DarkMatterIntegrator::getProcess() const { return process; }

QAnnihilationCrossSection DarkMatterIntegrator::getSigmaV() const { return sigmaV; }

double DarkMatterIntegrator::getSymmetryFactor() const { return symmetryFactor; }

QDecayTime DarkMatterIntegrator::getDecayTime() const { return decayTime; }

void DarkMatterIntegrator::setAbsorptionEnabled(bool enabled) { absorptionEnabled = enabled; }

bool DarkMatterIntegrator::isAbsorptionEnabled() const { return absorptionEnabled; }

}  // namespace hermes
