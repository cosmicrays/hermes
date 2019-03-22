#ifndef HERMES_SYNCHROSKYMAPRANGE_H
#define HERMES_SYNCHROSKYMAPRANGE_H

#include <hermes/skymaps/SynchroSkymap.h>

namespace hermes {

class SynchroSkymapRange {
private:
	typedef std::vector<SynchroSkymap> tSkymapsContainer;
	tSkymapsContainer skymaps;
	std::vector<QFrequency> freqs;
	QFrequency minFreq, maxFreq;
	std::size_t nside;
	int freqSteps;
	void initFrequencyRange();
public:
	SynchroSkymapRange(std::size_t nside_, QFrequency minFreq_, QFrequency maxFreq_, int freqSteps_);
	~SynchroSkymapRange();

	void setIntegrator(std::shared_ptr<IntegratorTemplate<QTemperature> > integrator_);
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

#endif // HERMES_SYNCHROSKYMAPRANGE_H
