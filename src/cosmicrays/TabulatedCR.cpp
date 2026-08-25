#include "hermes/cosmicrays/TabulatedCR.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace hermes { namespace cosmicrays {

namespace {

QDiffIntensity defaultIntensityUnit() { return 1. / (1_GeV * 1_cm2 * 1_s * 1_sr); }

double protonBeta(const QEnergy &kineticEnergy) {
	const double kineticEnergyInRestMassUnits = static_cast<double>(kineticEnergy / (m_proton * c_squared));
	if (kineticEnergyInRestMassUnits < 1e-4) {
		return std::sqrt(kineticEnergyInRestMassUnits * (kineticEnergyInRestMassUnits + 2.)) /
		       (kineticEnergyInRestMassUnits + 1.);
	}

	const double inverseGamma = 1. / (1. + kineticEnergyInRestMassUnits);
	return std::sqrt((1. - inverseGamma) * (1. + inverseGamma));
}

std::invalid_argument invalidRow(const std::string &filename, std::size_t lineNumber, const std::string &reason) {
	std::ostringstream message;
	message << "hermes::TabulatedCR: invalid row " << lineNumber << " in '" << filename << "': " << reason;
	return std::invalid_argument(message.str());
}

}  // namespace

TabulatedCR::TabulatedCR(const std::string &filename, const QLength &sigmaZ_, int integrationSteps)
    : TabulatedCR(filename, 1_GeV, defaultIntensityUnit(), sigmaZ_, integrationSteps) {}

TabulatedCR::TabulatedCR(const std::string &filename, const QEnergy &energyUnit, const QDiffIntensity &intensityUnit,
                         const QLength &sigmaZ_, int integrationSteps)
    : CosmicRayDensity(Proton), sigmaZ(sigmaZ_) {
	if (!std::isfinite(static_cast<double>(sigmaZ)) || sigmaZ <= QLength(0)) {
		throw std::invalid_argument("hermes::TabulatedCR: sigmaZ must be finite and positive");
	}
	if (!std::isfinite(static_cast<double>(energyUnit)) || energyUnit <= QEnergy(0)) {
		throw std::invalid_argument("hermes::TabulatedCR: energyUnit must be finite and positive");
	}
	if (!std::isfinite(static_cast<double>(intensityUnit)) || intensityUnit <= QDiffIntensity(0)) {
		throw std::invalid_argument("hermes::TabulatedCR: intensityUnit must be finite and positive");
	}

	loadTable(filename, energyUnit, intensityUnit);
	makeEnergyRange(integrationSteps);
}

void TabulatedCR::loadTable(const std::string &filename, const QEnergy &energyUnit,
                            const QDiffIntensity &intensityUnit) {
	std::ifstream input(filename);
	if (!input) {
		throw std::runtime_error("hermes::TabulatedCR: cannot open '" + filename + "'");
	}

	std::string line;
	std::size_t lineNumber = 0;
	while (std::getline(input, line)) {
		++lineNumber;
		const auto comment = line.find('#');
		if (comment != std::string::npos) line.erase(comment);

		std::istringstream row(line);
		row >> std::ws;
		if (row.eof()) continue;

		double energyValue = 0;
		double intensityValue = 0;
		if (!(row >> energyValue >> intensityValue)) {
			throw invalidRow(filename, lineNumber, "expected two numeric columns");
		}
		row >> std::ws;
		if (!row.eof()) {
			throw invalidRow(filename, lineNumber, "expected exactly two columns");
		}
		if (!std::isfinite(energyValue) || energyValue <= 0) {
			throw invalidRow(filename, lineNumber, "energy must be finite and positive");
		}
		if (!std::isfinite(intensityValue) || intensityValue <= 0) {
			throw invalidRow(filename, lineNumber, "intensity must be finite and positive");
		}

		const QEnergy energy = energyValue * energyUnit;
		const QDiffIntensity intensity = intensityValue * intensityUnit;

		if (!std::isfinite(static_cast<double>(energy)) || energy <= QEnergy(0) ||
		    !std::isfinite(static_cast<double>(intensity)) || intensity <= QDiffIntensity(0)) {
			throw invalidRow(filename, lineNumber, "converted values exceed the supported range");
		}
		if (!tabulatedEnergies.empty() && energy <= tabulatedEnergies.back()) {
			throw invalidRow(filename, lineNumber, "energies must be strictly increasing");
		}

		tabulatedEnergies.push_back(energy);
		tabulatedIntensities.push_back(intensity);
	}

	if (input.bad()) {
		throw std::runtime_error("hermes::TabulatedCR: error while reading '" + filename + "'");
	}
	if (tabulatedEnergies.size() < 2) {
		throw std::invalid_argument("hermes::TabulatedCR: table must contain at least two data rows");
	}
}

