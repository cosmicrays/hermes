#ifndef HERMES_DARKMATTERINTEGRATOR_H
#define HERMES_DARKMATTERINTEGRATOR_H

#include <array>
#include <memory>

#include "hermes/Units.h"
#include "hermes/darkmatter/DarkMatterSpectrum.h"
#include "hermes/darkmatter/GalacticProfile.h"
#include "hermes/integrators/IntegratorTemplate.h"

namespace hermes {
/**
 * \addtogroup Integrators
 * @{
 */

class DarkMatterIntegrator : public GammaIntegratorTemplate {
  private:
	std::shared_ptr<darkmatter::DarkMatterSpectrum> spectrum;
	std::shared_ptr<darkmatter::GalacticProfile> profile;

  public:
	DarkMatterIntegrator(
	    const std::shared_ptr<darkmatter::DarkMatterSpectrum> &spectrum,
	    const std::shared_ptr<darkmatter::GalacticProfile> &profile);
	~DarkMatterIntegrator();

	QDiffIntensity integrateOverLOS(const QDirection &direction) const override;
	QDiffIntensity integrateOverLOS(const QDirection &iterdir,
	                                const QEnergy &Egamma) const override;
	QGREmissivity spectralEmissivity(const Vector3QLength &pos,
	                                 QEnergy Egamma) const;
};

/** @}*/
}  // namespace hermes

#endif  // HERMES_DARKMATTERINTEGRATOR_H
