#ifdef HERMES_HAVE_CFITSIO

#ifndef HERMES_HEALPIXFORMAT_H
#define HERMES_HEALPIXFORMAT_H

#include <memory>

#include "hermes/FITSWrapper.h"
#include "hermes/outputs/Output.h"

namespace hermes { namespace outputs {

/* Based on Hivon et al. 2019:
 * "FITS file format specifications for HEALPix products"
 * URL: https://healpix.sourceforge.io/data/examples/healpix_fits_specs.pdf
 */
class HEALPixFormat : public Output {
  private:
	std::string filename;
	std::unique_ptr<FITSFile> ffile;

  public:
	HEALPixFormat(const std::string &filename);
	void initOutput() override;
	void createTable(int nrows) override;
	void writeMetadata(int nside, double res,
	                   const std::string &description) override;
	void writeKeyValueAsDouble(const std::string &key, double value,
	                           const std::string &description) override;
	void writeKeyValueAsString(const std::string &key, const std::string &value,
	                           const std::string &description) override;
	void writeColumn(int nElements, void *array) override;
};

}}  // namespace hermes::outputs

#endif  // HERMES_HEALPIXFORMAT_H

#endif  // HERMES_HAVE_CFITSIO
