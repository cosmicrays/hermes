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
#include <iterator>

namespace hermes {

	std::string str(const int& n) {
		std::stringstream ss;
		ss << std::setfill('0') << std::setw(3) << n;
		return ss.str();
	}

	ISRF::ISRF() {
		loadFrequencyAxis();
		
		auto logWavelenghtToFrequency = [](double lambda){
			return c_light / (std::pow(10, lambda) * micrometre); };
    		setStartEnergy(
			logWavelenghtToFrequency(logwavelenghts.back()) * h_planck);
    		setEndEnergy(
			logWavelenghtToFrequency(logwavelenghts.front()) * h_planck);
		
		// Spares steps
		setEnergyScaleFactor(1.1); // 145 steps

		// Alternative (slow), all available energy steps	
		/*
		double scaling = std::pow(static_cast<double>(
					getEndEnergy()/getStartEnergy()),
				1.0/logwavelenghts.size()); // ~1.01
		setEnergyScaleFactor(scaling); // 1211 steps
		*/
		
		buildEnergyRange();
		loadISRF();
	}
    
	void ISRF::buildEnergyRange() {
    		const double scaling = getEnergyScaleFactor();
		const QEnergy E_start = getStartEnergy();
		const QEnergy E_end = getEndEnergy();
	
		for(QEnergy E = E_start; E < E_end; E = E * scaling)  
    		energyRange.push_back(E);
    	}

	void ISRF::loadFrequencyAxis() {
		double logwl = log10(0.01); // micron
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
				name << "RadiationField/Vernetto16/spectrum_r" << str(i*10) << "_z" << str(j*10) << ".dat";
				std::string filename = getDataPath(name.str());

				// TODO: exception if file not found
				std::ifstream file_to_read(filename.c_str());
				for(int k = 0; k < num_of_header_lines; ++k)
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
		assert(isrf.size() == r_id.size() * z_id.size() * logwavelenghts.size());
	}

	double ISRF::getISRF(const int& ir, const int& iz, const int& imu) const {
		long int i = imu + iz * logwavelenghts.size() + ir * (logwavelenghts.size() * z_id.size());
		return isrf[i];
	}
    
	QEnergyDensity ISRF::getEnergyDensity(const Vector3QLength &pos, int iE) const {
		QLength r = sqrt(pos.x*pos.x + pos.y*pos.y);
		QLength z = pos.z;
		QEnergy E = energyRange[iE];
		// TODO: not implemented
		return getEnergyDensity(r, z, E);
	}
	
	QEnergyDensity ISRF::getEnergyDensity(const Vector3QLength &pos, const QEnergy &E_photon) const {
		QLength r = sqrt(pos.x*pos.x + pos.y*pos.y);
		QLength z = pos.z;
		return getEnergyDensity(r, z, E_photon);
	}

	QEnergyDensity ISRF::getEnergyDensity(const QLength &r, const QLength &z, const QEnergy &E_photon) const {
		double r_ = static_cast<double>(r / 1_kpc);
		double z_ = static_cast<double>(fabs(z) / 1_kpc);
		double f_mu = static_cast<double>(h_planck * c_light / E_photon / (micrometre));
		double logf_ = std::log10(f_mu);

		if (r_ < r_id.front() || r_ > r_id.back())
			return 0;
		if (z_ < z_id.front() || z_ > z_id.back())
			return 0;
		if (logf_ < logwavelenghts.front() || logf_ > logwavelenghts.back())
			return 0;


		int ir = std::lower_bound(r_id.begin(), r_id.end(), r_) - r_id.begin();
		int iz = std::lower_bound(z_id.begin(), z_id.end(), z_) - z_id.begin();
		int ifreq = std::lower_bound(logwavelenghts.begin(), logwavelenghts.end(), logf_) - logwavelenghts.begin() - 1;

		if (ir == r_id.size())
			return 0;
		if (iz == z_id.size())
			return 0;
		if (ifreq == logwavelenghts.size())
			return 0;

		double r_d = (r_ - r_id[ir]) / (r_id[ir + 1] - r_id[ir]);
		double z_d = (z_ - z_id[iz]) / (z_id[iz + 1] - z_id[iz]);
		double f_d = (logf_ - logwavelenghts[ifreq]) / (logwavelenghts[ifreq + 1] - logwavelenghts[ifreq]);

		/*
		if (!(r_d >= 0 && r_d <= 1))
			return 0;
		if (!(z_d >= 0 && z_d <= 1))
			return 0;
		if (!(f_d >= 0 && f_d <= 1))
			return 0;
		*/

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
