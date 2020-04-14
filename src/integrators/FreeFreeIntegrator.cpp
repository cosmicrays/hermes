#include "hermes/integrators/FreeFreeIntegrator.h"
#include "hermes/integrators/LOSIntegrationMethods.h"

#include <memory>

namespace hermes {

FreeFreeIntegrator::FreeFreeIntegrator(
	const std::shared_ptr<chargedgas::ChargedGasDensity> gdensity_) : 
	RadioIntegratorTemplate(), gdensity(gdensity_) {	
}

FreeFreeIntegrator::~FreeFreeIntegrator() { }

QTemperature FreeFreeIntegrator::integrateOverLOS(
                QDirection direction) const {
	return integrateOverLOS(direction, 408.0_GHz);
}

QTemperature FreeFreeIntegrator::integrateOverLOS(
		QDirection direction, QFrequency freq_) const {
	
	auto integrand = [this, direction, freq_](const QLength &dist) {
		return this->spectralEmissivity(getGalacticPosition(getSunPosition(), dist, direction), freq_); };

	QIntensity total_intensity = simpsonIntegration<QIntensity, QEmissivity>(
			[integrand](QLength dist) {return integrand(dist);}, 0, getMaxDistance(direction), 500);

	return intensityToTemperature(total_intensity / 4_pi, freq_);
}

QNumber FreeFreeIntegrator::gauntFactor(QFrequency freq, QTemperature T, int Z) const {
	// Gaunt factor in the radio approximation from Longair 2011, Eq. 6.48a

	// optimisation of constants
	constexpr double sqrtEuler = sqrt(eulerGamma);
	constexpr auto K = (128*pow<2>(epsilon0)*pow<3>(k_boltzmann)) /
			(m_electron*pow<4>(e_plus));
	
	auto logLambda = (log(K * T*T*T / ((freq*Z)*(freq*Z))) - sqrtEuler) / 2.;

	return std::sqrt(3)/pi * logLambda;
}

QEmissivity FreeFreeIntegrator::spectralEmissivity(
	Vector3QLength pos_, QFrequency freq_) const {

	int Z = 1;
	QPDensity N, N_e;
	QTemperature T = gdensity->getTemperature();
	N = N_e = gdensity->getDensity(pos_); 
		
	return spectralEmissivityExplicit(N, N_e, freq_, T, Z);
	
}

QInverseLength FreeFreeIntegrator::absorptionCoefficient(
	Vector3QLength pos_, QFrequency freq_) const {

	QTemperature T = 1e4_K;
	
	return spectralEmissivity(pos_, freq_) *
		c_squared / (8_pi*h_planck*pow<3>(freq_)) *
		expm1(h_planck*freq_/(k_boltzmann*T));
}


QEmissivity FreeFreeIntegrator::spectralEmissivityExplicit(
	QPDensity N, QPDensity N_e, QFrequency freq, QTemperature T, int Z) const {

	// optimisation of constants
	constexpr auto K = 1.0/(3*pi*pi) * std::sqrt(pi/6) * pow<6>(e_plus) /
                        (pow<3>(epsilon0*c_light)*pow<2>(m_electron)) *
			sqrt(m_electron/(k_boltzmann));

	return K * Z*Z * gauntFactor(freq, T, Z) * N * N_e * sqrt(1/T) *
		exp(-h_planck*freq/(k_boltzmann*T));
}

} // namespace hermes 
