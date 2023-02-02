#include "hermes/interactions/AAfragGamma.h"

#include <cmath>
#include <fstream>
#include <izstream.hpp>
#include <sstream>

#include "hermes/Common.h"

#define DEFAULT_GAMMASPECTRUM_FILE "Interactions/AAfrag2021Gamma.txt.gz"
#define DEFAULT_NUSPECTRUM_FILE "Interactions/AAfrag2021Neutrino.txt.gz"
#define XSIZE 100
#define YSIZE 200
#define minLogX -5
#define maxLogX 0
#define minLogProjEnergy 0
#define maxLogProjEnergy 8

namespace hermes { namespace interactions {

AAfragGamma::AAfragGamma(AAfragParticle particle) : DifferentialCrossSection() {
	if (particle == AAfragParticle::GAMMA) {
		filename = getDataPath(DEFAULT_GAMMASPECTRUM_FILE);
	} else {
		filename = getDataPath(DEFAULT_NUSPECTRUM_FILE);
	}
	loadData();
}

void AAfragGamma::loadData() {
	std::ifstream infile(filename.c_str());
	if (!infile.good()) throw std::runtime_error("hermes::AAfragGamma: could not open file " + filename);
	std::istream *in = &infile;
	auto izstream = new zstream::igzstream(*in);
	std::string line;
	std::getline(*izstream, line);  // skip first line which is header
	while (std::getline(*izstream, line)) {
		std::stringstream stream(line);
		double E, x, xs_pp_i, xs_phe_i, xs_hep_i, xs_hehe_i;
		stream >> E >> x >> xs_pp_i >> xs_phe_i >> xs_hep_i >> xs_hehe_i;
		xs_pp.push_back(xs_pp_i);
		xs_pHe.push_back(xs_phe_i);
		xs_Hep.push_back(xs_hep_i);
		xs_HeHe.push_back(xs_hehe_i);
	}
	infile.close();
	if (xs_pp.size() != XSIZE * YSIZE) throw std::runtime_error("hermes: error in reading AAfragGamma.");
}

QDiffCrossSection AAfragGamma::getDiffCrossSection(const QEnergy &E_proton, const QEnergy &E_gamma) const {
	if (E_gamma > E_proton) return QDiffCrossSection(0);
	return QDiffCrossSection(0.);
}

QDiffCrossSection AAfragGamma::getAADiffCrossSection(const PID &projectile, const PID &target, const QEnergy &E_proton,
                                                     const QEnergy &E_gamma) const {
	auto logProjEnergy = std::log10(static_cast<double>(E_proton / 1_GeV));
	auto logX = std::log10(static_cast<double>(E_gamma / E_proton));
	if (logProjEnergy < minLogProjEnergy || logProjEnergy > maxLogProjEnergy) return QDiffCrossSection(0.);
	if (logX < minLogX || logX > maxLogX) return QDiffCrossSection(0.);
	size_t i = (logX - minLogX) / (maxLogX - minLogX) * XSIZE;
	size_t j = (logProjEnergy - minLogProjEnergy) / (maxLogProjEnergy - minLogProjEnergy) * YSIZE;
	return QDiffCrossSection(0.);
}

}}  // namespace hermes::interactions
