#ifndef HERMES_KAMAE06_H
#define HERMES_KAMAE06_H

/*
https://github.com/niklask/cparamlib

cparamlib is a simple library for calculation of fluxes for stable secondary particles
created in proton-proton interactions in astronomical environment according to the model
described in 
  Tuneyoshi Kamae, Niklas Karlsson, Tsunefumi Mizuno, Toshinori Abe, and Tatsumi Koi; 
  "Parameterization of $\gamma$, $e^{\pm}$ and Neutrino Spectra Produced by $p-p$ Interaction in Astronomical Environment"
to appear in Astrophysical Journal (2006) and
  Niklas Karlsson and Tuneyoshi Kamae;
  "Parameterization of the Angular Distribution of Gamma Rays Produced by P-P Interaction in Astronomical Environment"
to by submitted to Astrophysical Journal (2007).
*/

#include "hermes/interactions/DifferentialCrossSection.h"
#include "hermes/CacheTools.h"

#include <memory>

extern "C" {
	#include "cparamlib.h"
}

namespace hermes {

class Kamae06: public DifferentialCrossSection {
private:
	bool cachingEnabled;
        std::unique_ptr<CacheStorageCrossSection> cache;
public:
	Kamae06();
	void setCachingStorage(
		std::unique_ptr<CacheStorageCrossSection> cache);
	void enableCaching();
	void disableCaching();
	
	QDifferentialCrossSection getDiffCrossSection(
			const QEnergy &E_proton, const QEnergy &E_gamma) const override;
	QDifferentialCrossSection getDiffCrossSectionDirectly(
			const QEnergy &E_proton, const QEnergy &E_gamma) const;
};

} // namespace hermes

#endif // HERMES_KAMAE06_H
