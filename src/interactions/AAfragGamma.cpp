#include "hermes/interactions/AAfragGamma.h"

#include <cmath>
#include <fstream>
#include <izstream.hpp>
#include <sstream>

#include "hermes/Common.h"

#define DEFAULT_GAMMASPECTRUM_FILE "Interactions/AAfrag2021Gamma.txt.gz"
#define DEFAULT_NUSPECTRUM_FILE "Interactions/AAfrag2021Neutrino.txt.gz"
#define XSIZE 1024
#define YSIZE 768
#define minLogEta -5
#define maxLogEta 0
#define minLogProjEnergy 0
#define maxLogProjEnergy 8

namespace hermes { namespace interactions {

AAfragGamma::AAfragGamma(AAfragParticle particle) : DifferentialCrossSection() {
	// if (particle == AAfragParticle::GAMMA) {
	// 	filename = getDataPath(DEFAULT_GAMMASPECTRUM_FILE);
	// } else {
	filename = getDataPath(DEFAULT_NUSPECTRUM_FILE);
	//}
	loadData();
	for (double x = 1e-4; x < 1; x *= 1.1) {
		std::cout << std::scientific << x << "\t";
		auto E = 5_GeV;
		auto units = 1_mbarn;
		std::cout << E * getAADiffCrossSection(Proton, Proton, E, x * E) / units << "\t";
		E = 20_GeV;
		std::cout << E * getAADiffCrossSection(Proton, Proton, E, x * E) / units << "\t";
		E = 100_GeV;
		std::cout << E * getAADiffCrossSection(Proton, Proton, E, x * E) / units << "\t";
		E = 1_TeV;
		std::cout << E * getAADiffCrossSection(Proton, Proton, E, x * E) / units << "\t";
		E = 10_TeV;
		std::cout << E * getAADiffCrossSection(Proton, Proton, E, x * E) / units << "\t";
		std::cout << "\n";
	}
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
		double T, x, xs_pp_i, xs_phe_i, xs_hep_i, xs_hehe_i;
		stream >> T >> x >> xs_pp_i >> xs_phe_i >> xs_hep_i >> xs_hehe_i;
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
	auto x = std::log10(static_cast<double>(E_proton / 1_GeV));
	auto y = std::log10(static_cast<double>(E_gamma / E_proton));
	if (x < minLogProjEnergy || x > maxLogProjEnergy) return QDiffCrossSection(0.);
	if (y < minLogEta || y > maxLogEta) return QDiffCrossSection(0.);

	// linear fraction to lower and upper neighbors
	auto fx = (x - minLogProjEnergy) / (maxLogProjEnergy - minLogProjEnergy);
	auto fX = 1 - fx;
	auto fy = (y - minLogEta) / (maxLogEta - minLogEta);
	auto fY = 1 - fy;

	// lower-left point coordinates
	size_t ix = floor(fx * (XSIZE - 1));
	size_t iy = floor(fy * (YSIZE - 1));

	// bilinear interpolation
	auto indx = [](size_t i, size_t j) { return j + YSIZE * i; };

	auto value = 0.;
	//   f(0,0) (1 - x) (1 - y) +
	value += xs_pp.at(indx(ix, iy)) * fX * fY;
	//   f(1,0) x (1 - y) +
	value += xs_pp.at(indx(ix + 1, iy)) * fx * fY;
	//   f(0,1) (1 - x) y +
	value += xs_pp.at(indx(ix, iy + 1)) * fX * fy;
	//   f(1,1) x y +
	value += xs_pp.at(indx(ix + 1, iy + 1)) * fx * fy;

	return value * 1_mbarn / 1_GeV;
}

}}  // namespace hermes::interactions
