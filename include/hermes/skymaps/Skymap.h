#ifndef HERMES_SKYMAP_H
#define HERMES_SKYMAP_H

#include <iostream>
#include <memory>
#include <vector>

#include "hermes/HEALPixBits.h"
#include "hermes/outputs/Output.h"

namespace hermes {
/**
 * \addtogroup Skymaps
 * @{
 */

/**
 \class Skymap
 \brief Base abstract class for all Skymaps; provides elementary HEALPix
 functions

 res	: The resolution index
 nside	= 2^res		: The number of pixels per side.
 npix	= 12 * nside^2	: The total number of pixels in the map.
 */
class Skymap {
  protected:
	std::size_t nside;
	std::size_t npix;
	std::size_t res;
	std::string description;

  public:
	Skymap(std::size_t nside_);
	virtual ~Skymap(){};

	void setNside(std::size_t nside_);
	std::size_t getNside() const;
	void setRes(std::size_t res_);
	std::size_t getRes() const;
	std::size_t getNpix() const;

	std::string getDescription() const;
	void setDescription(const std::string &description);

	virtual double getPixelAsDouble(std::size_t i) const = 0;
	virtual void save(std::shared_ptr<outputs::Output> output) const = 0;
};

/** @}*/
}  // namespace hermes

#endif  // HERMES_SKYMAP_H
