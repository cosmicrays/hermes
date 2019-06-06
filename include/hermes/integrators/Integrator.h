#ifndef HERMES_INTEGRATOR_H
#define HERMES_INTEGRATOR_H

#include "hermes/Units.h"
#include "hermes/Grid.h"
#include "hermes/HEALPixBits.h"
#include "hermes/Common.h"

namespace hermes {

template <class QPXL, typename QSTEP>
class IntegratorTemplate {
public:
	IntegratorTemplate() { };
	/**
 		Every child class should implement this method which represents an integral
		of a targeted accumulated quantity `T` in a given direction `interdir`.
	*/
	virtual QPXL integrateOverLOS(QDirection iterdir) const { return  QPXL(0); };
	/**
		Additionally, for a frequency or energy dependent integrals one should
		implement the following method too.
	*/
	virtual QPXL integrateOverLOS(QDirection iterdir, QSTEP) const { return QPXL(0); };
};

} // namespace hermes

#endif // HERMES_INTEGRATOR_H
