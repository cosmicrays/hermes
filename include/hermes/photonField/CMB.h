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
	
	for(auto E = E_start; E < E_end; E = E * scaling)  
    		energyRange.push_back(E);
    }

    void precomputeValues() {
	for (auto itE = begin(); itE != end(); ++itE)
		density.push_back(getEnergyDensity(Vector3QLength(0), (*itE)));
    }

public:
    CMB() {
	setEnergyScaleFactor(1.01);
    	setStartEnergy(1.0e9_Hz*h_planck);
    	setEndEnergy(1.0e12_Hz*h_planck);
	buildEnergyRange();
    	precomputeValues();
    }

    QEnergyDensity getEnergyDensity(
		    const Vector3QLength &pos,
		    const QEnergy &E_photon) const {
	const QTemperature T_CMB(2.725);
        QFrequency nu = E_photon / h_planck;
    
    	return getPlanckSpectralEnergyDensity(nu, T_CMB) * nu;
    }

    QSpectralEnergyDensity getPlanckSpectralEnergyDensity(
		    const QFrequency &nu, const QTemperature &T) const {

    	return (8.0_pi * h_planck * pow<3>(nu)) /
		pow<3>(c_light) /
		(exp(h_planck*nu / (k_boltzmann * T)) - 1.);
    }

    QEnergyDensity getEnergyDensity(
		    const Vector3QLength& pos_, int iE_) const {
   	return density[iE_];
    }

};

} // namespace hermes

#endif // HERMES_CMB_H
