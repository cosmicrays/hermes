#include "hermes/integrators/FreeFreeIntegrator.h"

#include <iostream>
#include <memory>

namespace hermes {

FreeFreeIntegrator::FreeFreeIntegrator(
	const std::shared_ptr<GasDensity> gdensity_) : 
	gdensity(gdensity_) {	
}

FreeFreeIntegrator::~FreeFreeIntegrator() { }

QNumber FreeFreeIntegrator::gauntFactor(QFrequency freq, QTemperature T, int Z) const {

	// optimisation of constants
	constexpr double sqrtEuler = std::sqrt(eulerGamma);
	constexpr auto K = (128*pow<2>(epsilon0)*pow<3>(k_boltzmann)) /
			(m_electron*pow<4>(e_plus));

	return std::sqrt(3)/2_pi *
		(log(K * T*T*T / ((freq*Z)*(freq*Z))) - sqrtEuler);
}

QEmissivity FreeFreeIntegrator::spectralEmissivity(
	Vector3QLength pos_, QFrequency freq_) const {

	int Z = 1;
	QPDensity N, N_e;
	QTemperature T = gdensity->getTemperature();
	N = N_e = gdensity->getDensity(pos_); 
		
	return spectralEmissivityExplicit(N, N_e, freq_, T, Z);
	
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


QTemperature FreeFreeIntegrator::integrateOverLOS(
                QDirection direction) const {
	return integrateOverLOS(direction, 408.0_GHz);
}

QTemperature FreeFreeIntegrator::integrateOverLOS(
		QDirection direction, QFrequency freq_) const {

	Vector3QLength positionSun(8.5_kpc, 0, 0);
	Vector3QLength pos(0.0);
	QIntensity total_intensity(0);
	QLength delta_d = 10.0_pc;

	// distance from the (spherical) galactic border in the given direction
	QLength maxDistance = distanceToGalBorder(positionSun, direction);

	// TODO: implement sophisticated adaptive integration method :-)
	for(QLength dist = 0; dist <= maxDistance; dist += delta_d) {
		pos = getGalacticPosition(positionSun, dist, direction);
		total_intensity += spectralEmissivity(pos, freq_) * delta_d;
	}
	return intensityToTemperature(total_intensity / 4_pi, freq_);
}

} // namespace hermes 
