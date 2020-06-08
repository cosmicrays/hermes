#ifndef HERMES_DISPERSIONMEASUREINTEGRATOR_H
#define HERMES_DISPERSIONMEASUREINTEGRATOR_H

#include <memory>

#include "hermes/Units.h"
#include "hermes/chargedgas/ChargedGasDensity.h"
#include "hermes/integrators/IntegratorTemplate.h"
#include "hermes/skymaps/DispersionMeasureSkymap.h"

/** \file DispersionMeasureIntegrator.h
 *  Declares DispersionMeasureIntegrator
 */

namespace hermes {
/**
 * \addtogroup Integrators
 * @{
 */

/**
 * \class DispersionMeasureIntegrator
 * \brief Calculates dispersion measure (DM) over the LOS
 *
 * The integrators depends only on a charged gas density model, such as
 * chargedgas::YMW16
 */
class DispersionMeasureIntegrator : public DispersionMeasureIntegratorTemplate {
  private:
	std::shared_ptr<chargedgas::ChargedGasDensity> gdensity;

  public:
	DispersionMeasureIntegrator(
	    const std::shared_ptr<chargedgas::ChargedGasDensity> &gdensity);
	~DispersionMeasureIntegrator();

	QDispersionMeasure integrateOverLOS(
	    const QDirection &iterdir) const override;
	QDispersionMeasure integrateOverLOS(const QDirection &iterdir,
	                                    const QNumber &num) const override {
		return QDispersionMeasure(0);
	}

	tLOSProfile getLOSProfile(const QDirection &direction,
	                          int Nsteps) const override;
};

/** @}*/
}  // namespace hermes

#endif  // HERMES_DISPERSIONMEASUREINTEGRATOR_H
