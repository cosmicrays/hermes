#ifndef HERMES_WMAP07CRDENSITY_H
#define HERMES_WMAP07CRDENSITY_H

#include "hermes/cosmicrays/CosmicRayDensity.h"

namespace hermes {
namespace cosmicrays {

class WMAP07CRDensity : public CosmicRayDensity {
      private:
	QEnergy minE, maxE;
	int spectralIndex;
	QEnergy E_0, E_cutoff;
	QPDensity C_0, C_Earth;
	QLength h_r, h_d, r_Earth;
	int steps;
	void makeEnergyRange();
	void setParameters();

      public:
	WMAP07CRDensity();
	WMAP07CRDensity(QEnergy minE_, QEnergy maxE_, int steps_);
	QPDensityPerEnergy
	getDensityPerEnergy(const QEnergy &E_,
			    const Vector3QLength &pos_) const override;
};

} // namespace cosmicrays
} // namespace hermes

#endif // HERMES_WMAP07CRDENSITY_H
