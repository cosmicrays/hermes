#ifdef HERMES_HAVE_CFITSIO

#ifndef HERMES_HEALPIXFORMAT_H
#define HERMES_HEALPIXFORMAT_H

#include <memory>

#include "hermes/FITSWrapper.h"
#include "hermes/outputs/Output.h"

namespace hermes { namespace outputs {

/* \class HEALPixFormat
 * \brief Saves skymap to the HEALPix format (used by Healpy)
 *
 * Follows Hivon et al. 2019:
 * "FITS file format specifications for HEALPix products"
 * URL: https://healpix.sourceforge.io/data/examples/healpix_fits_specs.pdf
 * for the base header and common keywords
 */
class HEALPixFormat : public Output {
  private:
	std::string filename;
	std::unique_ptr<FITSFile> ffile;

  public:
	/** Saves output to a \p filename */
	HEALPixFormat(const std::string &filename);
	/** The primary header
	 Outputs:
	 SIMPLE =  T / file does conform to FITS standard
	 BITPIX = 32 / number of bits per data pixel
	 NAXIS  =  0 / number of data axes
	 EXTEND =  T / FITS dataset may contain extensions
	 DATE   = ’2099-12-31T23:59:59’ / file creation date (YYYY-MM-DDThh:mm:ss
	 UT) END
	*/
	void initOutput() override;
	/** Creates binary table
	 \param nrows	Number of rows in the table
	 \param tunit1	Physical unit of data in the table
	*/
	void createTable(int nrows, const std::string &unit) override;
	/** The healpix HDU header
	 PIXTYPE = 'HEALPIX' / HEALPIX Pixelisation
	 INDXSCHM= 'IMPLICIT' / Indexing : IMPLICIT or EXPLICIT
	 ORDERING= 'RING' / Pixel ordering scheme
	 NSIDE   = 2^n / where n={0, 29}
	 FIRSTPIX= 0 / First pixel
	 LASTPIX = 12*nside^2-1 / Last pixel
	 BAD_DATA= -1.6375000E+30 / Sentinel value given to missing or bad pixels
	 COORDSYS= 'G' / Pixelisation coordinate system, G = Galactic
	*/
	void writeMetadata(int nside, double res, bool hasMask,
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
