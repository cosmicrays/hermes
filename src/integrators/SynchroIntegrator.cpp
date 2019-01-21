#include "hermes/integrators/SynchroIntegrator.h"

#include <iostream>
#include <memory>
#include <gsl/gsl_sf_synchrotron.h>

namespace hermes {

SynchroIntegrator::SynchroIntegrator(
	const std::shared_ptr<MagneticField> mfield_,
	const std::shared_ptr<CosmicRayDensity> crdensity_) : 
	mfield(mfield_), crdensity(crdensity_) {	
}

SynchroIntegrator::~SynchroIntegrator() { }

QTemperature SynchroIntegrator::intensityToTemperature(QIntensity intensity) {
	return intensity*c_squared /
		(2*pow<2>(working_skymap->getFrequency())*k_boltzmann);
}

SynchroSkymap::tPixel SynchroIntegrator::integrateOverLOS(QDirection direction) {

	Vector3QLength positionSun(8.5_kpc, 0, 0);
	Vector3QLength pos(0.0);
	QIntensity total_intensity(0);
	QLength delta_d = 10.0_pc;

	// distance from the (spherical) galactic border in the given direction
	QLength maxDistance = distanceToGalBorder(positionSun, direction);

	// TODO: quick hack > implement generic coordinate system transformations
	direction[1] = pi*radian-direction[1]; // rotate for JF12 (tmp)

	// TODO: implement sophisticated adaptive integration method :-)
	for(QLength dist = 0; dist <= maxDistance; dist += delta_d) {
		pos.setRThetaPhi(dist, direction[0], direction[1]);
		pos += positionSun;
		
		total_intensity +=
			integrateOverEnergy(pos, working_skymap->getFrequency()) * delta_d;
	}

	return intensityToTemperature(total_intensity / 4_pi);
}

QEmissivity SynchroIntegrator::integrateOverEnergy(Vector3QLength pos, QFrequency freq) {

	QEmissivity emissivity(0);
	constexpr auto const_synchro =
		std::sqrt(3)*pow<3>(e_plus)/(8*pi*pi*epsilon0*c_light*m_electron);
	QEnergy deltaE;
	QFrequency freq_c, freq_giro;
	Vector3QMField B;
       	QMField B_perp;
	double ratio;

	B = mfield->getField(pos);
	// skip B null-vector as it will produce NaN in the next step
	if (B.getR() == 0_muG) return emissivity;

	B_perp = B.getR() * sin( (B.getValue()).getAngleTo(pos.getValue()) );
	// TODO: non-relativistic factor (c/v) (see Longair eq. 8.55)
	freq_giro = e_plus * B_perp / (2.*pi*m_electron);

	if (B_perp == 0_T) return emissivity;

	for (auto itE = std::next(crdensity->begin()); itE != crdensity->end(); ++itE) {
		deltaE = (*itE) - *std::prev(itE);

		freq_c = 3./2. * pow<2>(getLorentzFactor(m_electron, *itE))
                	* freq_giro;
                ratio = (freq/freq_c).getValue();
        
		if (ratio > 100) continue; // speed-up by skipping negligible contributions
		// F(100) ~ 4e-43
		// https://www.gnu.org/software/gsl/manual/html_node/Synchrotron-Functions.html
		// http://iopscience.iop.org/article/10.1088/1674-4527/13/6/007#
		
		emissivity += const_synchro *
                	B_perp * gsl_sf_synchrotron_1(ratio) *
                        crdensity->getDensityPerEnergy(pos, *itE) * deltaE;
        }

	return emissivity;
}

} // namespace hermes 
