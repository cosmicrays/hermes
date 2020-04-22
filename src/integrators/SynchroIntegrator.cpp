#include "hermes/integrators/SynchroIntegrator.h"
#include "hermes/Common.h"
#include "hermes/integrators/LOSIntegrationMethods.h"

#include <gsl/gsl_sf_synchrotron.h>
#include <memory>

namespace hermes {

SynchroIntegrator::SynchroIntegrator(
    const std::shared_ptr<magneticfields::MagneticField> mfield_,
    const std::shared_ptr<cosmicrays::CosmicRayDensity> crdensity_)
    : RadioIntegratorTemplate(), mfield(mfield_), crdensity(crdensity_) {}

SynchroIntegrator::~SynchroIntegrator() {}

void SynchroIntegrator::setFrequency(const QFrequency &freq) {
	setSkymapParameter(freq);
}

QFrequency SynchroIntegrator::getFrequency() const { return skymapParameter; }

QTemperature SynchroIntegrator::integrateOverLOS(QDirection direction) const {
	return integrateOverLOS(direction, skymapParameter);
}

QTemperature SynchroIntegrator::integrateOverLOS(QDirection direction,
						 QFrequency freq_) const {

	auto integrand = [this, direction, freq_](const QLength &dist) {
		return this->integrateOverEnergy(
		    getGalacticPosition(this->positionSun, dist, direction),
		    freq_);
	};

	QIntensity total_intensity =
	    simpsonIntegration<QIntensity, QEmissivity>(
		[integrand](QLength dist) { return integrand(dist); }, 0,
		getMaxDistance(direction), 100);

	return intensityToTemperature(total_intensity / 4_pi, freq_);
}

QEnergy SynchroIntegrator::singleElectronEmission(QFrequency freq_, QEnergy E_,
						  QMField B_perp_) const {

	// TODO: non-relativistic factor (c/v) (see Longair eq. 8.55)
	QFrequency freq_giro = e_plus * B_perp_ / m_electron;
	QFrequency freq_c = 3. / 2. * pow<2>(getLorentzFactor(m_electron, E_)) *
			    freq_giro / 2_pi;
	double ratio = static_cast<double>(freq_ / freq_c);

	// speed-up by skipping negligible contributions
	// F(100) ~ 4e-43
	// https://www.gnu.org/software/gsl/manual/html_node/Synchrotron-Functions.html
	// http://iopscience.iop.org/article/10.1088/1674-4527/13/6/007#
	if (ratio > 100)
		return QEnergy(0);

	return const_synchro * B_perp_ * gsl_sf_synchrotron_1(ratio);
}

QEmissivity SynchroIntegrator::integrateOverEnergy(Vector3QLength pos_,
						   QFrequency freq_) const {
	if (crdensity->existsScaleFactor())
		return integrateOverLogEnergy(pos_, freq_);
	else
		return integrateOverSumEnergy(pos_, freq_);
}

QEmissivity SynchroIntegrator::integrateOverSumEnergy(Vector3QLength pos_,
						      QFrequency freq_) const {

	QEmissivity emissivity(0);
	QEnergy deltaE;
	Vector3QMField B;
	QMField B_perp;

	B = mfield->getField(pos_);
	// skip B null-vector as it will produce NaN in the next step
	if (B.getR() == 0_muG)
		return emissivity;
	if (pos_ == Vector3QLength(0))
		return emissivity; // skip the origin
	B_perp = B.getR() * sin((B.getValue()).getAngleTo(pos_.getValue()));
	if (B_perp == 0_T)
		return emissivity;

	for (auto itE = std::next(crdensity->begin()); itE != crdensity->end();
	     ++itE) {
		deltaE = (*itE) - *std::prev(itE);
		emissivity += singleElectronEmission(freq_, (*itE), B_perp) *
			      crdensity->getDensityPerEnergy(*itE, pos_) *
			      deltaE;
	}

	return emissivity;
}

QEmissivity SynchroIntegrator::integrateOverLogEnergy(Vector3QLength pos_,
						      QFrequency freq_) const {

	QEmissivity emissivity(0);
	Vector3QMField B;
	QMField B_perp;

	B = mfield->getField(pos_);
	if (B.getR() == 0_muG)
		return emissivity;
	if (pos_ == Vector3QLength(0))
		return emissivity; // skip the origin
	B_perp = B.getR() * sin((B.getValue()).getAngleTo(pos_.getValue()));
	if (B_perp == 0_T)
		return emissivity;

	for (auto itE = crdensity->begin(); itE != crdensity->end(); ++itE) {
		emissivity += singleElectronEmission(freq_, (*itE), B_perp) *
			      crdensity->getDensityPerEnergy(*itE, pos_) *
			      (*itE);
	}

	return emissivity * log(crdensity->getEnergyScaleFactor());
}

} // namespace hermes
