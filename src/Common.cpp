#include "hermes/Common.h"

#include "kiss/path.h"
#include "kiss/logger.h"

#include <cstdlib>

namespace hermes {

std::string getDataPath(std::string filename) {
        // adopted from CRPropa3
        static std::string dataPath;
        if (dataPath.size())
                return concat_path(dataPath, filename);

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
                        KISS_LOG_INFO
                        << "getDataPath: use install prefix, " << dataPath << std::endl;
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

bool isWithinAngle(QDirection a, QDirection b, QAngle d) {
	Vector3d v1, v2;
	v1.setRThetaPhi(1, a[0], a[1]);
	v2.setRThetaPhi(1, b[0], b[1]);
	return (v1.getAngleTo(v2) < d);
}

QLength distanceFromGC(QDirection direction, QLength distanceFromSun,
                                Vector3QLength vecGCSun) {
        Vector3QLength vecSunTarget;
        vecSunTarget.setRThetaPhi(distanceFromSun, direction[0], direction[1]);
        Vector3QLength vecGCTarget = vecSunTarget - vecGCSun;

        return vecGCTarget.getR();
}

QLength distanceToGalBorder(Vector3QLength positionSun, QDirection direction) {

        Vector3QLength positionGC(0, 0, 0);
        QLength galacticBorder = 20_kpc; // for example JF12 is zero for r > 20kpc
        Vector3QLength vecSunToGalBorder;
        vecSunToGalBorder.setRThetaPhi(1_m, direction[0], direction[1]);

        QLength a = (positionGC - positionSun).getR();
        QLength c = galacticBorder;
        QAngle gamma = vecSunToGalBorder.getAngleTo(positionSun-positionGC);

        return a*cos(gamma) + sqrt(c*c - a*a*(1 - cos(2*gamma))/2.0);
}

QNumber getLorentzFactor(QMass m, QEnergy E) {
        return E / (m * c_squared);
}


QTemperature intensityToTemperature(QIntensity intensity_, QFrequency freq_) {
	return intensity_*c_squared / (2*freq_*freq_*k_boltzmann);
}


} // namespace hermes 
