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

QTemperature SynchroIntegrator::intensityToTemperature(
		QIntensity intensity_, QFrequency freq_) const {
	return intensity_*c_squared /
		(2*freq_*freq_*k_boltzmann);
}

QTemperature SynchroIntegrator::integrateOverLOS(
		QDirection direction) const {
	return integrateOverLOS(direction, 408_MHz);
}

QTemperature SynchroIntegrator::integrateOverLOS(
		QDirection direction, QFrequency freq_) const {

	Vector3QLength positionSun(8.5_kpc, 0, 0);
	Vector3QLength pos(0.0);
	QIntensity total_intensity(0);
	QLength delta_d = 5.0_pc;

	// distance from the (spherical) galactic border in the given direction
	QLength maxDistance = distanceToGalBorder(positionSun, direction);

	// TODO: implement sophisticated adaptive integration method :-)
	for(QLength dist = 0; dist <= maxDistance; dist += delta_d) {
		pos = getGalacticPosition(positionSun, dist, direction);

		if (crdensity->existsScaleFactor())
			total_intensity +=
				integrateOverLogEnergy(pos, freq_) * delta_d;
		else
			total_intensity +=
				integrateOverEnergy(pos, freq_) * delta_d;
	}
	return intensityToTemperature(total_intensity / 4_pi, freq_);
}

QEmissivity SynchroIntegrator::integrateOverEnergy(Vector3QLength pos_, QFrequency freq_) const {

	QEmissivity emissivity(0);
	QEnergy deltaE;
	QFrequency freq_c, freq_giro;
	Vector3QMField B;
       	QMField B_perp;
	double ratio;
	QNumber gamma;

	B = mfield->getField(pos_);
	// skip B null-vector as it will produce NaN in the next step
	if (B.getR() == 0_muG) return emissivity;
	if (pos_ == Vector3QLength(0)) return emissivity; // skip the origin
	B_perp = B.getR() * sin( (B.getValue()).getAngleTo(pos_.getValue()) );
	if (B_perp == 0_T) return emissivity;
	
	// TODO: non-relativistic factor (c/v) (see Longair eq. 8.55)
	freq_giro = e_plus * B_perp / (2.*pi*m_electron);


	for (auto itE = std::next(crdensity->begin()); itE != crdensity->end(); ++itE) {
		deltaE = (*itE) - *std::prev(itE);

		freq_c = 3./2. * pow<2>(getLorentzFactor(m_electron, *itE))
		       	* freq_giro;
                ratio = (freq_/freq_c).getValue();
        
		if (ratio > 100) continue; // speed-up by skipping negligible contributions
		// F(100) ~ 4e-43
		// https://www.gnu.org/software/gsl/manual/html_node/Synchrotron-Functions.html
		// http://iopscience.iop.org/article/10.1088/1674-4527/13/6/007#
		
		emissivity += const_synchro *
                	B_perp * gsl_sf_synchrotron_1(ratio) *
                        crdensity->getDensityPerEnergy(*itE, pos_) * deltaE;
        }
	return emissivity;
}

QEmissivity SynchroIntegrator::integrateOverLogEnergy(Vector3QLength pos_, QFrequency freq_) const {

	QEmissivity emissivity(0);
	QEnergy deltaE;
	QFrequency freq_c, freq_giro;
	Vector3QMField B;
       	QMField B_perp;
	double ratio;

	B = mfield->getField(pos_);
	if (B.getR() == 0_muG) return emissivity;
	if (pos_ == Vector3QLength(0)) return emissivity; // skip the origin
	B_perp = B.getR() * sin( (B.getValue()).getAngleTo(pos_.getValue()) );
	if (B_perp == 0_T) return emissivity;

	freq_giro = e_plus * B_perp / (2.*pi*m_electron);

	for (auto itE = crdensity->begin(); itE != crdensity->end(); ++itE) {
		freq_c = 3./2. * pow<2>(getLorentzFactor(m_electron, *itE))
                	* freq_giro;
                ratio = (freq_/freq_c).getValue();
        
		if (ratio > 100) continue; // speed-up by skipping negligible contributions
		
		emissivity += const_synchro *
                	B_perp * gsl_sf_synchrotron_1(ratio) *
                        crdensity->getDensityPerEnergy(*itE, pos_) * (*itE);
        }

	return emissivity * log(crdensity->getEnergyScaleFactor());
}

} // namespace hermes 
