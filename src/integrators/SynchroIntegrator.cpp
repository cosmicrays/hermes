#include "hermes/integrators/SynchroIntegrator.h"

#include <iostream>
#include <memory>
#include <gsl/gsl_sf_synchrotron.h>

namespace hermes {

SynchroIntegrator::SynchroIntegrator(
	const std::shared_ptr<MagneticField> mfield_,
	const std::shared_ptr<CosmicRayDensity> crdensity_,
	QFrequency freq_) : 
	mfield(mfield_), crdensity(crdensity_), freq(freq_) {	
}

SynchroIntegrator::~SynchroIntegrator() { }

SynchroSkymap::tPixel SynchroIntegrator::integral(QDirection direction) {
	constexpr auto const_synchro =
		std::sqrt(3)*pow<3>(e_plus)/(8*pi*pi*epsilon0*c_light*m_electron);
	QFrequency freq_giro, freq_c;
		
	Vector3QLength positionSun(8.5_kpc, 0, 0);
	Vector3QLength pos(0.0);
	QIntensity sum(0); QIntensity intensity(0);
	Vector3QMField B;
       	QMField B_perp;
	QLength delta_d = 10.0_pc;
	double ratio;

	// https://www.gnu.org/software/gsl/manual/html_node/Synchrotron-Functions.html
	// http://iopscience.iop.org/article/10.1088/1674-4527/13/6/007#
	
	// distance from the (spherical) galactic border in the given direction
	QLength maxDistance = distanceToGalBorder(positionSun, direction);

	// TODO: quick hack > implement generic coordinate system transformations
	direction[1] = pi*radian-direction[1]; // rotate for JF12 (tmp)

	// TODO: implement sophisticated adaptive integration method :-)
	for(QLength dist = 0; dist <= maxDistance; dist += delta_d) {
		pos.setRThetaPhi(dist, direction[0], direction[1]);
		pos += positionSun;
		
		B = mfield->getField(pos);
		B_perp = B.getR() * sin((B.getValue()).getAngleTo(pos.getValue()));
		// TODO: non-relativistic factor (c/v) (see Longair eq. 8.55)
		freq_giro = e_plus * B_perp / (2.*pi*m_electron);

		if (B_perp == 0_T) continue;

		//std::cout << sum << std::endl;
		
		intensity = 0;
		for (auto itE = crdensity->begin(); itE != crdensity->end(); ++itE) {
			freq_c = 3./2. * pow<2>(getLorentzFactor(m_electron, *itE))
				* freq_giro;
			ratio = (freq/freq_c).getValue();
			if (!(ratio > 0 && ratio < 1)) continue; // bug?
			intensity += const_synchro *
				B_perp * gsl_sf_synchrotron_1(ratio) *
			 	crdensity->getDensity(pos, *itE) * (*itE) * delta_d;
		}
		sum += intensity;
	}
	return sum;
}

} // namespace hermes 
