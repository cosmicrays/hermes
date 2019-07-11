#ifndef HERMES_PHOTONFIELD_H
#define HERMES_PHOTONFIELD_H

#include "hermes/Units.h"
#include "hermes/Grid.h"

namespace hermes {

class PhotonField {
protected:
	typedef std::vector<QEnergy> tEnergyRange;
	mutable tEnergyRange energyRange;

	bool scaleFactorFlag;
	double energyScaleFactor;
	QEnergy startEnergy, endEnergy;
public:
        typedef tEnergyRange::iterator iterator;
        typedef tEnergyRange::const_iterator const_iterator;
	PhotonField() { };

	virtual QEnergyDensity getEnergyDensity(
		    const Vector3QLength &pos, const QEnergy &E_photon) const = 0;
    
    	virtual QEnergyDensity getEnergyDensity(
		    const Vector3QLength &pos_, int iE_) const = 0;
	
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
        
	iterator begin() {
		return energyRange.begin();	
	}
        const_iterator begin() const {
		return energyRange.begin();
	}
        iterator end() {
		return energyRange.end();
	}
        const_iterator end() const {
		return energyRange.end();
	}
};

} // namespace hermes

#endif // HERMES_PHOTONFIELD_H
