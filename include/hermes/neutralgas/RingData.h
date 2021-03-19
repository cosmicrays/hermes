#ifdef HERMES_HAVE_CFITSIO

#ifndef HERMES_RINGDATA_H
#define HERMES_RINGDATA_H

#include <array>
#include <utility>

#include "hermes/FITSWrapper.h"
#include "hermes/Units.h"
#include "hermes/neutralgas/GasType.h"
#include "hermes/neutralgas/NeutralGasAbstract.h"

namespace hermes { namespace neutralgas {

class RingData {
  private:
	std::unique_ptr<FITSFile> ffile;

	const GasType type;
	int n_lon, n_lat, n_rings;
	double min_lon, min_lat;
	double delta_lon, delta_lat;
	std::vector<float> dataVector;

	void readDataFile(const std::string &filename);
	double getRawValue(int ring, const QDirection &dir) const;

  public:
	RingData(GasType gas);
	QColumnDensity getHIColumnDensityInRing(int ring, const QDirection &dir) const;
	QRingCOIntensity getCOIntensityInRing(int ring, const QDirection &dir) const;

	GasType getGasType() const;
	int getRingNumber() const;
};

}}  // namespace hermes::neutralgas

#endif  // HERMES_RINGMODEL_H
#endif  // HERMES_HAVE_CFITSIO