#ifndef HERMES_SUN08CRDENSITY_H
#define HERMES_SUN08CRDENSITY_H

#include "hermes/cosmicRayDensity/CosmicRayDensity.h"

namespace hermes {

class Sun08CRDensity: public CosmicRayDensity {
private:
	QEnergy minE, maxE;
	int spectralIndex;
	QEnergy E_0, E_cutoff;
	QPDensity C_0;
	QLength h_r, h_d, r_Earth;
	int steps;
	void makeEnergyRange();
	void setParameters();
public:
	Sun08CRDensity();
	Sun08CRDensity(QEnergy minE_, QEnergy maxE_, int steps_);
	QPDensityPerEnergy getDensityPerEnergy(const QEnergy& E_, const Vector3QLength& pos_) const;
};

} // namespace hermes

#endif // HERMES_SUN08CRDENSITY_H
