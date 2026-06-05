#include "hermes/darkmatter/PPPC4DMIDSpectrum.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <izstream.hpp>
#include <sstream>
#include <stdexcept>

#include "hermes/Common.h"

#define DEFAULT_DMSPECTRUM_FILE "DarkMatter/PPPC4DMID/AtProduction_gammas.dat.gz"

namespace hermes { namespace darkmatter {

namespace {
constexpr double minLog10X = -8.;
constexpr double maxLog10X = 0.;
constexpr std::size_t log10xSize = 179;
constexpr std::size_t channelCount = 28;
}  // namespace

PPPC4DMIDSpectrum::PPPC4DMIDSpectrum(Channel c, Mass m)
    : filename(getDataPath(DEFAULT_DMSPECTRUM_FILE)), channel(c), mass(m) {
	loadData();
}

PPPC4DMIDSpectrum::PPPC4DMIDSpectrum(Channel c, Mass m, const std::string &filename)
    : filename(filename), channel(c), mass(m) {
	loadData();
}

QEnergy PPPC4DMIDSpectrum::getRestMassEnergy() const { return static_cast<int>(mass) * 1_GeV; }

void PPPC4DMIDSpectrum::loadData() {
	spectrum.clear();
	std::ifstream infile(filename.c_str(), std::ios::binary);
	if (!infile.good()) throw std::runtime_error("hermes::PPPC4DMIDSpectrum: could not open file " + filename);
	zstream::igzstream izstream(infile);

	std::string line;
	std::getline(izstream, line);  // skip first line which is header
	while (std::getline(izstream, line)) {
		std::stringstream stream(line);
		int mDM;
		double log10x, spec;
		if (!(stream >> mDM >> log10x))
			throw std::runtime_error("hermes::PPPC4DMIDSpectrum: malformed row in " + filename);
		if (!std::isfinite(log10x))
			throw std::runtime_error("hermes::PPPC4DMIDSpectrum: non-finite energy grid in " + filename);

		if (mDM != static_cast<int>(mass)) continue;

		for (std::size_t k = 0; k < channelCount; ++k) {
			if (!(stream >> spec))
				throw std::runtime_error("hermes::PPPC4DMIDSpectrum: malformed spectrum row in " + filename);
			if (!std::isfinite(spec) || spec < 0.)
				throw std::runtime_error("hermes::PPPC4DMIDSpectrum: invalid spectrum value in " + filename);
			if (k == static_cast<int>(channel)) spectrum.push_back(std::pair<double, double>(log10x, spec));
		}
	}
	infile.close();

	if (spectrum.size() != log10xSize) throw std::runtime_error("hermes: error: dark matter mass value not found.");
	if (!std::is_sorted(spectrum.cbegin(), spectrum.cend()))
		throw std::runtime_error("hermes::PPPC4DMIDSpectrum: energy grid is not sorted.");
}

QInverseEnergy PPPC4DMIDSpectrum::getParticlesPerEnergy(QEnergy Egamma) const {
	const double x = static_cast<double>(Egamma / getRestMassEnergy());
	if (!std::isfinite(x) || x <= 0. || x > 1.) return QInverseEnergy(0);

	double lgx = std::log10(x);
	if (!std::isfinite(lgx) || lgx < minLog10X || lgx > maxLog10X) return QInverseEnergy(0);

	auto lessThan = [](const std::pair<double, double> &point, double x) { return point.first < x; };

	auto iter = std::lower_bound(spectrum.cbegin(), spectrum.cend(), lgx, lessThan);

	if (iter == spectrum.cend()) return QInverseEnergy(0);
	if (iter == spectrum.cbegin()) return iter->second / M_LN10 / Egamma;
	if (iter->first == lgx) return iter->second / M_LN10 / Egamma;

	double upperX{iter->first};
	double upperY{iter->second};
	double lowerX{(iter - 1)->first};
	double lowerY{(iter - 1)->second};

	double deltaY{upperY - lowerY};
	double deltaX{upperX - lowerX};

	double dNdlog10x = lowerY + ((lgx - lowerX) / deltaX) * deltaY;
	if (!std::isfinite(dNdlog10x) || dNdlog10x <= 0.) return QInverseEnergy(0);
	return dNdlog10x / M_LN10 / Egamma;
}

}}  // namespace hermes::darkmatter
