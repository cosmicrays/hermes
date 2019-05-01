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
        
	void readDataFiles();
public:
	RingModelDensity();
	~RingModelDensity() { }
	//virtual QPDensity getDensity(const Vector3QLength& pos) const = 0;

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
