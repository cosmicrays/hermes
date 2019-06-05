#ifndef HERMES_PHOTONFIELD_H
#define HERMES_PHOTONFIELD_H

#include "hermes/Units.h"
#include "hermes/Grid.h"

namespace hermes {

class PhotonField {
private:
	QEnergy startEnergy, endEnergy;
	double energyScaleFactor = 1.1;
public:
	PhotonField() { };

	virtual QEnergyDensity getEnergyDensity(
		    const Vector3QLength &pos, const QEnergy &E_photon) const = 0;
	
	void setStartEnergy(QEnergy E_) {
		startEnergy = E_;
	}
	
	void setEndEnergy(QEnergy E_) {
		endEnergy = E_;
	}
	
	void setEnergyScaleFactor(double s_) {
		energyScaleFactor = s_;
	}

	QEnergy getStartEnergy() const {
		return startEnergy;
	}
	
	QEnergy getEndEnergy() const {
		return endEnergy;
	}
	
	double getEnergyScaleFactor() const {
		return energyScaleFactor;
	}
};

} // namespace hermes

#endif // HERMES_PHOTONFIELD_H
