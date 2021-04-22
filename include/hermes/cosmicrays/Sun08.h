#ifndef HERMES_SUN08CRDENSITY_H
#define HERMES_SUN08CRDENSITY_H

#include "hermes/cosmicrays/CosmicRayDensity.h"

namespace hermes { namespace cosmicrays {
/**
 * \addtogroup CosmicRays
 * @{
 */

class Sun08 : public CosmicRayDensity {
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
	Sun08();
	Sun08(QEnergy minE_, QEnergy maxE_, int steps_);
	QPDensityPerEnergy getDensityPerEnergy(
	    const QEnergy &E_, const Vector3QLength &pos_) const override;
};

/** @}*/
}}  // namespace hermes::cosmicrays

#endif  // HERMES_SUN08CRDENSITY_H
