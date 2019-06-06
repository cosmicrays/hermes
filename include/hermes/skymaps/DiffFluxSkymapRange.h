#ifndef HERMES_DIFFFLUXSKYMAPRANGE_H
#define HERMES_DIFFFLUXSKYMAPRANGE_H

#include <hermes/skymaps/DiffFluxSkymap.h>

namespace hermes {

class DiffFluxSkymapRange {
private:
	typedef std::vector<DiffFluxSkymap> tSkymapsContainer;
	tSkymapsContainer skymaps;
	std::vector<QEnergy> energies;
	QEnergy minEn, maxEn;
	std::size_t nside;
	int enSteps;
	void initEnergyRange();
public:
	DiffFluxSkymapRange(std::size_t nside_, QEnergy minEn_, QEnergy maxEn_, int enSteps_);
	~DiffFluxSkymapRange();

	void setIntegrator(std::shared_ptr<IntegratorTemplate<QDifferentialFlux, QEnergy> > integrator_);
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

#endif // HERMES_DIFFFLUXSKYMAPRANGE_H
