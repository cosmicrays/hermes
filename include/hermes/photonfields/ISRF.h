#ifndef HERMES_ISRF_H
#define HERMES_ISRF_H

#include <string>
#include <vector>

#include "hermes/photonfields/PhotonField.h"

namespace hermes { namespace photonfields {
/**
 * \addtogroup PhotonFields
 * @{
 */

/** Vernetto16 interstellar radiation field backed by a FITS density cube. */
class ISRF : public PhotonField {
  private:
	const static int freqR1 = 200;
	const static int freqR2 = 680;
	const static int freqR3 = 331;
	std::vector<double> logwavelenghts;
	std::vector<double> r_id;
	std::vector<double> z_id;
	std::vector<double> isrf;

	void buildEnergyRange();
	void initializeEnergyRange();

	double getISRF(std::size_t ir, std::size_t iz, std::size_t ifreq) const;

	void loadLegacyFrequencyAxis();
	void loadISRF();
	void loadCombinedISRF(const std::string &filename);
	void loadLegacyISRF();

  public:
	/** Load the standard data cube, with fallback to the legacy text tables. */
	ISRF();
	/** Load a combined Vernetto16-format FITS cube from an explicit path. */
	explicit ISRF(const std::string &filename);
	std::size_t getSize() const;
	QEnergyDensity getEnergyDensity(const QLength &r, const QLength &z,
	                                const QEnergy &E_photon) const;
	QEnergyDensity getEnergyDensity(const Vector3QLength &pos,
	                                const QEnergy &E_photon) const override;
	QEnergyDensity getEnergyDensity(const Vector3QLength &pos_,
	                                std::size_t iE_) const override;
};

/** @}*/
}}  // namespace hermes::photonfields

#endif  // HERMES_ISRF_H
