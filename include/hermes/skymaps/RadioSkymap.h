#ifndef HERMES_RADIOSKYMAP_H
#define HERMES_RADIOSKYMAP_H

#include "hermes/skymaps/SkymapTemplate.h"

namespace hermes {

/**
 * \addtogroup Skymaps
 * @{
 */

/**
 @class RadioSkymap
 @brief A skymap container suitable for radio emissions; saves pixels in units
 of temperature (K), specified by frequency (Hz).
 */
class RadioSkymap : public SkymapTemplate<QTemperature, QFrequency> {
  public:
	RadioSkymap(std::size_t nside_, QFrequency freq_)
	    : SkymapTemplate(nside_, freq_){};

	void setFrequency(QFrequency freq_) { setSkymapParameter(freq_); }
	QFrequency getFrequency() const { return skymapParameter; }
	void computePixel(
	    std::size_t ipix,
	    std::shared_ptr<IntegratorTemplate<QTemperature, QFrequency>>
	        integrator_) {
		fluxContainer[ipix] = integrator_->integrateOverLOS(
		    pix2ang_ring(getNside(), ipix), skymapParameter);
	}
};

/** @}*/
}  // namespace hermes
#endif  // HERMES_RADIOSKYMAP_H
