#include "hermes/Common.h"

#include <cstdlib>
#include <thread>

#include "kiss/logger.h"
#include "kiss/path.h"

namespace hermes {

std::string getDataPath(const std::string &filename) {
	// adopted from CRPropa3
	static std::string dataPath;
	if (dataPath.size()) return concat_path(dataPath, filename);

	const char *env_path = getenv("HERMES_DATA_PATH");
	if (env_path) {
		if (is_directory(env_path)) {
			dataPath = env_path;
			KISS_LOG_INFO << "getDataPath: use environment variable, "
			              << dataPath << std::endl;
			return concat_path(dataPath, filename);
		}
	}

#ifdef HERMES_INSTALL_PREFIX
	{
		std::string _path = HERMES_INSTALL_PREFIX "/share/hermes/data";
		if (is_directory(_path)) {
			dataPath = _path;
			KISS_LOG_INFO << "getDataPath: use install prefix, " << dataPath
			              << std::endl;
			return concat_path(dataPath, filename);
		}
	}
#endif

	{
		std::string _path = executable_path() + "../data";
		if (is_directory(_path)) {
			dataPath = _path;
			KISS_LOG_INFO << "getDataPath: use executable path, " << dataPath
			              << std::endl;
			return concat_path(dataPath, filename);
		}
	}

	dataPath = "data";
	KISS_LOG_INFO << "getDataPath: use default, " << dataPath << std::endl;
	return concat_path(dataPath, filename);
}

bool isWithinAngle(const QDirection &a, const QDirection &b, const QAngle &d) {
	Vector3d v1, v2;
	v1.setRThetaPhi(1, a[0], a[1]);
	v2.setRThetaPhi(1, b[0], b[1]);
	return (v1.getAngleTo(v2) < d);
}

QLength distanceFromGC(const QDirection &direction,
                       const QLength &distFromObserver,
                       const Vector3QLength &vecGCObs) {
	Vector3QLength vecObsTarget;
	vecObsTarget.setRThetaPhi(distFromObserver, direction[0], direction[1]);
	Vector3QLength vecGCTarget = vecObsTarget - vecGCObs;

	return vecGCTarget.getR();
}

QLength distanceToGalBorder(const Vector3QLength &observerPosition,
                            const QDirection &direction,
                            const QLength &galacticBorder,
                            const QLength &zBorder) {
	static const Vector3QLength gcPosition(0, 0, 0);
	// static const QLength galacticBorder = 30_kpc; // for example JF12 is
	// zero for r > 20kpc static const QLength zBorder = 5_kpc;
	Vector3QLength vecObsToGalBorder;
	vecObsToGalBorder.setRThetaPhi(1_m, direction[0], direction[1]);

	QLength a = (gcPosition - observerPosition).getR();
	QLength c = galacticBorder;
	QAngle gamma = vecObsToGalBorder.getAngleTo(observerPosition - gcPosition);

	QLength sphericalBorder =
	    a * cos(gamma) + sqrt(c * c - a * a * (1 - cos(2 * gamma)) / 2.0);
	QLength heightBroder = fabs(zBorder / cos(direction[0]));

	return std::min(heightBroder, sphericalBorder);
}

Vector3QLength getGalacticPosition(const Vector3QLength &observerPosition,
                                   const QLength &dist, const QDirection &dir) {
	Vector3QLength pos(0);

	// TODO(adundovi): should be more general for any observer position
	pos.setRThetaPhi(dist, dir[0], dir[1]);
	pos.x = observerPosition.x - pos.x;
	pos.y = -pos.y;

	return pos;
}

QDirection toGalCoord(const QDirection &d) {
	return QDirection({pi * 0.5_rad - fmod(d[0], pi), fmod(d[1], 2_pi)});
}

QDirection fromGalCoord(const QDirection &d) {
	return QDirection({fmod(pi * 0.5_rad - d[0], pi), fmod(d[1], 2_pi)});
}

QNumber getLorentzFactor(const QMass &m, const QEnergy &E) {
	return E / (m * c_squared);
}

QTemperature intensityToTemperature(const QIntensity &intensity_,
                                    const QFrequency &freq_) {
	return intensity_ * c_squared / (2 * freq_ * freq_ * k_boltzmann);
}

unsigned int getThreadsNumber() {
	// From std::thread docs: the value should be considered only a hint
	unsigned int max_threads = std::thread::hardware_concurrency();

	const char *env_num_threads = getenv("HERMES_NUM_THREADS");
	if (env_num_threads) {
		unsigned int i = std::atoi(env_num_threads);
		if (i < 1)  // just in case, since atoi throws no exceptions
			return max_threads;
		if (i < max_threads) return i;
	}

	return max_threads;
}

std::size_t getThreadId() {
	return std::hash<std::thread::id>()(std::this_thread::get_id());
}

std::vector<std::vector<std::size_t>> getIndexedThreadChunks(
    std::vector<std::size_t> validPixels) {
    
    std::size_t threads = getThreadsNumber();
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

std::vector<std::pair<unsigned int, unsigned int>> getThreadChunks(
    unsigned int queueSize) {
	unsigned int tasks_per_thread = queueSize / getThreadsNumber();
	unsigned int reminder_tasks = queueSize % getThreadsNumber();

	// Init chunks of pixels:  chunk[i] = [ i, (i+1)*pixel_per_thread >
	std::vector<std::pair<unsigned int, unsigned int>> chunks;
	for (unsigned int i = 0; i < getThreadsNumber(); ++i) {
		chunks.push_back(
		    std::make_pair(i * tasks_per_thread, (i + 1) * tasks_per_thread));
	}
	chunks[getThreadsNumber() - 1].second += reminder_tasks;

	return chunks;
}

}  // namespace hermes
