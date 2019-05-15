#ifdef HERMES_HAVE_CFITSIO

#ifndef HERMES_RINGMODELDENSITY_H
#define HERMES_RINGMODELDENSITY_H

#include "hermes/Units.h"
#include "hermes/Grid.h"
#include "hermes/FITSWrapper.h"

namespace hermes {

class RingModelDensity {
private:
        std::unique_ptr<FITSFile> ffile;
	QTemperature gasTemp;
	
	int n_lon, n_lat, n_rings;
	double min_lon, min_lat, delta_lon, delta_lat;
	std::vector<float> dataVector;

	void readDataFiles();
public:
	RingModelDensity();
	//~RingModelDensity() { }
	QPDensity getDensityInRing(int ring, const QDirection& dir) const;

	inline void setTemperature(QTemperature T) {
		gasTemp = T;
	}
	inline QTemperature getTemperature() const {
		return gasTemp;
	}
};

} // namespace hermes

#endif // HERMES_RINGMODELDENSITY_H
#endif // HERMES_HAVE_CFITSIO
