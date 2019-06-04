#ifdef HERMES_HAVE_CFITSIO

#include "hermes/photonField/ISRF.h"
#include "hermes/Common.h"

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <algorithm>

namespace hermes {

	std::string str(const int& n) {
		std::stringstream ss;
		ss << std::setfill('0') << std::setw(3) << n;
		return ss.str();
	}

	ISRF::ISRF() {
		loadFrequencyAxis();
		loadSpatialAxis();
		loadISRF();
	}

	void ISRF::loadFrequencyAxis() {
		double logwl = log10(0.01); // micron
		for (size_t i = 0; i < 200; ++i) {
			logwavelenghts.push_back(logwl);
			logwl += 0.01;
		}
		for (size_t i = 0; i < 680; ++i) {
			logwavelenghts.push_back(logwl);
			logwl += 0.0025;
		}
		for (size_t i = 0; i < 331; ++i) {
			logwavelenghts.push_back(logwl);
			logwl += 0.01;
		}
	}

	void ISRF::loadSpatialAxis() {
		for (auto i: r_id) {
			rs.push_back(static_cast<double>(i) / 10.);
		}
		for (auto i: z_id) {
			zs.push_back(static_cast<double>(i) / 10.);
		}
	}

	int ISRF::getSize() const {
		return isrf.size();
	}

	void ISRF::loadISRF() {
		const int max_num_of_char_in_a_line = 512;
		const int num_of_header_lines = 1;

		int n = 0;
		for (auto i: r_id) {
			for (auto j: z_id) {
				std::ostringstream name;
				name << "RadiationField/Vernetto16/spectrum_r" << str(i) << "_z" << str(j) << ".dat";
				std::string filename = getDataPath(name.str());

				// TODO: exception if file not found
				std::ifstream file_to_read(filename.c_str());
				for(int i = 0; i < num_of_header_lines; ++i)
					file_to_read.ignore(max_num_of_char_in_a_line, '\n');
				while(!file_to_read.eof()) {
					double f_, e_;
					file_to_read >> f_ >> e_;
					if (!file_to_read.eof())
						isrf.push_back(e_);
				}
				n++;
			}
		}
		assert(isrf.size() == rs.size() * zs.size() * logwavelenghts.size());
	}


	QEnergyDensity ISRF::getCMB(QFrequency nu) const {
		QTemperature T_CMB = 2.73_K;
		return (8_pi * h_planck * pow<3>(nu)) / pow<3>(c_light) / (exp(h_planck*nu / (k_boltzmann * T_CMB)) - 1.) * nu;
	}

	double ISRF::getISRF(const int& ir, const int& iz, const int& imu) const {
		long int i = imu + iz * logwavelenghts.size() + ir * (logwavelenghts.size() * zs.size());
		return isrf.at(i);
	}
	
	QEnergyDensity ISRF::getEnergyDensity(const Vector3QLength &pos, const QEnergy &E_photon) const {
		QLength r = sqrt(pos.x*pos.x + pos.y*pos.y);
		QLength z = pos.z;
		return getEnergyDensity(r, z, E_photon);
	}

	QEnergyDensity ISRF::getEnergyDensity(const QLength &r, const QLength &z, const QEnergy &E_photon) const {
		double r_ = static_cast<double>(r / 1_kpc);
		double z_ = static_cast<double>(fabs(z) / 1_kpc);
		double f_mu = static_cast<double>(h_planck * c_light / E_photon / (micro*metre));
		double logf_ = std::log10(f_mu);

		if (r_ < rs.front() || r_ > rs.back())
			return 0;
		if (z_ < zs.front() || z_ > zs.back())
			return 0;
		if (logf_ < logwavelenghts.front() || logf_ > logwavelenghts.back())
			return 0;

		int ir = 0;
		for (int i = 0; i < rs.size(); ++i) {
			if (rs.at(i) < r_)
				ir = i;
		}
		int iz = 0;
		for (int i = 0; i < zs.size(); ++i) {
			if (zs.at(i) < z_)
				iz = i;
		}
		int ifreq = 0;
		for (int i = 0; i < logwavelenghts.size(); ++i) {
			if (logwavelenghts.at(i) < logf_)
				ifreq = i;
		}

		assert(r_ >= rs.at(ir) && r_ <= rs.at(ir + 1));
		assert(z_ >= zs.at(iz) && z_ <= zs.at(iz + 1));
		assert(logf_ >= logwavelenghts.at(ifreq) && logf_ <= logwavelenghts.at(ifreq + 1));

		double r_d = (r_ - rs.at(ir)) / (rs.at(ir + 1) - rs.at(ir));
		double z_d = (z_ - zs.at(iz)) / (zs.at(iz + 1) - zs.at(iz));
		double f_d = (logf_ - logwavelenghts.at(ifreq)) / (logwavelenghts.at(ifreq + 1) - logwavelenghts.at(ifreq));

		assert(r_d >= 0 && r_d <= 1);
		assert(z_d >= 0 && z_d <= 1);
		assert(f_d >= 0 && f_d <= 1);

		double c_00 = getISRF(ir, iz, ifreq)         * (1. - r_d) + getISRF(ir + 1, iz, ifreq)         * r_d;
		double c_01 = getISRF(ir, iz, ifreq + 1)     * (1. - r_d) + getISRF(ir + 1, iz, ifreq + 1)     * r_d;
		double c_10 = getISRF(ir, iz + 1, ifreq)     * (1. - r_d) + getISRF(ir + 1, iz + 1, ifreq)     * r_d;
		double c_11 = getISRF(ir, iz + 1, ifreq + 1) * (1. - r_d) + getISRF(ir + 1, iz + 1, ifreq + 1) * r_d;

		double c_0 = c_00 * (1. - z_d) + c_10 * z_d;
		double c_1 = c_01 * (1. - z_d) + c_11 * z_d;

		double c = c_0 * (1. - f_d) + c_1 * f_d;

		return c * 1_eV / 1_cm3;
	}

} // namespace hermes

#endif // HERMES_HAVE_CFITSIO
