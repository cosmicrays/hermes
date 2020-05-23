#ifndef HERMES_BREMSSTRAHLUNGGALPROP_H
#define HERMES_BREMSSTRAHLUNGGALPROP_H

#include <array>
#include <memory>

#include "hermes/CacheTools.h"
#include "hermes/interactions/BremsstrahlungAbstract.h"

namespace hermes { namespace interactions {

class BremsstrahlungGALPROP : public BremsstrahlungAbstract {
  private:
	bool cachingEnabled;
	std::array<std::unique_ptr<CacheStorageCrossSection>, Ntargets> cache;

	QNumber ElwertFactor(const QNumber &beta_i, const QNumber &beta_f,
	                     int Z) const;
	QNumber xiFunc(const QNumber &T_electron_i, const QNumber &k, int Z,
	               int N) const;
	QNumber Phi_u(const QNumber &gamma_i, const QNumber &gamma_f,
	              const QNumber &k) const;

	inline double R_1(double q, double Z) const;
	inline double R_2(double q, double Z) const;
	QNumber I_Phi_1(const QNumber &delta, int Z, int N) const;
	QNumber I_Phi_2(const QNumber &delta, int Z, int N) const;
	QNumber Phi_1(const QNumber &gamma_i, const QNumber &gamma_f,
	              const QNumber &k, const QNumber &delta, int Z, int N) const;
	QNumber Phi_2(const QNumber &gamma_i, const QNumber &gamma_f,
	              const QNumber &k, const QNumber &delta, int Z, int N) const;

	QArea dsdk_LowEnergy(const QNumber &p_i, const QNumber &p_f, QNumber k,
	                     int Z) const;
	QArea dsdk_IntermediateEnergy(const QNumber &gamma_i,
	                              const QNumber &gamma_f, const QNumber &p_i,
	                              const QNumber &p_f, const QNumber &k,
	                              int Z) const;
	QArea dsdk_HighEnergy(const QNumber &gamma_i, const QNumber &gamma_f,
	                      const QNumber &k, int Z, int N) const;

	QDiffCrossSection getDiffCrossSectionForTargetDirectly(
	    Target t, const QEnergy &T_electron, const QEnergy &E_gamma) const;

  public:
	BremsstrahlungGALPROP();

	void enableCaching();
	void disableCaching();

	QDiffCrossSection getDiffCrossSectionForTarget(
	    Target t, const QEnergy &T_electron,
	    const QEnergy &E_gamma) const override;
};

}}  // namespace hermes::interactions

#endif  // HERMES_BREMSSTRAHLUNGGALPROP_H
