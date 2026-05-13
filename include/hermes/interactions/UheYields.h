#ifndef HERMES_UHEYIELDS_H
#define HERMES_UHEYIELDS_H

#include "hermes/interactions/DiffCrossSection.h"

namespace hermes { namespace interactions {
/**
 * \addtogroup Interactions
 * @{
 */

enum class UheHadronicModel {
	Sibyll23e,
	QGSJetII04,
	EposLHC,
};

class UheYields : public DifferentialCrossSection {
  protected:
	UheHadronicModel model;
	virtual double getXDNdX(double x) const = 0;

  public:
	UheYields(UheHadronicModel model = UheHadronicModel::Sibyll23e);

	QDiffCrossSection getDiffCrossSection(const QEnergy &E_proton, const QEnergy &E_secondary) const override;
	QDiffCrossSection getDiffCrossSection(const PID &projectile, const PID &target, const QEnergy &E_proj,
	                                      const QEnergy &E_secondary) const override;

	static QArea sigmaInelastic(const QEnergy &E_proton);
};

/** @}*/
}}  // namespace hermes::interactions

#endif  // HERMES_UHEYIELDS_H
