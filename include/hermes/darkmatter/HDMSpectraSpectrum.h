#ifndef HERMES_HDMSPECTRASPECTRUM_H
#define HERMES_HDMSPECTRASPECTRUM_H

#include <iosfwd>
#include <string>
#include <utility>
#include <vector>

#include "hermes/Units.h"
#include "hermes/darkmatter/DarkMatterSpectrum.h"
#include "hermes/darkmatter/PPPC4DMIDChannels.h"

namespace hermes { namespace darkmatter {
/**
 * \addtogroup DarkMatter
 * @{
 */

enum class HDMSpectraProduct { gamma, neutrino, proton, neutron };

class HDMSpectraSpectrum : public DarkMatterSpectrum {
  private:
	struct MassSpectrum {
		double massGeV;
		double log10MassGeV;
		std::vector<std::pair<double, double> > spectrum;
	};

	const std::string filename;
	const Channel channel;
	const HDMSpectraProduct product;
	const QEnergy restMassEnergy;
	std::vector<MassSpectrum> spectra;

	void loadData();
	void loadDataFromStream(std::istream &input);
	void validateRequestedMass() const;
	double interpolateInX(const MassSpectrum &massSpectrum,
	                      double log10x) const;
	double interpolate(double log10x) const;
	double getRestMassEnergyGeV() const;

  public:
	HDMSpectraSpectrum(Channel c, QEnergy m,
	                   HDMSpectraProduct p = HDMSpectraProduct::gamma);
	HDMSpectraSpectrum(Channel c, QEnergy m, HDMSpectraProduct p,
	                   const std::string &filename);

	Channel getChannel() const;
	HDMSpectraProduct getProduct() const;
	const std::string &getFilename() const;
	QEnergy getRestMassEnergy() const override;
	QInverseEnergy getParticlesPerEnergy(QEnergy energy) const override;
};

/** @}*/
}}  // namespace hermes::darkmatter

#endif  // HERMES_HDMSPECTRASPECTRUM_H
