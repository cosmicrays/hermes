#include "hermes/photonfields/ISRF.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>

#include "hermes/Common.h"
#include "hermes/FITSWrapper.h"
#include "kiss/logger.h"

#define DEFAULT_ISRF_FILE "RadiationField/Vernetto16/isrf.fits.gz"

namespace hermes { namespace photonfields {

std::string str(const int &n) {
	std::stringstream ss;
	ss << std::setfill('0') << std::setw(3) << n;
	return ss.str();
}

ISRF::ISRF() {
	loadISRF();
	initializeEnergyRange();
}

ISRF::ISRF(const std::string &filename) {
	loadCombinedISRF(filename);
	initializeEnergyRange();
}

void ISRF::initializeEnergyRange() {
	auto logWavelenghtToFrequency = [](double lambda) {
		return c_light / (std::pow(10, lambda) * micrometre);
	};
	setStartEnergy(logWavelenghtToFrequency(logwavelenghts.back()) * h_planck);
	setEndEnergy(logWavelenghtToFrequency(logwavelenghts.front()) * h_planck);

	// Spares steps
	// setEnergyScaleFactor(1.1); // 145 steps
	setEnergyScaleFactor(1.05);

	// Alternative (slow), all available energy steps
	/*
	double scaling = std::pow(static_cast<double>(
	            getEndEnergy()/getStartEnergy()),
	        1.0/logwavelenghts.size()); // ~1.01
	setEnergyScaleFactor(scaling); // 1211 steps
	*/

	buildEnergyRange();
}

void ISRF::buildEnergyRange() {
	const double scaling = getEnergyScaleFactor();
	const QEnergy E_start = getStartEnergy();
	const QEnergy E_end = getEndEnergy();

	for (QEnergy E = E_start; E < E_end; E = E * scaling)
		energyRange.push_back(E);
}

void ISRF::loadLegacyFrequencyAxis() {
	logwavelenghts.resize(freqR1 + freqR2 + freqR3);
	double logwl = log10(0.01);  // micron
	for (size_t i = 0; i < freqR1; ++i) {
		logwavelenghts[i] = logwl;
		logwl += 0.01;
	}
	for (size_t i = 0; i < freqR2; ++i) {
		logwavelenghts[freqR1 + i] = logwl;
		logwl += 0.0025;
	}
	for (size_t i = 0; i < freqR3; ++i) {
		logwavelenghts[freqR1 + freqR2 + i] = logwl;
		logwl += 0.01;
	}
}

std::size_t ISRF::getSize() const { return isrf.size(); }

void ISRF::loadISRF() {
	const std::string filename = getDataPath(DEFAULT_ISRF_FILE);
	std::ifstream combinedFile(filename, std::ios::binary);
	if (combinedFile.good()) {
		loadCombinedISRF(filename);
		return;
	}

	KISS_LOG_WARNING << "ISRF: combined data file not found; reading legacy "
	                    "Vernetto16 text tables"
	                 << std::endl;
	loadLegacyFrequencyAxis();
	loadLegacyISRF();
}

void ISRF::loadCombinedISRF(const std::string &filename) {
	FITSFile file(filename);
	file.openFile(FITS::READ);
	file.moveToHDU(1);

	const auto dimensions = file.getImageDimensions();
	const std::vector<long> expectedDimensions = {
	    freqR1 + freqR2 + freqR3, 24, 30};
	if (dimensions != expectedDimensions || file.getNumberOfHDUs() != 4)
		throw std::runtime_error(
		    "hermes: error: Invalid Vernetto16 density-cube dimensions in " +
		    filename);

	const auto wavelengthCount = static_cast<std::size_t>(dimensions[0]);
	const auto heightCount = static_cast<std::size_t>(dimensions[1]);
	const auto radiusCount = static_cast<std::size_t>(dimensions[2]);
	const auto densityCount = wavelengthCount * heightCount * radiusCount;
	isrf = file.readImageAsDouble(1, densityCount);

	file.moveToHDU(2);
	if (file.readKeyValueAsString("EXTNAME") != "LOGWAVE" ||
	    file.getImageDimensions() != std::vector<long>{dimensions[0]})
		throw std::runtime_error(
		    "hermes: error: Invalid LOGWAVE axis in " + filename);
	logwavelenghts = file.readImageAsDouble(1, wavelengthCount);

	file.moveToHDU(3);
	if (file.readKeyValueAsString("EXTNAME") != "R_GRID" ||
	    file.getImageDimensions() != std::vector<long>{dimensions[2]})
		throw std::runtime_error(
		    "hermes: error: Invalid R_GRID axis in " + filename);
	r_id = file.readImageAsDouble(1, radiusCount);

	file.moveToHDU(4);
	if (file.readKeyValueAsString("EXTNAME") != "Z_GRID" ||
	    file.getImageDimensions() != std::vector<long>{dimensions[1]})
		throw std::runtime_error(
		    "hermes: error: Invalid Z_GRID axis in " + filename);
	z_id = file.readImageAsDouble(1, heightCount);

	const auto isStrictlyIncreasing = [](const std::vector<double> &axis) {
		return std::adjacent_find(axis.begin(), axis.end(),
		                          [](double left, double right) {
			                          return !std::isfinite(left) ||
			                                 !std::isfinite(right) || left >= right;
		                          }) == axis.end();
	};
	if (!isStrictlyIncreasing(logwavelenghts) ||
	    !isStrictlyIncreasing(r_id) || !isStrictlyIncreasing(z_id) ||
	    std::any_of(isrf.begin(), isrf.end(), [](double density) {
		    return !std::isfinite(density) || density < 0;
	    }))
		throw std::runtime_error(
		    "hermes: error: Invalid numerical values in " + filename);
}

void ISRF::loadLegacyISRF() {
	const int max_num_of_char_in_a_line = 512;
	const int num_of_header_lines = 1;
	r_id = {0.0,  0.2,  0.5,  1.0,  1.5,  2.0,  2.5,  3.0,
	        3.5,  4.0,  4.5,  5.0,  5.5,  6.0,  6.5,  7.0,
	        7.5,  8.0,  8.5,  9.0,  9.5,  10.0, 11.0, 12.0,
	        14.0, 16.0, 18.0, 20.0, 25.0, 30.0};
	z_id = {0.0, 0.1, 0.2, 0.3, 0.4,  0.5,  0.6,  0.8,
	        1.0, 1.2, 1.5, 2.0, 2.5,  3.0,  4.0,  5.0,
	        6.0, 8.0, 10., 12., 15.0, 20.0, 25.0, 30.0};

	for (auto i : r_id) {
		for (auto j : z_id) {
			std::ostringstream name;
			name << "RadiationField/Vernetto16/spectrum_r"
			     << str(static_cast<int>(i * 10)) << "_z"
			     << str(static_cast<int>(j * 10)) << ".dat";
			std::string filename = getDataPath(name.str());

			std::ifstream fin(filename.c_str());
			if (!fin) {
				std::stringstream ss;
				ss << "hermes: error: File " << filename << " not found";
				throw std::runtime_error(ss.str());
			}
			for (std::size_t k = 0; k < num_of_header_lines; ++k) {
				fin.ignore(max_num_of_char_in_a_line, '\n');
			}
			while (!fin.eof()) {
				double f_, e_;
				fin >> f_ >> e_;
				if (!fin.eof()) isrf.push_back(e_);
			}
		}
	}
	const auto expectedSize =
	    r_id.size() * z_id.size() * logwavelenghts.size();
	if (isrf.size() != expectedSize)
		throw std::runtime_error(
		    "hermes: error: Invalid legacy Vernetto16 table dimensions");
}

double ISRF::getISRF(std::size_t ir, std::size_t iz, std::size_t imu) const {
	std::size_t i = imu + iz * logwavelenghts.size() +
	                ir * (logwavelenghts.size() * z_id.size());
	return isrf[i];
}

QEnergyDensity ISRF::getEnergyDensity(const Vector3QLength &pos,
                                      std::size_t iE) const {
	QLength r = sqrt(pos.x * pos.x + pos.y * pos.y);
	QLength z = pos.z;
	QEnergy E = energyRange[iE];
	// TODO(adundovi): not implemented
	return getEnergyDensity(r, z, E);
}

QEnergyDensity ISRF::getEnergyDensity(const Vector3QLength &pos,
                                      const QEnergy &E_photon) const {
	QLength r = sqrt(pos.x * pos.x + pos.y * pos.y);
	QLength z = pos.z;
	return getEnergyDensity(r, z, E_photon);
}

QEnergyDensity ISRF::getEnergyDensity(const QLength &r, const QLength &z,
                                      const QEnergy &E_photon) const {
	double r_ = static_cast<double>(r / 1_kpc);
	double z_ = static_cast<double>(fabs(z) / 1_kpc);
	double f_mu =
	    static_cast<double>(h_planck * c_light / E_photon / (micrometre));
	double logf_ = std::log10(f_mu);

	if (r_ < r_id.front() || r_ > r_id.back()) return 0;
	if (z_ < z_id.front() || z_ > z_id.back()) return 0;
	if (logf_ < logwavelenghts.front() || logf_ > logwavelenghts.back())
		return 0;

	const auto lowerGridIndex = [](const std::vector<double> &axis,
	                               double value) {
		auto upper = std::upper_bound(axis.begin(), axis.end(), value);
		if (upper == axis.begin()) return std::size_t{0};
		if (upper == axis.end()) return axis.size() - 2;
		return static_cast<std::size_t>(upper - axis.begin() - 1);
	};
	const std::size_t ir = lowerGridIndex(r_id, r_);
	const std::size_t iz = lowerGridIndex(z_id, z_);
	const std::size_t ifreq = lowerGridIndex(logwavelenghts, logf_);

	double r_d = (r_ - r_id[ir]) / (r_id[ir + 1] - r_id[ir]);
	double z_d = (z_ - z_id[iz]) / (z_id[iz + 1] - z_id[iz]);
	double f_d = (logf_ - logwavelenghts[ifreq]) /
	             (logwavelenghts[ifreq + 1] - logwavelenghts[ifreq]);

	/*
	if (!(r_d >= 0 && r_d <= 1))
	    return 0;
	if (!(z_d >= 0 && z_d <= 1))
	    return 0;
	if (!(f_d >= 0 && f_d <= 1))
	    return 0;
	*/

	double c_00 =
	    getISRF(ir, iz, ifreq) * (1. - r_d) + getISRF(ir + 1, iz, ifreq) * r_d;
	double c_01 = getISRF(ir, iz, ifreq + 1) * (1. - r_d) +
	              getISRF(ir + 1, iz, ifreq + 1) * r_d;
	double c_10 = getISRF(ir, iz + 1, ifreq) * (1. - r_d) +
	              getISRF(ir + 1, iz + 1, ifreq) * r_d;
	double c_11 = getISRF(ir, iz + 1, ifreq + 1) * (1. - r_d) +
	              getISRF(ir + 1, iz + 1, ifreq + 1) * r_d;

	double c_0 = c_00 * (1. - z_d) + c_10 * z_d;
	double c_1 = c_01 * (1. - z_d) + c_11 * z_d;

	double c = c_0 * (1. - f_d) + c_1 * f_d;

	return c * 1_eV / 1_cm3;
}

}}  // namespace hermes::photonfields
