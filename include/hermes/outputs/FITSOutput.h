#ifndef HERMES_FITSOUTPUT_H
#define HERMES_FITSOUTPUT_H

#include "hermes/outputs/Output.h"

#include <string>
#include "fitsio.h"

namespace hermes {

class FITSOutput: public Output {
private:
	std::string filename;
	fitsfile *fptr = 0;

	int create_file();
	int write_key(const char key[], int type, void *value, const char comment[]);
public:
	FITSOutput(const std::string &filename);
	virtual void save(std::shared_ptr<Skymap> skymap);
};

} // namespace hermes

#endif // HERMES_FITSOUTPUT_H
