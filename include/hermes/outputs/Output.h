#ifndef HERMES_OUTPUT_H
#define HERMES_OUTPUT_H

#include "hermes/skymaps/Skymap.h"

#include <memory>

namespace hermes {

class Output {
public:
        const std::string outputName;
        Output() {};
	virtual void save(std::shared_ptr<Skymap> skymap) = 0;
}; 

} // namespace hermes

#endif // HERMES_OUTPUT_H
