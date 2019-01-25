#ifndef HERMES_OUTPUT_H
#define HERMES_OUTPUT_H

#include <string>

namespace hermes {

class Output {
public:
        const std::string outputName;
        Output() {};
	virtual void initOutput() = 0;
	virtual void createTable(int) = 0;
	virtual void writeMetadata(int, double, const std::string &) = 0;
	virtual void writeColumn(int, void *) = 0;
}; 

} // namespace hermes

#endif // HERMES_OUTPUT_H
