#ifndef HERMES_DARKMATTERNEUTRONINTEGRATOR_H
#define HERMES_DARKMATTERNEUTRONINTEGRATOR_H

#include <memory>

#include "hermes/integrators/DarkMatterIntegrator.h"

namespace hermes {
/**
 * \addtogroup Integrators
 * @{
 */

class DarkMatterNeutronIntegrator : public GammaIntegratorTemplate {
  private:
	std::shared_ptr<darkmatter::DarkMatterSpectrum> spectrum;
	std::shared_ptr<darkmatter::GalacticProfile> profile;
	DarkMatterProcess process;
	QAnnihilationCrossSection sigmaV;
	double symmetryFactor;
	QDecayTime decayTime;
	bool includeDecay;

  public:
	DarkMatterNeutronIntegrator(const std::shared_ptr<darkmatter::DarkMatterSpectrum> &spectrum,
	                            const std::shared_ptr<darkmatter::GalacticProfile> &profile);
	DarkMatterNeutronIntegrator(const std::shared_ptr<darkmatter::DarkMatterSpectrum> &spectrum,
	                            const std::shared_ptr<darkmatter::GalacticProfile> &profile,
	                            const QAnnihilationCrossSection &sigmaV, double symmetryFactor = 0.5);
	DarkMatterNeutronIntegrator(const std::shared_ptr<darkmatter::DarkMatterSpectrum> &spectrum,
	                            const std::shared_ptr<darkmatter::GalacticProfile> &profile,
	                            const QDecayTime &decayTime);
	DarkMatterNeutronIntegrator(const std::shared_ptr<darkmatter::DarkMatterSpectrum> &spectrum,
	                            const std::shared_ptr<darkmatter::GalacticProfile> &profile, DarkMatterProcess process,
	                            const QAnnihilationCrossSection &sigmaV = 3e-26_cm3 / 1_s, double symmetryFactor = 0.5,
	                            const QDecayTime &decayTime = 1e28_s);
	~DarkMatterNeutronIntegrator();

	QDiffIntensity integrateOverLOS(const QDirection &direction) const override;
	QDiffIntensity integrateOverLOS(const QDirection &iterdir, const QEnergy &Eneutron) const override;
	QGREmissivity spectralEmissivity(const Vector3QLength &pos, QEnergy Eneutron) const;
	QInverseLength decayInverseLength(const QEnergy &Eneutron) const;
	DarkMatterProcess getProcess() const;
	QAnnihilationCrossSection getSigmaV() const;
	double getSymmetryFactor() const;
	QDecayTime getDecayTime() const;
	void setIncludeDecay(bool includeDecay_);
	bool isDecayEnabled() const;
};

/** @}*/
}  // namespace hermes

#endif  // HERMES_DARKMATTERNEUTRONINTEGRATOR_H
