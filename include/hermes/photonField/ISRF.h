#ifndef HERMES_ISRF_H
#define HERMES_ISRF_H

#include "hermes/photonField/PhotonField.h"

#include <array>

namespace hermes {

class ISRF : public PhotonField {
private:
	const static int freqR1 = 200;
	const static int freqR2 = 680;
	const static int freqR3 = 331;
    	std::array<double, freqR1 + freqR2 + freqR3> logwavelenghts;
	std::array<double, 30> r_id = {0.0, 0.2, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5,
	    4.0, 4.5, 5.0, 5.5, 6.0, 6.5, 7.0, 7.5, 8.0,  8.5,  9.0,  9.5, 10.0,
	    11.0, 12.0, 14.0, 16.0, 18.0, 20.0, 25.0, 30.0}; // in kpc (30)
	std::array<double, 24> z_id = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.8, 1.0,
	    1.2, 1.5, 2.0, 2.5, 3.0, 4.0, 5.0, 6.0, 8.0, 10.0, 12.0, 15.0,
	    20.0, 25.0, 30.0}; // in kpc (24)
	std::vector<double> isrf;

	double getISRF(const int& ir, const int& iz, const int& ifreq) const;

	void loadFrequencyAxis();
	void loadISRF();

public:
    ISRF();
    int getSize() const;
    QEnergyDensity getEnergyDensity(
		    const QLength &r, const QLength &z, const QEnergy &E_photon) const;
    QEnergyDensity getEnergyDensity(
		    const Vector3QLength &pos, const QEnergy &E_photon) const;
    QEnergyDensity getEnergyDensity(
		    const Vector3QLength &pos_, int iE_) const;
    
};

} // namespace hermes

#endif // HERMES_ISRF_H
