#ifndef HERMES_KLEINNISHINA_H
#define HERMES_KLEINNISHINA_H

#include "hermes/interactions/DifferentialCrossSection.h"
#include "hermes/CacheTools.h"

#include <memory>

namespace hermes {

class KleinNishina: public DifferentialCrossSection {
private:
	bool cacheStoragePresent;
        std::unique_ptr<CacheStorageKN> cache;
	
public:
	KleinNishina();
	void setCacheStorage(
		std::unique_ptr<CacheStorageKN> cache);
	// not used
	QDifferentialCrossSection getDiffCrossSection(
			const QEnergy &E_photon,
			const QEnergy &E_gamma) const;

	QDifferentialCrossSection getDiffCrossSection(
			const QEnergy &E_electron,
			const QEnergy &E_photon,
			const QEnergy &E_gamma) const;

	QDifferentialCrossSection getDiffCrossSectionFromCache(
		const QEnergy &E_electron,
		const QEnergy &E_photon,
		const QEnergy &E_gamma) const;
};

} // namespace hermes

#endif // HERMES_KLEINNISHINA_H
