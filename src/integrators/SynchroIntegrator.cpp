#include "hermes/integrators/SynchroIntegrator.h"

#include <iostream>
#include <gsl/gsl_sf_synchrotron.h>

namespace hermes {

SynchroIntegrator::SynchroIntegrator(const std::shared_ptr<MagneticField> mfield,
			   const std::shared_ptr<GasDensity> gdensity) : mfield(mfield), gdensity(gdensity) {	
}

SynchroIntegrator::~SynchroIntegrator() { }

SynchroSkymap::tPixel SynchroIntegrator::integral(QDirection direction) {
	constexpr auto const_synchro = std::sqrt(3)*pow<3>(e_plus)/(8*pi*pi*epsilon0*c_light*m_electron);
		
	Vector3QLength positionSun(8.5_kpc, 0, 0);
	Vector3QLength pos(0.0);
	
	// distance from the (spherical) galactic border in the given direction
	QLength maxDistance = distanceToGalBorder(positionSun, direction);

	// TODO: quick hack > implement generic coordinate system transformations
	direction[1] = pi*radian-direction[1]; // rotate for JF12 (tmp)

	QEnergy sum(0);
	QLength delta_d = 10.0_pc;
	// TODO: implement sophisticated adaptive integration method :-)
	for(QLength dist = 0; dist <= maxDistance; dist += delta_d) {
		pos.setRThetaPhi(dist, direction[0], direction[1]);
		pos += positionSun;

// https://www.gnu.org/software/gsl/manual/html_node/Synchrotron-Functions.html
// http://iopscience.iop.org/article/10.1088/1674-4527/13/6/007#
		QEnergy emissivity = const_synchro *
			mfield->getField(pos).getPerpendicularTo(pos) *
			gsl_sf_synchrotron_1(1.0);


		sum += emissivity;
	}
	return sum;
}

} // namespace hermes 
