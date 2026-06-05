#include "hermes/integrators/DarkMatterNeutronIntegrator.h"

#include <cmath>
#include <stdexcept>

#include "hermes/integrators/LOSIntegrationMethods.h"

namespace hermes {

namespace {

void validateDarkMatterNeutronInputs(const std::shared_ptr<darkmatter::DarkMatterSpectrum> &spectrum,
                                     const std::shared_ptr<darkmatter::GalacticProfile> &profile,
                                     DarkMatterProcess process, const QAnnihilationCrossSection &sigmaV,
                                     double symmetryFactor, const QDecayTime &decayTime) {
	if (!spectrum) throw std::invalid_argument("hermes::DarkMatterNeutronIntegrator: spectrum must not be null");
	if (!profile) throw std::invalid_argument("hermes::DarkMatterNeutronIntegrator: profile must not be null");
	if (process == DarkMatterProcess::annihilation) {
		const double sigmaVValue = static_cast<double>(sigmaV / (units::centimetre3 / units::second));
		if (!std::isfinite(sigmaVValue) || sigmaV <= QAnnihilationCrossSection(0))
			throw std::invalid_argument(
			    "hermes::DarkMatterNeutronIntegrator: sigmaV must be positive for annihilation");
		if (!std::isfinite(symmetryFactor) || symmetryFactor < 0.)
			throw std::invalid_argument(
			    "hermes::DarkMatterNeutronIntegrator: symmetryFactor must be non-negative for annihilation");
		return;
	}

	const double decayTimeValue = static_cast<double>(decayTime / units::second);
	if (!std::isfinite(decayTimeValue) || decayTime <= QDecayTime(0))
		throw std::invalid_argument("hermes::DarkMatterNeutronIntegrator: decayTime must be positive for decay");
}

}  // namespace

DarkMatterNeutronIntegrator::DarkMatterNeutronIntegrator(
    const std::shared_ptr<darkmatter::DarkMatterSpectrum> &spectrum,
    const std::shared_ptr<darkmatter::GalacticProfile> &profile)
    : DarkMatterNeutronIntegrator(spectrum, profile, DarkMatterProcess::annihilation, 3e-26_cm3 / 1_s, 0.5, 1e28_s) {}

DarkMatterNeutronIntegrator::DarkMatterNeutronIntegrator(
    const std::shared_ptr<darkmatter::DarkMatterSpectrum> &spectrum,
    const std::shared_ptr<darkmatter::GalacticProfile> &profile, const QAnnihilationCrossSection &sigmaV,
    double symmetryFactor)
    : DarkMatterNeutronIntegrator(spectrum, profile, DarkMatterProcess::annihilation, sigmaV, symmetryFactor, 1e28_s) {}

DarkMatterNeutronIntegrator::DarkMatterNeutronIntegrator(
    const std::shared_ptr<darkmatter::DarkMatterSpectrum> &spectrum,
    const std::shared_ptr<darkmatter::GalacticProfile> &profile, const QDecayTime &decayTime)
    : DarkMatterNeutronIntegrator(spectrum, profile, DarkMatterProcess::decay, 3e-26_cm3 / 1_s, 0.5, decayTime) {}

DarkMatterNeutronIntegrator::DarkMatterNeutronIntegrator(
    const std::shared_ptr<darkmatter::DarkMatterSpectrum> &spectrum,
    const std::shared_ptr<darkmatter::GalacticProfile> &profile, DarkMatterProcess process,
    const QAnnihilationCrossSection &sigmaV, double symmetryFactor, const QDecayTime &decayTime)
    : GammaIntegratorTemplate("DarkMatterNeutron"),
      spectrum(spectrum),
      profile(profile),
      process(process),
      sigmaV(sigmaV),
      symmetryFactor(symmetryFactor),
      decayTime(decayTime),
      includeDecay(true) {
	validateDarkMatterNeutronInputs(spectrum, profile, process, sigmaV, symmetryFactor, decayTime);
}

DarkMatterNeutronIntegrator::~DarkMatterNeutronIntegrator() {}

QDiffIntensity DarkMatterNeutronIntegrator::integrateOverLOS(const QDirection &direction) const {
	return QDiffIntensity(0);
}

QDiffIntensity DarkMatterNeutronIntegrator::integrateOverLOS(const QDirection &direction_,
                                                             const QEnergy &Eneutron_) const {
	try {
		const auto K = includeDecay ? decayInverseLength(Eneutron_) : QInverseLength(0);

		auto integrand = [this, direction_, Eneutron_, K](const QLength &dist) {
			return this->spectralEmissivity(getGalacticPosition(getObsPosition(), dist, direction_), Eneutron_) *
			       exp(-K * dist);
		};

		return gslQAGSIntegration<QDiffFlux, QGREmissivity>([integrand](QLength dist) { return integrand(dist); }, 0,
		                                                    getMaxDistance(direction_), 500) /
		       (4_pi * 1_sr);
	} catch (const std::exception &) {
		return QDiffIntensity(0);
	}
}

QGREmissivity DarkMatterNeutronIntegrator::spectralEmissivity(const Vector3QLength &pos, QEnergy Eneutron) const {
	const auto restMassEnergy = spectrum->getRestMassEnergy();
	if (restMassEnergy <= QEnergy(0)) return QGREmissivity(0);

	const auto dNdE = spectrum->getParticlesPerEnergy(Eneutron);
	if (dNdE <= QInverseEnergy(0)) return QGREmissivity(0);

	const auto density = profile->getMassDensity(pos.getR());
	const double densityValue = static_cast<double>(density / (1_kg / 1_m3));
	if (!std::isfinite(densityValue) || densityValue < 0.) return QGREmissivity(0);

	if (process == DarkMatterProcess::decay) {
		return pow<2>(c_light) / (decayTime * restMassEnergy) * dNdE * density;
	}

	return symmetryFactor * sigmaV * pow<4>(c_light) / pow<2>(restMassEnergy) * dNdE * pow<2>(density);
}

QInverseLength DarkMatterNeutronIntegrator::decayInverseLength(const QEnergy &Eneutron_) const {
	if (Eneutron_ <= QEnergy(0)) return QInverseLength(0);

	const QTime neutronDecayTime = 878.4 * 1_s;
	const QEnergy neutronRestMassEnergy = m_neutron * c_squared;
	const auto gamma_n = Eneutron_ / neutronRestMassEnergy;
	return QInverseLength(1. / gamma_n / neutronDecayTime / c_light);
}

DarkMatterProcess DarkMatterNeutronIntegrator::getProcess() const { return process; }

QAnnihilationCrossSection DarkMatterNeutronIntegrator::getSigmaV() const { return sigmaV; }

double DarkMatterNeutronIntegrator::getSymmetryFactor() const { return symmetryFactor; }

QDecayTime DarkMatterNeutronIntegrator::getDecayTime() const { return decayTime; }

void DarkMatterNeutronIntegrator::setIncludeDecay(bool includeDecay_) { includeDecay = includeDecay_; }

bool DarkMatterNeutronIntegrator::isDecayEnabled() const { return includeDecay; }

}  // namespace hermes
