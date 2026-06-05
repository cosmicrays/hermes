#include "hermes/cosmicrays/UHECR.h"

#include <cmath>
#include <stdexcept>

namespace hermes { namespace cosmicrays {

namespace {

double log1pExp(double x) { return x > 0.0 ? x + std::log1p(std::exp(-x)) : std::log1p(std::exp(x)); }

double smoothBreak(double energy, double breakEnergy, double alphaBefore, double alphaAfter, double omega) {
	return (alphaBefore - alphaAfter) * omega * log1pExp(std::log(energy / breakEnergy) / omega);
}

}  // namespace

UHECR::UHECR(const PID &pid_) : CosmicRayDensity(pid_), minE(1e8_GeV), maxE(1e12_GeV), steps(40) {
	makeEnergyRange();
	setParameters();
}

UHECR::UHECR(const PID &pid_, QEnergy minE_, QEnergy maxE_, int steps_)
    : CosmicRayDensity(pid_), minE(minE_), maxE(maxE_), steps(steps_) {
	makeEnergyRange();
	setParameters();
}

void UHECR::setParameters() {
	// Table III. PAO, Phys.Rev.D 102 (2020) 6, 062005
	n_0 = 1.748e-45 / 1_eV / 1_cm3;
	E_0 = std::pow(10., 18.5) * 1_eV;
	alpha_1 = 3.29;
	alpha_2 = 2.51;
	alpha_3 = 3.05;
	alpha_4 = 5.1;
	omega_12 = 0.05;
	omega_23 = 0.05;
	omega_34 = 0.05;
	E_min = 1e18 * 1_eV;
	E_12 = 5e18 * 1_eV;
	E_23 = 13e18 * 1_eV;
	E_34 = 46e18 * 1_eV;
}

void UHECR::makeEnergyRange() {
	if (steps < 2) throw std::invalid_argument("hermes::UHECR: steps must be at least 2");
	if (minE <= QEnergy(0)) throw std::invalid_argument("hermes::UHECR: minE must be positive");
	if (maxE <= minE) throw std::invalid_argument("hermes::UHECR: maxE must be greater than minE");

	QEnergy energy = minE;
	energyScaleFactor = exp(1. / static_cast<double>(steps - 1) * log(maxE / minE));

	for (int i = 0; i < steps; ++i) {
		energyRange.push_back(energy);
		energy = energy * energyScaleFactor;
	}
}

QPDensityPerEnergy UHECR::getDensityPerEnergy(const QEnergy &E_, const Vector3QLength &pos_) const {
	if (E_ < 0.3 * E_min) return QPDensityPerEnergy(0);

	const double energy = static_cast<double>(E_ / 1_eV);
	const double logSpectrum = -alpha_1 * std::log(static_cast<double>(E_ / E_0)) +
	                           smoothBreak(energy, static_cast<double>(E_12 / 1_eV), alpha_1, alpha_2, omega_12) +
	                           smoothBreak(energy, static_cast<double>(E_23 / 1_eV), alpha_2, alpha_3, omega_23) +
	                           smoothBreak(energy, static_cast<double>(E_34 / 1_eV), alpha_3, alpha_4, omega_34);

	return n_0 * std::exp(logSpectrum);
}

}}  // namespace hermes::cosmicrays
