#ifdef HERMES_HAVE_CFITSIO

#ifndef HERMES_FITSOUTPUT_H
#define HERMES_FITSOUTPUT_H

#include "hermes/outputs/Output.h"
#include "hermes/FITSWrapper.h"
#include <memory>

namespace hermes {


class FITSOutput: public Output {
private:
	std::string filename;
	std::unique_ptr<FITSFile> ffile;
public:
	FITSOutput(const std::string &filename);
	void initOutput();
	void createTable(int nrows);
	void writeMetadata(int nside, double res, const std::string &description);
	void writeColumn(int nElements, void *array);
};

} // namespace hermes

#endif // HERMES_FITSOUTPUT_H

#endif // HERMES_HAVE_CFITSIO
