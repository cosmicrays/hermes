#ifndef HERMES_DUMMYCROSSSECTION_H
#define HERMES_DUMMYCROSSSECTION_H

#include "hermes/CacheTools.h"
#include "hermes/interactions/DifferentialCrossSection.h"

#include <memory>

namespace hermes {
namespace interactions {

class DummyCrossSection : public DifferentialCrossSection {
      private:
	QDifferentialCrossSection cs;

      public:
	DummyCrossSection(const QDifferentialCrossSection &cs_)
	    : DifferentialCrossSection(), cs(cs_){};

	QDifferentialCrossSection
	getDiffCrossSection(const QEnergy &E_photon,
			    const QEnergy &E_gamma) const {
		return QDifferentialCrossSection(cs);
	}

	QDifferentialCrossSection
	getDiffCrossSection(const QEnergy &E_electron, const QEnergy &E_photon,
			    const QEnergy &E_gamma) const {
		return QDifferentialCrossSection(cs);
	};
};

} // namespace interactions
} // namespace hermes

#endif // HERMES_DUMMYCROSSSECTION_H
