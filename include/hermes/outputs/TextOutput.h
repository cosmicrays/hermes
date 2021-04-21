#ifndef HERMES_TEXTOUTPUT_H
#define HERMES_TEXTOUTPUT_H

#include "hermes/outputs/Output.h"

#include <memory>
#include <fstream>

namespace hermes {

class TextOutput: public Output {
private:
	std::string filename;
        std::ostream *out;
        std::ofstream outfile;
public:
	TextOutput(const std::string &filename);
	void initOutput() override;
	void createTable(int nrows) override;
	void writeMetadata(int nside, double res,
		const std::string &description) override;
	void writeKeyValueAsDouble(
		const std::string &key, double value,
		const std::string &description) override;
	void writeKeyValueAsString(
		const std::string &key, const std::string &value,
		const std::string &description) override;
	void writeColumn(int nElements, void *array) override;
};

} // namespace hermes

#endif // HERMES_TEXTOUTPUT_H
