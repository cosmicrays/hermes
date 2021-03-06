#ifndef HERMES_SKYMAPTEMP_H
#define HERMES_SKYMAPTEMP_H

#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "hermes/Common.h"
#include "hermes/ProgressBar.h"
#include "hermes/Signals.h"
#include "hermes/Units.h"
#include "hermes/integrators/IntegratorTemplate.h"
#include "hermes/skymaps/Skymap.h"
#include "hermes/skymaps/SkymapMask.h"

/**
 \file SkymapTemplate.h
 \brief Contains \p SkymapTemplate definition used in all skymaps.
 */

namespace hermes {
/**
 * \addtogroup Skymaps
 * @{
 */

/**
 \class SkymapDefinitions
 \brief Struct for internal usage in Skymap classes
 */
struct SkymapDefinitions {
	const std::string skymapParameterName;

	SkymapDefinitions() : skymapParameterName("NONE") { }
	SkymapDefinitions(const std::string &pName) : skymapParameterName(pName) { }
};

/**
 \class SkymapTemplate
 \brief Provides a HEALPix-compatibile container template with undefined units
 of pixels; should be inherited in every process-specific skymap.
 \tparam QPXL A type of pixel which a skymap contains (e.g.,
 units::QTemperature, units::QRotationMeasure)
 \tparam QSTEP A physical quantity (parameter) that describes a particular map
 (if needed), e.g., units::QFrequency, units::QEnergy
 */
template <typename QPXL, typename QSTEP>
class SkymapTemplate : public Skymap {
  protected:
	typedef QPXL tPixel;

	typedef std::vector<tPixel> tFluxContainer;
	mutable tFluxContainer fluxContainer;
	mutable std::vector<bool> maskContainer;

	mutable tPixel defaultOutputUnits;
	mutable std::string defaultOutputUnitsString;

	QSTEP skymapParameter = 0;
	const SkymapDefinitions defs;

	std::shared_ptr<SkymapMask> mask;
	std::shared_ptr<IntegratorTemplate<QPXL, QSTEP>> integrator;

	std::shared_ptr<ProgressBar> progressbar;
	std::shared_ptr<std::mutex> progressbar_mutex;

	void initDefaultOutputUnits(QPXL units, const std::string &unitsString);
	void initContainer();
	void initMask();

  public:
	SkymapTemplate(std::size_t nside, const SkymapDefinitions &s);
	SkymapTemplate(std::size_t nside = 64, const QSTEP &p = QSTEP(0), const SkymapDefinitions &s = SkymapDefinitions());
	virtual ~SkymapTemplate();

	/**
	    Setter for the skymap parameter
	*/
	void setSkymapParameter(const QSTEP &p) { skymapParameter = p; }
	/**
	    Getter for the skymap parameter
	*/
	QSTEP getSkymapParameter() const { return skymapParameter; }
	/**
	    Size of the skymap container (same as getNPix())
	*/
	std::size_t size() const;
	/**
	    Calculate the total number of unmasked pixels
	*/
	std::size_t getUnmaskedPixelCount() const;
	QPXL getPixel(std::size_t ipix) const;
	/**
	    Retrieve ith pixel as naked double
		\par i	ith pixel (starting from 0)
	*/
	double getPixelAsDouble(std::size_t i) const;
	/**
	    Calculate the mean value of unmasked pixels
	*/
	QPXL getMean() const;
	/**
	    Checks if the skymap is masked
	*/
	bool hasMask() const;
	/**
	    Pixel accessor [] (same as getPixel)
	*/
	QPXL operator[](std::size_t ipix) const;
	/**
	    Returns a pointer to the pixel container
	*/
	QPXL *data() { return fluxContainer.data(); }
	/**
	    Set the line of sight integrator
	*/
	void setIntegrator(
	    std::shared_ptr<IntegratorTemplate<QPXL, QSTEP>> integrator_);
	void setOutput();

	void printPixels() const;
	void setMask(std::shared_ptr<SkymapMask> mask_);
	std::vector<bool> getMask() const;
	inline bool isMasked(std::size_t pixel) const;

	virtual void computePixel(
	    std::size_t ipix,
	    const std::shared_ptr<IntegratorTemplate<QPXL, QSTEP>> &integrator_);
	void computePixelRange(
	    std::vector<std::size_t> chunk,
	    const std::shared_ptr<IntegratorTemplate<QPXL, QSTEP>> &integrator_);
	void compute();

	/** output **/
	void convertToUnits(QPXL units_, const std::string &defaultUnitsString);
	QNumber toSkymapDefaultUnits(const QPXL pixel) const;
	QPXL getOutputUnits() const;
	std::string getOutputUnitsAsString() const;
	std::string getUnits() const;
	std::vector<float> containerToRawVector() const;
	void save(std::shared_ptr<outputs::Output> output) const;

