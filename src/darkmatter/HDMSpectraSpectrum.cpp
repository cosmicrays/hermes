#include "hermes/darkmatter/HDMSpectraSpectrum.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <izstream.hpp>
#include <map>
#include <sstream>
#include <stdexcept>

#include "hermes/Common.h"

namespace hermes { namespace darkmatter {

namespace {
constexpr double decayEnergyFractionFactor = 2.;
constexpr double tolerance = 1e-12;
constexpr const char *hdmSpectraEnv = "HERMES_HDMSPECTRA_DATA_PATH";

std::string productFilename(HDMSpectraProduct product) {
	switch (product) {
		case HDMSpectraProduct::gamma:
			return "AtProduction_gammas.dat.gz";
		case HDMSpectraProduct::neutrino:
			return "AtProduction_neutrinos.dat.gz";
		case HDMSpectraProduct::proton:
			return "AtProduction_protons.dat.gz";
		case HDMSpectraProduct::neutron:
			return "AtProduction_neutrons.dat.gz";
	}
	throw std::invalid_argument("hermes::HDMSpectraSpectrum: unknown product.");
}

std::string joinPath(const std::string &directory, const std::string &filename) {
	if (directory.empty()) return filename;
	const char last = directory[directory.size() - 1];
	if (last == '/' || last == '\\') return directory + filename;
	return directory + "/" + filename;
}

bool fileExists(const std::string &path) {
	std::ifstream file(path.c_str(), std::ios::binary);
	return file.good();
}

std::string defaultFilename(HDMSpectraProduct product) {
	const std::string table = productFilename(product);
	const char *envPath = std::getenv(hdmSpectraEnv);
	if (envPath != nullptr && std::string(envPath).size() > 0)
		return joinPath(envPath, table);

	const std::string relativePath = "DarkMatter/HDMSpectra/" + table;
	const std::string hermesDataPath = getDataPath(relativePath);
	if (fileExists(hermesDataPath)) return hermesDataPath;

	const std::string localDataPath = joinPath("data", relativePath);
	if (fileExists(localDataPath)) return localDataPath;

	const std::string buildDataPath = joinPath("build/data", relativePath);
	if (fileExists(buildDataPath)) return buildDataPath;

	return hermesDataPath;
}

std::string channelName(Channel channel) {
	switch (channel) {
		case Channel::e:
			return "e";
		case Channel::mu:
			return "mu";
		case Channel::tau:
			return "tau";
		case Channel::q:
			return "q";
		case Channel::c:
			return "c";
		case Channel::b:
			return "b";
		case Channel::t:
			return "t";
		case Channel::W:
			return "W";
		case Channel::Z:
			return "Z";
		case Channel::g:
			return "g";
		case Channel::gamma:
			return "gamma";
		case Channel::h:
			return "h";
	}
	throw std::invalid_argument("hermes::HDMSpectraSpectrum: unknown channel.");
}

bool endsWith(const std::string &value, const std::string &suffix) {
	return value.size() >= suffix.size() &&
	       value.compare(value.size() - suffix.size(), suffix.size(), suffix) == 0;
}

bool closeTo(double lhs, double rhs) {
	const double difference = std::fabs(lhs - rhs);
	const double scale = std::max(1., std::max(std::fabs(lhs), std::fabs(rhs)));
	return difference <= tolerance * scale;
}

std::vector<std::string> readColumns(const std::string &line) {
	std::stringstream stream(line);
	std::vector<std::string> columns;
	std::string column;
	while (stream >> column) columns.push_back(column);
	return columns;
}

std::size_t channelColumn(const std::vector<std::string> &columns,
	                       Channel channel, const std::string &filename) {
	const std::string selectedChannel = channelName(channel);
	for (std::size_t i = 2; i < columns.size(); ++i) {
		if (columns[i] == selectedChannel) return i;
	}
	throw std::runtime_error("hermes::HDMSpectraSpectrum: channel " +
	                         selectedChannel + " not found in " + filename);
}

void validateTableHeader(const std::vector<std::string> &columns,
                         const std::string &filename) {
	if (columns.size() < 3 || columns[0] != "mDM_GeV" ||
	    columns[1] != "Log[10,x]")
		throw std::runtime_error(
		    "hermes::HDMSpectraSpectrum: malformed header in " + filename);
}

}  // namespace

HDMSpectraSpectrum::HDMSpectraSpectrum(Channel c, QEnergy m,
                                       HDMSpectraProduct p)
    : filename(defaultFilename(p)), channel(c), product(p), restMassEnergy(m) {
	loadData();
	validateRequestedMass();
}

HDMSpectraSpectrum::HDMSpectraSpectrum(Channel c, QEnergy m,
                                       HDMSpectraProduct p,
                                       const std::string &filename)
    : filename(filename), channel(c), product(p), restMassEnergy(m) {
	loadData();
	validateRequestedMass();
}

Channel HDMSpectraSpectrum::getChannel() const { return channel; }

HDMSpectraProduct HDMSpectraSpectrum::getProduct() const { return product; }

const std::string &HDMSpectraSpectrum::getFilename() const { return filename; }

QEnergy HDMSpectraSpectrum::getRestMassEnergy() const {
	return restMassEnergy;
}

double HDMSpectraSpectrum::getRestMassEnergyGeV() const {
	return static_cast<double>(restMassEnergy / 1_GeV);
}

void HDMSpectraSpectrum::loadData() {
	spectra.clear();
	std::ifstream infile(filename.c_str(), std::ios::binary);
	if (!infile.good())
		throw std::runtime_error(
		    "hermes::HDMSpectraSpectrum: could not open file " + filename);

	if (endsWith(filename, ".gz")) {
		zstream::igzstream izstream(infile);
		loadDataFromStream(izstream);
	} else {
		loadDataFromStream(infile);
	}
}

void HDMSpectraSpectrum::loadDataFromStream(std::istream &input) {
	std::string line;
	if (!std::getline(input, line))
		throw std::runtime_error(
		    "hermes::HDMSpectraSpectrum: empty file " + filename);

	const auto columns = readColumns(line);
	validateTableHeader(columns, filename);
	const std::size_t selectedColumn = channelColumn(columns, channel, filename);

	std::map<double, std::vector<std::pair<double, double> > > byMass;
	while (std::getline(input, line)) {
		if (line.empty() || line[0] == '#') continue;

		std::stringstream stream(line);
		double massGeV;
		double log10x;
		if (!(stream >> massGeV >> log10x))
			throw std::runtime_error(
			    "hermes::HDMSpectraSpectrum: malformed row in " + filename);
		if (!std::isfinite(massGeV) || massGeV <= 0. ||
		    !std::isfinite(log10x))
			throw std::runtime_error(
			    "hermes::HDMSpectraSpectrum: non-finite row in " + filename);

		double selectedValue = 0.;
		for (std::size_t i = 2; i < columns.size(); ++i) {
			double value;
			if (!(stream >> value))
				throw std::runtime_error(
				    "hermes::HDMSpectraSpectrum: malformed spectrum row in " +
				    filename);
			if (i == selectedColumn) selectedValue = value;
		}

		std::string extra;
		if (stream >> extra)
			throw std::runtime_error(
			    "hermes::HDMSpectraSpectrum: too many columns in " + filename);
		if (!std::isfinite(selectedValue) || selectedValue < 0.)
			throw std::runtime_error(
			    "hermes::HDMSpectraSpectrum: invalid spectrum value in " +
			    filename);

		byMass[massGeV].push_back({log10x, selectedValue});
	}

	if (byMass.empty())
		throw std::runtime_error(
		    "hermes::HDMSpectraSpectrum: no spectrum rows found in " + filename);

	std::vector<double> referenceGrid;
	for (auto &entry : byMass) {
		auto &points = entry.second;
		std::sort(points.begin(), points.end());
		if (points.empty())
			throw std::runtime_error(
			    "hermes::HDMSpectraSpectrum: empty mass block in " + filename);

		for (std::size_t i = 1; i < points.size(); ++i) {
			if (closeTo(points[i - 1].first, points[i].first))
				throw std::runtime_error(
				    "hermes::HDMSpectraSpectrum: duplicated x grid point in " +
				    filename);
		}

		if (referenceGrid.empty()) {
			for (const auto &point : points) referenceGrid.push_back(point.first);
		} else {
			if (referenceGrid.size() != points.size())
				throw std::runtime_error(
				    "hermes::HDMSpectraSpectrum: inconsistent x grid in " +
				    filename);
			for (std::size_t i = 0; i < referenceGrid.size(); ++i) {
				if (!closeTo(referenceGrid[i], points[i].first))
					throw std::runtime_error(
					    "hermes::HDMSpectraSpectrum: inconsistent x grid in " +
					    filename);
			}
		}

		spectra.push_back(
		    {entry.first, std::log10(entry.first), std::move(points)});
	}
}

void HDMSpectraSpectrum::validateRequestedMass() const {
	const double massGeV = getRestMassEnergyGeV();
	if (!std::isfinite(massGeV) || massGeV <= 0.)
		throw std::invalid_argument(
		    "hermes::HDMSpectraSpectrum: rest mass energy must be positive.");
	if (spectra.empty())
		throw std::runtime_error(
		    "hermes::HDMSpectraSpectrum: no loaded spectra.");

	const double logMass = std::log10(massGeV);
	const double logMin = spectra.front().log10MassGeV;
	const double logMax = spectra.back().log10MassGeV;
	if (logMass < logMin && !closeTo(logMass, logMin))
		throw std::invalid_argument(
		    "hermes::HDMSpectraSpectrum: rest mass energy is below table range.");
	if (logMass > logMax && !closeTo(logMass, logMax))
		throw std::invalid_argument(
		    "hermes::HDMSpectraSpectrum: rest mass energy is above table range.");
}

double HDMSpectraSpectrum::interpolateInX(
    const MassSpectrum &massSpectrum, double log10x) const {
	const auto &points = massSpectrum.spectrum;
	if (points.empty() || !std::isfinite(log10x)) return 0.;
	if (log10x < points.front().first && !closeTo(log10x, points.front().first))
		return 0.;
	if (log10x > points.back().first && !closeTo(log10x, points.back().first))
		return 0.;

	auto lessThan = [](const std::pair<double, double> &point, double x) {
		return point.first < x;
	};
	auto upper = std::lower_bound(points.cbegin(), points.cend(), log10x,
	                              lessThan);
	if (upper == points.cend()) return points.back().second;
	if (upper == points.cbegin() || closeTo(upper->first, log10x))
		return upper->second;

	const auto lower = upper - 1;
	const double fraction =
	    (log10x - lower->first) / (upper->first - lower->first);
	return lower->second + fraction * (upper->second - lower->second);
}

double HDMSpectraSpectrum::interpolate(double log10x) const {
	const double logMass = std::log10(getRestMassEnergyGeV());
	auto lessThan = [](const MassSpectrum &spectrum, double mass) {
		return spectrum.log10MassGeV < mass;
	};
	auto upper =
	    std::lower_bound(spectra.cbegin(), spectra.cend(), logMass, lessThan);

	if (upper == spectra.cend()) return interpolateInX(spectra.back(), log10x);
	if (upper == spectra.cbegin() || closeTo(upper->log10MassGeV, logMass))
		return interpolateInX(*upper, log10x);

	const auto lower = upper - 1;
	const double lowerValue = interpolateInX(*lower, log10x);
	const double upperValue = interpolateInX(*upper, log10x);
	const double fraction =
	    (logMass - lower->log10MassGeV) /
	    (upper->log10MassGeV - lower->log10MassGeV);
	return lowerValue + fraction * (upperValue - lowerValue);
}

QInverseEnergy HDMSpectraSpectrum::getParticlesPerEnergy(QEnergy energy) const {
	const double x =
	    decayEnergyFractionFactor * static_cast<double>(energy / restMassEnergy);
	if (!std::isfinite(x) || x <= 0. || x > 1.) return QInverseEnergy(0);

	const double log10x = std::log10(x);
	const double dNdlog10x = interpolate(log10x);
	if (!std::isfinite(dNdlog10x) || dNdlog10x <= 0.)
		return QInverseEnergy(0);

	return dNdlog10x / (std::log(10.) * energy);
}

}}  // namespace hermes::darkmatter
