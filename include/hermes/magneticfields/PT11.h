#ifndef HERMES_PSHIRKOVFIELD_H
#define HERMES_PSHIRKOVFIELD_H

#include "hermes/magneticfields/MagneticField.h"

namespace hermes { namespace magneticfields {
/**
 * \addtogroup MagneticFields
 * @{
 */

/**
 @class PT11
 @brief Pshirkov2011 galactic magnetic field model

 Implements the Pshirkov2011 galactic magnetic field model, consisting of
 large-scale regular disk and halo fields. For the disk field an axisymmetric
 (ASS) and the bisymmetric (BSS, default) model can be chosen. For the halo
 field the BHM Halo field model (Sun et al. 2008) is considered.

 Currently only best fit values of the field parameters are implemented.
 The field is defined in the usual galactocentric coordinate system with the
 Galactic center at the origin, the x-axis pointing in the opposite direction of
 the Sun, and the z-axis pointing towards Galactic north.

 See: Pshirkov, Tinyakov, Kronberg Newton-McGee 2011 - Deriving global structure
 of the Galactic Magnetic Field from Faraday Rotation Measures of extragalactic
 sources, DOI: 10.1088/0004-637X/738/2/192, arXiv:1103.0814
 */

class PT11 : public MagneticField {
  private:
	bool useASS;   // switch for axisymmetric spiral field (ASS)
	bool useBSS;   // switch for bisymmetric spiral field (BSS)
	bool useHalo;  // switch for halo field

	// disk parameters
	QAngle pitch, PHI;
	QNumber cos_pitch, sin_pitch, cos_PHI;  // pitch angle parameters
	QLength d;                              // distance to first field reversal
	QLength R_sun;  // distance between sun and galactic center
	QLength R_c;    // radius of central region
	QLength z0_D;   // vertical thickness in the galactic disk
	QMField B0_D;   // magnetic field scale

	// halo parameters
	QLength z0_H;   // halo vertical position
	QLength R0_H;   // halo radial position
	QMField B0_Hn;  // halo magnetic field scale (north)
	QMField B0_Hs;  // halo magnetic field scale (south)
	QLength z11_H;  // halo vertical thickness towards disc
	QLength z12_H;  // halo vertical thickness off the disk

	void SetParams();

  public:
	PT11();

	void setUseASS(bool use);
	void setUseBSS(bool use);
	void setUseHalo(bool use);

	bool isUsingASS();
	bool isUsingBSS();
	bool isUsingHalo();

	Vector3QMField getField(const Vector3QLength &pos) const override;
};

/** @} */
}}  // namespace hermes::magneticfields

#endif  // HERMES_PSHIRKOVFIELD_H
