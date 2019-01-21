#ifdef HERMES_HAVE_CFITSIO

#ifndef HERMES_FITSOUTPUT_H
#define HERMES_FITSOUTPUT_H

#include "hermes/FITSWrapper.h"
#include "hermes/outputs/Output.h"

namespace hermes {


class FITSOutput: public Output {
private:
	std::string filename;
	fitsfile *fptr = 0;
public:
	FITSOutput(const std::string &filename);
	virtual void save(std::shared_ptr<Skymap> skymap);
};

} // namespace hermes

#endif // HERMES_FITSOUTPUT_H

#endif // HERMES_HAVE_CFITSIO
