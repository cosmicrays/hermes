#ifndef HERMES_BREMSSTRAHLUNGSIMPLE_H
#define HERMES_BREMSSTRAHLUNGSIMPLE_H

#include <memory>

#include "hermes/ParticleID.h"
#include "hermes/CacheTools.h"
#include "hermes/interactions/DiffCrossSection.h"

#define TARGETS_NUMBER

namespace hermes { namespace interactions {


class BremsstrahlungSimple : public DifferentialCrossSection {
  public:
	constexpr static std::size_t Ntargets = 3;
	enum class Target {HII, HI, He};
	const std::array<Target,Ntargets> allTargets = { // to iterate over targets
		Target::HII, Target::HI, Target::He
	};
  private:
	bool cachingEnabled;
	std::array<std::unique_ptr<CacheStorageCrossSection>, Ntargets> cache;

  public:
	BremsstrahlungSimple();
	
	void enableCaching();
	void disableCaching();
	
	QDiffCrossSection getDiffCrossSection(const QEnergy &T_electron,
			const QEnergy &E_gamma) const override;
	QDiffCrossSection getDiffCrossSectionForTarget(Target t,
			const QEnergy &T_electron, const QEnergy &E_gamma) const;
	QDiffCrossSection getDiffCrossSectionForTargetDirectly(Target t,
			const QEnergy &T_electron, const QEnergy &E_gamma) const;

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
};

}}  // namespace hermes::interactions

#endif  // HERMES_BREMSSTRAHLUNGSIMPLE_H
