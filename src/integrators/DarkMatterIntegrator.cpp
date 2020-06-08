#include "hermes/integrators/DarkMatterIntegrator.h"

#include "hermes/integrators/LOSIntegrationMethods.h"

namespace hermes {

DarkMatterIntegrator::DarkMatterIntegrator(
    const std::shared_ptr<darkmatter::DarkMatterSpectrum> &spectrum,
    const std::shared_ptr<darkmatter::GalacticProfile> &profile)
    : GammaIntegratorTemplate("DarkMatter"),
      spectrum(spectrum),
      profile(profile) {}

DarkMatterIntegrator::~DarkMatterIntegrator() {}

QDiffIntensity DarkMatterIntegrator::integrateOverLOS(
    const QDirection &direction) const {
	return QDiffIntensity(0);
}

QDiffIntensity DarkMatterIntegrator::integrateOverLOS(
    const QDirection &direction_, const QEnergy &Egamma_) const {
	auto integrand = [this, direction_, Egamma_](const QLength &dist) {
		return this->spectralEmissivity(
		    getGalacticPosition(getSunPosition(), dist, direction_), Egamma_);
	};

	return gslQAGSIntegration<QDiffFlux, QGREmissivity>(
	           [integrand](QLength dist) { return integrand(dist); }, 0,
	           getMaxDistance(direction_), 500) /
	       (4_pi * 1_sr);
}

QGREmissivity DarkMatterIntegrator::spectralEmissivity(
    const Vector3QLength &pos, QEnergy Egamma) const {
	const auto sigma_v = 3e-26_cm3 / 1_s;
	return 0.5 * sigma_v * pow<4>(c_light) /
	       pow<2>(spectrum->getRestMassEnergy()) *
	       spectrum->getParticlesPerEnergy(Egamma) *
	       pow<2>(profile->getMassDensity(pos.getR()));
}

}  // namespace hermes