void TabulatedCR::makeEnergyRange(int integrationSteps) {
	if (integrationSteps < 2) {
		throw std::invalid_argument("hermes::TabulatedCR: integrationSteps must be at least 2");
	}

	const QEnergy minimumEnergy = tabulatedEnergies.front();
	const QEnergy maximumEnergy = tabulatedEnergies.back();
	energyScaleFactor = std::exp(std::log(static_cast<double>(maximumEnergy / minimumEnergy)) /
	                             static_cast<double>(integrationSteps - 1));

	energyRange.clear();
	energyRange.reserve(static_cast<std::size_t>(integrationSteps));
	for (int index = 0; index < integrationSteps; ++index) {
		energyRange.push_back(minimumEnergy * std::pow(energyScaleFactor, index));
	}
	energyRange.back() = maximumEnergy;
	setScaleFactor(true);
}

QDiffIntensity TabulatedCR::interpolateIntensity(const QEnergy &energy) const {
	if (!std::isfinite(static_cast<double>(energy)) || energy <= QEnergy(0)) {
		return QDiffIntensity(0);
	}
	if (energy < tabulatedEnergies.front() || energy > tabulatedEnergies.back()) {
		return QDiffIntensity(0);
	}

	const auto upper = std::upper_bound(tabulatedEnergies.begin(), tabulatedEnergies.end(), energy);
	if (upper == tabulatedEnergies.begin()) return tabulatedIntensities.front();
	if (upper == tabulatedEnergies.end()) return tabulatedIntensities.back();

	const auto upperIndex = static_cast<std::size_t>(std::distance(tabulatedEnergies.begin(), upper));
	const auto lowerIndex = upperIndex - 1;
	const QEnergy lowerEnergy = tabulatedEnergies[lowerIndex];
	const QEnergy upperEnergy = tabulatedEnergies[upperIndex];
	const QDiffIntensity lowerIntensity = tabulatedIntensities[lowerIndex];
	const QDiffIntensity upperIntensity = tabulatedIntensities[upperIndex];

	const double fraction =
	    std::log(static_cast<double>(energy / lowerEnergy)) / std::log(static_cast<double>(upperEnergy / lowerEnergy));
	const double intensityRatio = static_cast<double>(upperIntensity / lowerIntensity);
	return lowerIntensity * std::exp(fraction * std::log(intensityRatio));
}

QPDensityPerEnergy TabulatedCR::getDensityPerEnergy(const QEnergy &energy, const Vector3QLength &position) const {
	const QDiffIntensity intensity = interpolateIntensity(energy);
	if (intensity == QDiffIntensity(0)) return QPDensityPerEnergy(0);

	const double beta = protonBeta(energy);
	if (!std::isfinite(beta) || beta <= 0.) return QPDensityPerEnergy(0);
	const QPDensityPerEnergy midplaneDensity = intensity * (4_pi * 1_sr) / (beta * c_light);

	const double heightInSigma = static_cast<double>(position.getZ() / sigmaZ);
	const double spatialProfile = std::exp(-0.5 * heightInSigma * heightInSigma);
	return midplaneDensity * spatialProfile;
}

}}  // namespace hermes::cosmicrays
