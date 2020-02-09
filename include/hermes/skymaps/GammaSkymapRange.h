#ifndef HERMES_GAMMASKYMAPRANGE_H
#define HERMES_GAMMASKYMAPRANGE_H

#include <hermes/skymaps/GammaSkymap.h>

namespace hermes {

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
	GammaSkymapRange(std::size_t nside_, QEnergy minEn_, QEnergy maxEn_, int enSteps_);
	~GammaSkymapRange();

	void setIntegrator(std::shared_ptr<IntegratorTemplate<QDifferentialIntensity, QEnergy> > integrator_);
	void setMask(std::shared_ptr<SkymapMask> mask_);
	void compute();
       
	/** output **/
	void save(std::shared_ptr<Output> output) const;
 
	/** iterator goodies */
        typedef typename tSkymapsContainer::iterator iterator;
        typedef typename tSkymapsContainer::const_iterator const_iterator;
        iterator begin();
        const_iterator begin() const;
        iterator end();
        const_iterator end() const;
};

} // namespace hermes

#endif // HERMES_GAMMASKYMAPRANGE_H
