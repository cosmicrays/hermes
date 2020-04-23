#ifndef HERMES_GAMMASKYMAPRANGE_H
#define HERMES_GAMMASKYMAPRANGE_H

#include <hermes/skymaps/GammaSkymap.h>

namespace hermes {
/**
 * \addtogroup Skymaps
 * @{
 */

/**
 @class GammaSkymapRange
 @brief A range of GammaSkymap containers.
 */
class GammaSkymapRange {
  private:
	typedef std::vector<GammaSkymap> tSkymapsContainer;
	tSkymapsContainer skymaps;
	std::vector<QEnergy> energies;
	QEnergy minEn, maxEn;
	std::size_t nside;
	int enSteps;
	void initEnergyRange();

  public:
	GammaSkymapRange(std::size_t nside_, QEnergy minEn_, QEnergy maxEn_,
	                 int enSteps_);
	~GammaSkymapRange();

	void setIntegrator(
	    const std::shared_ptr<IntegratorTemplate<QDiffIntensity, QEnergy>>&
	        integrator_);
	void setMask(const std::shared_ptr<SkymapMask>& mask_);

	std::size_t size() const;
	GammaSkymap operator[](std::size_t ipix) const;

	void compute();

	/** output **/
	void save(const std::shared_ptr<outputs::Output>& output) const;

	/** iterator goodies */
	typedef typename tSkymapsContainer::iterator iterator;
	typedef typename tSkymapsContainer::const_iterator const_iterator;
	iterator begin();
	const_iterator begin() const;
	iterator end();
	const_iterator end() const;
};

/** @}*/
}  // namespace hermes
#endif  // HERMES_GAMMASKYMAPRANGE_H
