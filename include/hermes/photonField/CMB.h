#ifndef HERMES_CMB_H
#define HERMES_CMB_H

#include "hermes/photonField/PhotonField.h"

namespace hermes {

class CMB : public PhotonField {
public:
    CMB() {
	setEnergyScaleFactor(1.1);
    	setStartEnergy(1e10_Hz*h_planck);
    	setEndEnergy(1e12_Hz*h_planck);
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
};

} // namespace hermes

#endif // HERMES_CMB_H
