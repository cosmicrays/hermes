#ifndef HERMES_PPPC4DMIDSPECTRUM_H
#define HERMES_PPPC4DMIDSPECTRUM_H

#include <memory>
#include <vector>

#include "hermes/Units.h"
#include "hermes/darkmatter/DarkMatterSpectrum.h"
#include "hermes/darkmatter/PPPC4DMIDChannels.h"

/*
 *  M.Cirelli, G.Corcella, A.Hektor, G.Hütsi, M.Kadastik, P.Panci, M.Raidal,
 * F.Sala, A.Strumia, "PPPC 4 DM ID: A Poor Particle Physicist Cookbook for Dark
 * Matter Indirect Detection", arXiv 1012.4515, JCAP 1103 (2011) 051. Erratum:
 * JCAP 1210 (2012) E01.
 */

namespace hermes { namespace darkmatter {
/**
 * \addtogroup DarkMatter
 * @{
 */

class PPPC4DMIDSpectrum : public DarkMatterSpectrum {
  private:
	const std::string filename;
	const Channel channel;
	const Mass mass;
	std::vector<std::pair<double, double> > spectrum;
	void loadData();

  public:
	PPPC4DMIDSpectrum(Channel c, Mass m);
	PPPC4DMIDSpectrum(Channel c, Mass m, const std::string &filename);

	QEnergy getRestMassEnergy() const override;
	QInverseEnergy getParticlesPerEnergy(QEnergy Egamma) const override;
};

/** @}*/
}}  // namespace hermes::darkmatter

#endif  // HERMES_PPPC4DMIDSPECTRUM_H
