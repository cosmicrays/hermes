#ifndef HERMES_DARKMATTERINTEGRATOR_H
#define HERMES_DARKMATTERINTEGRATOR_H

#include <array>
#include <memory>
#include <utility>
#include <vector>

#include "hermes/Units.h"
#include "hermes/darkmatter/DarkMatterSpectrum.h"
#include "hermes/darkmatter/GalacticProfile.h"
#include "hermes/integrators/IntegratorTemplate.h"
#include "hermes/interactions/BreitWheeler.h"

namespace hermes {
/**
 * \addtogroup Integrators
 * @{
 */

using QAnnihilationCrossSection = decltype(units::centimetre3 / units::second);
using QDecayTime = QTime;

enum class DarkMatterProcess {
	annihilation,
	decay,
};

class DarkMatterIntegrator : public GammaIntegratorTemplate {
  private:
	std::shared_ptr<darkmatter::DarkMatterSpectrum> spectrum;
	std::shared_ptr<darkmatter::GalacticProfile> profile;
	DarkMatterProcess process;
	QAnnihilationCrossSection sigmaV;
	double symmetryFactor;
	QDecayTime decayTime;
	bool absorptionEnabled;
	std::shared_ptr<interactions::BreitWheeler> bwCrossSec;
	std::vector<std::pair<QEnergy, QInverseLength>> absorptionCoefficientTable;

  public:
	DarkMatterIntegrator(const std::shared_ptr<darkmatter::DarkMatterSpectrum> &spectrum,
	                     const std::shared_ptr<darkmatter::GalacticProfile> &profile, bool includeAbsorption = false);
	DarkMatterIntegrator(const std::shared_ptr<darkmatter::DarkMatterSpectrum> &spectrum,
	                     const std::shared_ptr<darkmatter::GalacticProfile> &profile,
	                     const QAnnihilationCrossSection &sigmaV, double symmetryFactor = 0.5,
	                     bool includeAbsorption = false);
	DarkMatterIntegrator(const std::shared_ptr<darkmatter::DarkMatterSpectrum> &spectrum,
	                     const std::shared_ptr<darkmatter::GalacticProfile> &profile, const QDecayTime &decayTime,
	                     bool includeAbsorption = false);
	DarkMatterIntegrator(const std::shared_ptr<darkmatter::DarkMatterSpectrum> &spectrum,
	                     const std::shared_ptr<darkmatter::GalacticProfile> &profile, DarkMatterProcess process,
	                     const QAnnihilationCrossSection &sigmaV = 3e-26_cm3 / 1_s, double symmetryFactor = 0.5,
	                     const QDecayTime &decayTime = 1e28_s, bool includeAbsorption = false);
	~DarkMatterIntegrator();

	QDiffIntensity integrateOverLOS(const QDirection &direction) const override;
	QDiffIntensity integrateOverLOS(const QDirection &iterdir, const QEnergy &Egamma) const override;
	QGREmissivity spectralEmissivity(const Vector3QLength &pos, QEnergy Egamma) const;
	QInverseLength absorptionCoefficient(const QEnergy &Egamma) const;
	QInverseLength getAbsorptionCoefficient(const QEnergy &Egamma) const;
	void precomputeAbsorptionCoefficients(const std::vector<QEnergy> &energies);
	void clearAbsorptionCoefficientTable();
	std::size_t getAbsorptionCoefficientTableSize() const;
	DarkMatterProcess getProcess() const;
	QAnnihilationCrossSection getSigmaV() const;
	double getSymmetryFactor() const;
	QDecayTime getDecayTime() const;
	void setAbsorptionEnabled(bool enabled);
	bool isAbsorptionEnabled() const;
};

/** @}*/
}  // namespace hermes

#endif  // HERMES_DARKMATTERINTEGRATOR_H
