#include "hermes/Common.h"

#include <cstdlib>
#include <fstream>
#include <limits>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

#include "kiss/logger.h"
#include "kiss/path.h"

namespace hermes {

namespace {

bool fileExists(const std::string &path) {
	std::ifstream file(path.c_str(), std::ios::binary);
	return file.good();
}

std::string findDataPath(const std::vector<std::pair<std::string, std::string>> &candidates,
                         const std::string &filename) {
	for (const auto &candidate : candidates) {
		const auto &path = candidate.second;
		if (!is_directory(path)) continue;
		if (!fileExists(concat_path(path, filename))) continue;

		KISS_LOG_INFO << "getDataPath: use " << candidate.first << ", " << path << std::endl;
		return path;
	}

	for (const auto &candidate : candidates) {
		const auto &path = candidate.second;
		if (!is_directory(path)) continue;

		KISS_LOG_INFO << "getDataPath: use " << candidate.first << ", " << path << std::endl;
		return path;
	}

	return "data";
}

}  // namespace

std::string getDataPath(const std::string &filename) {
	// adopted from CRPropa3
	static std::mutex dataPathMutex;
	static std::string dataPath;
	std::lock_guard<std::mutex> guard(dataPathMutex);
	if (dataPath.size() && fileExists(concat_path(dataPath, filename))) return concat_path(dataPath, filename);

	std::vector<std::pair<std::string, std::string>> candidates;

	const char *env_path = getenv("HERMES_DATA_PATH");
	if (env_path) candidates.push_back({"environment variable", env_path});

#ifdef HERMES_INSTALL_DATA_RELATIVE_TO_LIBRARY
	const std::string libraryPath = library_path();
	if (!libraryPath.empty()) {
		candidates.push_back({"library path", concat_path(libraryPath, HERMES_INSTALL_DATA_RELATIVE_TO_LIBRARY)});
	}
#endif

#ifdef HERMES_INSTALL_PREFIX
	candidates.push_back({"install prefix", HERMES_INSTALL_PREFIX "/share/hermes/data"});
#endif

#ifdef HERMES_BUILD_DATA_PATH
	candidates.push_back({"cmake build data", HERMES_BUILD_DATA_PATH});
#endif

	candidates.push_back({"build tree", "build/data"});
	candidates.push_back({"build tree", "../build/data"});
	candidates.push_back({"default", "data"});
	candidates.push_back({"default", "../data"});

	try {
		const std::string exePath = executable_path();
		if (!exePath.empty()) {
			candidates.push_back({"executable path", exePath + "data"});
			candidates.push_back({"executable path", exePath + "../data"});
			candidates.push_back({"executable path", exePath + "../build/data"});
		}
	} catch (const std::exception &) {
	}

	dataPath = findDataPath(candidates, filename);
	return concat_path(dataPath, filename);
}

bool isWithinAngle(const QDirection &a, const QDirection &b, const QAngle &d) {
	Vector3d v1, v2;
	v1.setRThetaPhi(1, a[0], a[1]);
	v2.setRThetaPhi(1, b[0], b[1]);
	return (v1.getAngleTo(v2) < d);
}

QLength distanceFromGC(const QDirection &direction, const QLength &distFromObserver, const Vector3QLength &vecGCObs) {
	Vector3QLength vecObsTarget;
	vecObsTarget.setRThetaPhi(distFromObserver, direction[0], direction[1]);
	Vector3QLength vecGCTarget = vecObsTarget - vecGCObs;

	return vecGCTarget.getR();
}

QLength distanceToGalBorder(const Vector3QLength &observerPosition, const QDirection &direction,
                            const QLength &galacticBorder, const QLength &zBorder) {
	static const Vector3QLength gcPosition(0, 0, 0);
	// static const QLength galacticBorder = 30_kpc; // for example JF12 is
	// zero for r > 20kpc static const QLength zBorder = 5_kpc;
	Vector3QLength vecObsToGalBorder;
	vecObsToGalBorder.setRThetaPhi(1_m, direction[0], direction[1]);

	QLength a = (gcPosition - observerPosition).getR();
	QLength c = galacticBorder;
	QAngle gamma = vecObsToGalBorder.getAngleTo(observerPosition - gcPosition);

	QLength sphericalBorder = a * cos(gamma) + sqrt(c * c - a * a * (1 - cos(2 * gamma)) / 2.0);
	QLength heightBroder = fabs(zBorder / cos(direction[0]));

	return std::min(heightBroder, sphericalBorder);
}

Vector3QLength getGalacticPosition(const Vector3QLength &observerPosition, const QLength &dist, const QDirection &dir) {
	Vector3QLength pos(0);

	pos.setRThetaPhi(dist, dir[0], dir[1]);
	pos.x = observerPosition.x - pos.x;
	pos.y = observerPosition.y - pos.y;
	pos.z = observerPosition.z + pos.z;

	return pos;
}

QDirection toGalCoord(const QDirection &d) { return QDirection({pi * 0.5_rad - fmod(d[0], pi), fmod(d[1], 2_pi)}); }

QDirection fromGalCoord(const QDirection &d) { return QDirection({fmod(pi * 0.5_rad - d[0], pi), fmod(d[1], 2_pi)}); }

QNumber getLorentzFactor(const QMass &m, const QEnergy &E) { return E / (m * c_squared); }

QTemperature intensityToTemperature(const QIntensity &intensity_, const QFrequency &freq_) {
	return intensity_ * c_squared / (2 * freq_ * freq_ * k_boltzmann);
}

unsigned int getThreadsNumber() {
	// From std::thread docs: the value should be considered only a hint
	const unsigned int hardware_threads = std::thread::hardware_concurrency();
	const unsigned int max_threads = std::max(1u, hardware_threads);

	const char *env_num_threads = getenv("HERMES_NUM_THREADS");
	if (env_num_threads) {
		char *end = nullptr;
		const unsigned long requested = std::strtoul(env_num_threads, &end, 10);
		if (end != env_num_threads && *end == '\0' && requested > 0) {
			return static_cast<unsigned int>(std::min<unsigned long>(requested, max_threads));
		}
	}

	return max_threads;
}

std::size_t getThreadId() { return std::hash<std::thread::id>()(std::this_thread::get_id()); }

std::vector<std::vector<std::size_t>> getIndexedThreadChunks(std::vector<std::size_t> validPixels) {
	if (validPixels.empty()) return {};

	const std::size_t threads = std::min<std::size_t>(getThreadsNumber(), validPixels.size());
	std::vector<std::vector<std::size_t>> chunks(threads);

	while (validPixels.size()) {
		for (std::size_t i = 0; i < threads; ++i) {
			chunks[i].push_back(validPixels.back());
			validPixels.pop_back();
			if (validPixels.size() == 0) {
				break;
			}
		}
	}

	return chunks;
}

std::vector<std::pair<unsigned int, unsigned int>> getThreadChunks(unsigned int queueSize) {
	if (queueSize == 0) return {};

	const unsigned int threads = std::min(getThreadsNumber(), queueSize);
	const unsigned int tasks_per_thread = queueSize / threads;
	const unsigned int remainder = queueSize % threads;
	std::vector<std::pair<unsigned int, unsigned int>> chunks;
	chunks.reserve(threads);
	unsigned int begin = 0;
	for (unsigned int i = 0; i < threads; ++i) {
		const unsigned int chunk_size = tasks_per_thread + (i < remainder ? 1u : 0u);
		chunks.emplace_back(begin, begin + chunk_size);
		begin += chunk_size;
	}

	return chunks;
}

}  // namespace hermes
