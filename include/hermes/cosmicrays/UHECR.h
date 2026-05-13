#ifndef HERMES_UHECR_H
#define HERMES_UHECR_H

#include "hermes/cosmicrays/CosmicRayDensity.h"

namespace hermes { namespace cosmicrays {
/**
 * \addtogroup CosmicRays
 * @{
 */

class UHECR : public CosmicRayDensity {
  private:
	QEnergy minE, maxE;
	int steps;
	double alpha_1;
	double alpha_2;
	double alpha_3;
	double alpha_4;
	double omega_12;
	double omega_23;
	double omega_34;
	QEnergy E_0;
	QEnergy E_min;
	QEnergy E_12;
	QEnergy E_23;
	QEnergy E_34;
	QPDensityPerEnergy n_0;
	void makeEnergyRange();
	void setParameters();

  public:
	UHECR(const PID &pid = Proton);
	UHECR(const PID &pid, QEnergy minE, QEnergy maxE, int steps);
	QPDensityPerEnergy getDensityPerEnergy(const QEnergy &E_, const Vector3QLength &pos_) const override;
};

/** @}*/
}}  // namespace hermes::cosmicrays

#endif  // HERMES_UHECR_H
