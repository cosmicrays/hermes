#include "hermes/darkmatter/PPPC4DMIDSpectrum.h"

#include <fstream>
#include <izstream.hpp>
#include <sstream>

#include "hermes/Common.h"

#define DEFAULT_DMSPECTRUM_FILE \
	"DarkMatter/PPPC4DMID/AtProduction_gammas.dat.gz"

namespace hermes { namespace darkmatter {

PPPC4DMIDSpectrum::PPPC4DMIDSpectrum(Channel c, Mass m)
    : channel(c), mass(m), filename(getDataPath(DEFAULT_DMSPECTRUM_FILE)) {
	loadData();
}

PPPC4DMIDSpectrum::PPPC4DMIDSpectrum(Channel c, Mass m,
                                     const std::string &filename)
    : channel(c), mass(m), filename(filename) {
	loadData();
}

QEnergy PPPC4DMIDSpectrum::getRestMassEnergy() const {
	return static_cast<int>(mass) * 1_GeV;
}

void PPPC4DMIDSpectrum::loadData() {
	std::ifstream infile(filename.c_str());
	if (!infile.good())
		throw std::runtime_error(
		    "hermes::PPPC4DMIDSpectrum: could not open file " + filename);
	std::istream *in = &infile;
	auto izstream = new zstream::igzstream(*in);

	std::string line;
	std::getline(*izstream, line);  // skip first line which is header
	while (std::getline(*izstream, line)) {
		std::stringstream stream(line);
		int mDM;
		double log10x, spec;
		stream >> mDM >> log10x;

		if (mDM != static_cast<int>(mass)) continue;

		for (std::size_t k = 0; k < 28; ++k) {
			stream >> spec;
			if (k == static_cast<int>(channel))
				spectrum.push_back(std::pair<double, double>(log10x, spec));
		}
	}
	infile.close();

	constexpr int log10xSize = 179;
	if (spectrum.size() != log10xSize)
		throw std::runtime_error(
		    "hermes: error: dark matter mass value not found.");
}

QInverseEnergy PPPC4DMIDSpectrum::getParticlesPerEnergy(QEnergy Egamma) const {
	double lgx = std::log10(static_cast<double>(Egamma / getRestMassEnergy()));

	if (lgx < -8. || lgx > 0.) return QInverseEnergy(0);

	auto lessThan = [](const std::pair<double, double> &point, double x) {
		return point.first < x;
	};

	auto iter =
	    std::lower_bound(spectrum.cbegin(), spectrum.cend(), lgx, lessThan);
    
	double upperX{iter->first};
	double upperY{iter->second};
	double lowerX{(iter - 1)->first};
	double lowerY{(iter - 1)->second};

	double deltaY{upperY - lowerY};
	double deltaX{upperX - lowerX};

	double dNdlog10x = lowerY + ((lgx - lowerX) / deltaX) * deltaY;
	return dNdlog10x / M_LN10 / Egamma;
}

}}  // namespace hermes::darkmatter
