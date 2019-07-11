#ifndef HERMES_CMB_H
#define HERMES_CMB_H

#include "hermes/photonField/PhotonField.h"

namespace hermes {

class CMB : public PhotonField {
private:
    std::vector<QEnergyDensity> density;

    void buildEnergyRange() {
    	const double scaling = getEnergyScaleFactor();
	const QEnergy E_start = getStartEnergy();
	const QEnergy E_end = getEndEnergy();
	
	for(QEnergy E = E_start; E < E_end; E = E * scaling)  
    		energyRange.push_back(E);
    }

    void precomputeValues() {
	for (auto itE = begin(); itE != end(); ++itE)
		density.push_back(getEnergyDensity(Vector3QLength(0), (*itE)));
    }

public:
    CMB() {
	setEnergyScaleFactor(1.1);
    	setStartEnergy(1e10_Hz*h_planck);
    	setEndEnergy(1e12_Hz*h_planck);
	buildEnergyRange();
    	precomputeValues();
    }

    QEnergyDensity getEnergyDensity(
		    const Vector3QLength &pos,
		    const QEnergy &E_photon) const {

        QFrequency nu = E_photon / h_planck;
	QTemperature T_CMB = 2.725_K;
    	return (8_pi*h_planck * pow<4>(nu)) /
		pow<3>(c_light) /
		(exp(h_planck*nu / (k_boltzmann * T_CMB)) - 1.);
    }

    QEnergyDensity getEnergyDensity(
		    const Vector3QLength& pos_, int iE_) const {
   	return density[iE_];
    }

};

} // namespace hermes

#endif // HERMES_CMB_H
