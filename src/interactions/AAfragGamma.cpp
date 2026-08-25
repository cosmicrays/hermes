#include "hermes/interactions/AAfragGamma.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <izstream.hpp>
#include <sstream>

#include "hermes/Common.h"

namespace {

constexpr std::size_t xSize = 1024;
constexpr std::size_t ySize = 768;
constexpr double minLogEta = -5;
constexpr double maxLogEta = 0;
constexpr double minLogProjEnergy = 0;
constexpr double maxLogProjEnergy = 8;

}  // namespace

namespace hermes { namespace interactions {

AAfragXsecs::AAfragXsecs(const std::string &filename) : DifferentialCrossSection() { loadData(getDataPath(filename)); }

void AAfragXsecs::loadData(const std::string &filename) {
	std::ifstream infile(filename.c_str());
	if (!infile.good()) throw std::runtime_error("hermes::AAfragGamma: could not open file " + filename);
	zstream::igzstream izstream(infile);
	std::string line;
	std::getline(izstream, line);  // skip first line which is header
	while (std::getline(izstream, line)) {
		std::stringstream stream(line);
		double T, x, xs_pp_i, xs_phe_i, xs_hep_i, xs_hehe_i;
		if (!(stream >> T >> x >> xs_pp_i >> xs_phe_i >> xs_hep_i >> xs_hehe_i))
			throw std::runtime_error("hermes::AAfragGamma: malformed row in " + filename);
		xs_pp.push_back(xs_pp_i);
		xs_pHe.push_back(xs_phe_i);
		xs_Hep.push_back(xs_hep_i);
		xs_HeHe.push_back(xs_hehe_i);
	}
	const std::size_t expectedSize = xSize * ySize;
	if (xs_pp.size() != expectedSize || xs_pHe.size() != expectedSize ||
	    xs_Hep.size() != expectedSize || xs_HeHe.size() != expectedSize)
		throw std::runtime_error("hermes: error in reading AAfragGamma table.");
}

QDiffCrossSection AAfragXsecs::getDiffCrossSection(const QEnergy &E_proton, const QEnergy &E_gamma) const {
	return getDiffCrossSection(Proton, Proton, E_proton, E_gamma);
}

QDiffCrossSection AAfragXsecs::getDiffCrossSection(const PID &projectile, const PID &target, const QEnergy &E_proj,
                                                   const QEnergy &E_secondary) const {
	if (!(E_proj > QEnergy(0)) || !(E_secondary > QEnergy(0)))
		return QDiffCrossSection(0);

	const std::vector<double> *table = nullptr;
	if (projectile == Proton && target == Proton)
		table = &xs_pp;
	else if (projectile == Proton && target == Helium)
		table = &xs_pHe;
	else if (projectile == Helium && target == Proton)
		table = &xs_Hep;
	else if (projectile == Helium && target == Helium)
		table = &xs_HeHe;
	else
		return QDiffCrossSection(0);

	const double x = std::log10(static_cast<double>(E_proj / 1_GeV));
	const double y = std::log10(static_cast<double>(E_secondary / E_proj));
	if (x < minLogProjEnergy || x > maxLogProjEnergy) return QDiffCrossSection(0.);
	if (y < minLogEta || y > maxLogEta) return QDiffCrossSection(0.);

	const double gridX = (x - minLogProjEnergy) /
	                     (maxLogProjEnergy - minLogProjEnergy) * (xSize - 1);
	const double gridY = (y - minLogEta) /
	                     (maxLogEta - minLogEta) * (ySize - 1);
	const std::size_t ix = std::min<std::size_t>(
	    static_cast<std::size_t>(std::floor(gridX)), xSize - 2);
	const std::size_t iy = std::min<std::size_t>(
	    static_cast<std::size_t>(std::floor(gridY)), ySize - 2);
	const double fractionX = gridX - static_cast<double>(ix);
	const double fractionY = gridY - static_cast<double>(iy);

	// bilinear interpolation
	const auto index = [](std::size_t i, std::size_t j) { return j + ySize * i; };
	const double lower = table->at(index(ix, iy)) * (1 - fractionY) +
	                     table->at(index(ix, iy + 1)) * fractionY;
	const double upper = table->at(index(ix + 1, iy)) * (1 - fractionY) +
	                     table->at(index(ix + 1, iy + 1)) * fractionY;
	const double value = lower * (1 - fractionX) + upper * fractionX;
	return value * 1_mbarn / 1_GeV;
}

}}  // namespace hermes::interactions
