#ifndef HERMES_OUTPUT_H
#define HERMES_OUTPUT_H

#include <string>

namespace hermes { namespace outputs {

class Output {
public:
        const std::string outputName;

        Output() {}
	virtual ~Output() {}

	virtual void initOutput() = 0;
	virtual void createTable(int) = 0;
	virtual void writeMetadata(int, double, const std::string &) = 0;
	virtual void writeKeyValueAsDouble(
		const std::string &key,
		double value,
		const std::string &description) = 0;
	virtual void writeKeyValueAsString(
		const std::string &key, const std::string &value,
		const std::string &description) = 0;
	virtual void writeColumn(int, void *) = 0;
}; 

} // namespace outputs
} // namespace hermes

#endif // HERMES_OUTPUT_H
