#ifndef HERMES_DARKMATTER_NFW1996PROFILE_H
#define HERMES_DARKMATTER_NFW1996PROFILE_H

#include <memory>

#include "hermes/darkmatter/GalacticProfile.h"

namespace hermes { namespace darkmatter {
/**
 * \addtogroup DarkMatter
 * @{
 */

class NFW1996Profile : public GalacticProfile {
  protected:
	const QMassDensity rho_cr = 9.1e-30 * 1_g / 1_cm3;  // WMAP-9

	// input
	double gamma;
	double concentration;
	QMass M_200;

	// derived
	QLength R_200;
	QMassDensity rho_s;
	QLength r_s;

  protected:
	void init();

  public:
	NFW1996Profile(double gamma, double c, QMass M_200);
	~NFW1996Profile() = default;
	QMassDensity getMassDensity(QLength r) const override;
};

/** @}*/
}}  // namespace hermes::darkmatter

#endif  // HERMES_DARKMATTER_NFW1996PROFILE_H