	/** iterator goodies */
	typedef typename tFluxContainer::iterator iterator;
	typedef typename tFluxContainer::const_iterator const_iterator;
	iterator begin();
	const_iterator begin() const;
	iterator end();
	const_iterator end() const;
};

/* Definitions */

/* Constructors */
template <typename QPXL, typename QSTEP>
SkymapTemplate<QPXL, QSTEP>::SkymapTemplate(
    std::size_t nside, const SkymapDefinitions &s)
    : Skymap(nside),
      skymapParameter(QSTEP(0)),
	  defs(s),
      defaultOutputUnits(QPXL(1)),
      defaultOutputUnitsString("SI Base Units") {
	initContainer();
	initMask();
}
template <typename QPXL, typename QSTEP>
SkymapTemplate<QPXL, QSTEP>::SkymapTemplate(std::size_t nside, const QSTEP &p, const SkymapDefinitions &s)
    : Skymap(nside),
      skymapParameter(p),
	  defs(s),
      defaultOutputUnits(QPXL(1)),
      defaultOutputUnitsString("SI Base Units") {
	initContainer();
	initMask();
}

/* Destructor */
template <typename QPXL, typename QSTEP>
SkymapTemplate<QPXL, QSTEP>::~SkymapTemplate() {
	fluxContainer.clear();
}

/* Initializers */
template <typename QPXL, typename QSTEP>
void SkymapTemplate<QPXL, QSTEP>::initContainer() {
	fluxContainer.reserve(npix);
	fluxContainer.insert(fluxContainer.begin(), npix, UNSEEN);
}
template <typename QPXL, typename QSTEP>
void SkymapTemplate<QPXL, QSTEP>::initMask() {
	if (mask == nullptr) mask = std::make_shared<SkymapMask>(SkymapMask());
	maskContainer = mask->getMask(nside);
}

/* Getters */
template <typename QPXL, typename QSTEP>
std::size_t SkymapTemplate<QPXL, QSTEP>::size() const {
	return fluxContainer.size();
}

template <typename QPXL, typename QSTEP>
std::size_t SkymapTemplate<QPXL, QSTEP>::getUnmaskedPixelCount() const {
	return std::count(maskContainer.begin(), maskContainer.end(), true);
}

template <typename QPXL, typename QSTEP>
QPXL SkymapTemplate<QPXL, QSTEP>::getPixel(std::size_t i) const {
	return fluxContainer[i];
}

template <typename QPXL, typename QSTEP>
double SkymapTemplate<QPXL, QSTEP>::getPixelAsDouble(std::size_t i) const {
	return static_cast<double>(fluxContainer[i]);
}

template <typename QPXL, typename QSTEP>
QPXL SkymapTemplate<QPXL, QSTEP>::getMean() const {
	QPXL accum(0);
	std::size_t count = 0;
	for (const auto &pxl : fluxContainer) {
		if (pxl == QPXL(UNSEEN)) continue;
		accum += pxl;
		count++;
	}
	return accum / count;
}

template <typename QPXL, typename QSTEP>
bool SkymapTemplate<QPXL, QSTEP>::hasMask() const {
	if (getUnmaskedPixelCount() < getNpix()) {
		return true;
	}
	return false;
}

template <typename QPXL, typename QSTEP>
QPXL SkymapTemplate<QPXL, QSTEP>::operator[](std::size_t i) const {
	return fluxContainer[i];
}

template <typename QPXL, typename QSTEP>
void SkymapTemplate<QPXL, QSTEP>::printPixels() const {
	for (auto i : fluxContainer) std::cout << static_cast<double>(i) << ' ';
}

template <typename QPXL, typename QSTEP>
void SkymapTemplate<QPXL, QSTEP>::setIntegrator(
    std::shared_ptr<IntegratorTemplate<QPXL, QSTEP>> integrator_) {
	integrator = integrator_;
	setDescription(integrator->getDescription());
}

template <typename QPXL, typename QSTEP>
void SkymapTemplate<QPXL, QSTEP>::computePixel(
    std::size_t ipix,
    const std::shared_ptr<IntegratorTemplate<QPXL, QSTEP>> &integrator_) {
	fluxContainer[ipix] =
	    integrator_->integrateOverLOS(pix2ang_ring(getNside(), ipix));
}

template <typename QPXL, typename QSTEP>
void SkymapTemplate<QPXL, QSTEP>::computePixelRange(
    std::vector<std::size_t> chunk,
    const std::shared_ptr<IntegratorTemplate<QPXL, QSTEP>> &integrator_) {
	for (auto ipxl : chunk) {
			computePixel(ipxl, integrator_);
			progressbar->update();
	}
}

template <typename QPXL, typename QSTEP>
void SkymapTemplate<QPXL, QSTEP>::compute() {
	std::cout << "hermes::Integrator: Number of Threads: " << getThreadsNumber()
	          << std::endl;

	if (integrator == nullptr)
		throw std::runtime_error(
		    "Provide an integrator with Skymap::setIntegrator()");

	// Generate cache tables in integrator for a given skymap parameter
	if (integrator->isCacheTableEnabled()) {
		integrator->setSkymapParameter(skymapParameter);
		integrator->initCacheTable();
	}

	// Progressbar init
	progressbar =
	    std::make_shared<ProgressBar>(ProgressBar(getUnmaskedPixelCount()));
	progressbar_mutex = std::make_shared<std::mutex>();
	progressbar->setMutex(progressbar_mutex);
	progressbar->start("Compute skymap");

    std::vector<std::size_t> validPixels;
    for (std::size_t ipxl = 0; ipxl < fluxContainer.size(); ++ipxl) {
        if (!isMasked(ipxl)) {
            validPixels.push_back(ipxl);
        } else {
            fluxContainer[ipxl] = QPXL(UNSEEN);
        }
    }
            
    auto job_chunks = getIndexedThreadChunks(validPixels);
	std::vector<std::thread> threads;
	for (auto &chunk : job_chunks) {
		threads.push_back(
		    std::thread(&SkymapTemplate<QPXL, QSTEP>::computePixelRange, this,
		                chunk, integrator));
	}
	for (auto &t : threads) {
		t.join();
	}
}

template <typename QPXL, typename QSTEP>
void SkymapTemplate<QPXL, QSTEP>::initDefaultOutputUnits(
    QPXL units_, const std::string &unitsString_) {
	defaultOutputUnits = units_;
	defaultOutputUnitsString = unitsString_;
}

template <typename QPXL, typename QSTEP>
QNumber SkymapTemplate<QPXL, QSTEP>::toSkymapDefaultUnits(
    const QPXL pixel) const {
	return pixel / defaultOutputUnits;
}

template <typename QPXL, typename QSTEP>
void SkymapTemplate<QPXL, QSTEP>::convertToUnits(
    QPXL units_, const std::string &unitsString_) {
	if (units_ == defaultOutputUnits) return;

	for (auto &i : fluxContainer) {
		if (static_cast<double>(i) != UNSEEN)
			i = i / (units_ / defaultOutputUnits);
	}

	defaultOutputUnits = units_;
	defaultOutputUnitsString = unitsString_;
}

template <typename QPXL, typename QSTEP>
QPXL SkymapTemplate<QPXL, QSTEP>::getOutputUnits() const {
	return defaultOutputUnits;
}

template <typename QPXL, typename QSTEP>
std::string SkymapTemplate<QPXL, QSTEP>::getOutputUnitsAsString() const {
	return defaultOutputUnitsString;
}

template <typename QPXL, typename QSTEP>
std::string SkymapTemplate<QPXL, QSTEP>::getUnits() const {
	return getOutputUnitsAsString();
}

template <typename QPXL, typename QSTEP>
std::vector<float> SkymapTemplate<QPXL, QSTEP>::containerToRawVector() const {
	std::vector<float> tempArray;  // allocate on heap, because of nside >= 512
	for (auto pxl : fluxContainer) {
		float converted_pxl =  // don't convert UNSEEN pixels
		    pxl == QPXL(UNSEEN) ? static_cast<float>(pxl)
		                        : static_cast<float>(toSkymapDefaultUnits(pxl));
		tempArray.push_back(converted_pxl);
	}
	return tempArray;
}

template <typename QPXL, typename QSTEP>
void SkymapTemplate<QPXL, QSTEP>::save(
    std::shared_ptr<outputs::Output> output) const {
	output->createTable(static_cast<int>(npix), getOutputUnitsAsString());
	output->writeMetadata(nside, res, hasMask(), description);
	output->writeKeyValueAsDouble(defs.skymapParameterName,
								  static_cast<double>(getSkymapParameter()),
		                          std::string("Skymap frequency/energy/... (in SI base unit)"));
	auto tempArray = containerToRawVector();
	output->writeColumn(npix, tempArray.data());
}

template <typename QPXL, typename QSTEP>
void SkymapTemplate<QPXL, QSTEP>::setMask(std::shared_ptr<SkymapMask> mask_) {
	mask = mask_;
	initMask();
}

template <typename QPXL, typename QSTEP>
std::vector<bool> SkymapTemplate<QPXL, QSTEP>::getMask() const {
	return maskContainer;
}

template <typename QPXL, typename QSTEP>
inline bool SkymapTemplate<QPXL, QSTEP>::isMasked(std::size_t ipix) const {
	return (maskContainer[ipix] == false);
}

template <typename QPXL, typename QSTEP>
typename SkymapTemplate<QPXL, QSTEP>::iterator
SkymapTemplate<QPXL, QSTEP>::begin() {
	return fluxContainer.begin();
}

template <typename QPXL, typename QSTEP>
typename SkymapTemplate<QPXL, QSTEP>::const_iterator
SkymapTemplate<QPXL, QSTEP>::begin() const {
	return fluxContainer.begin();
}

template <typename QPXL, typename QSTEP>
typename SkymapTemplate<QPXL, QSTEP>::iterator
SkymapTemplate<QPXL, QSTEP>::end() {
	return fluxContainer.end();
}

template <typename QPXL, typename QSTEP>
typename SkymapTemplate<QPXL, QSTEP>::const_iterator
SkymapTemplate<QPXL, QSTEP>::end() const {
	return fluxContainer.end();
}

/** @}*/
}  // namespace hermes

#endif  // HERMES_SKYMAPTEMP_H
