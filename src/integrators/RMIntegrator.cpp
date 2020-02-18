#include "hermes/integrators/RMIntegrator.h"
#include "hermes/integrators/LOSIntegrationMethods.h"

namespace hermes {

RMIntegrator::RMIntegrator(const std::shared_ptr<MagneticField> mfield,
			   const std::shared_ptr<ChargedGasDensity> gdensity) :
	RMIntegratorTemplate(), mfield(mfield), gdensity(gdensity) {	
}

RMIntegrator::~RMIntegrator() { }

QRotationMeasure RMIntegrator::integrateOverLOS(QDirection direction) const {
	auto integrand = [this, direction](const QLength &dist) {
		return this->integralFunction(getGalacticPosition(getSunPosition(), dist, direction)); };

	return simpsonIntegration<QRotationMeasure, QRMIntegral>(
			[integrand](QLength dist) {return integrand(dist);}, 0, getMaxDistance(direction), 500);
}

QRMIntegral RMIntegrator::integralFunction(Vector3QLength pos) const {
	constexpr auto const_a0 = pow<3>(e_plus)/(8*pi*pi*epsilon0*squared(m_electron)*pow<3>(c_light));
	
	Vector3QMField B = mfield->getField(pos);
	if (B.getR() == 0_muG) return 0;
	// TODO: optimise
	QMField B_parallel = B.getR() * cos((B.getValue()).getAngleTo(pos.getValue()));

	return const_a0 * B_parallel * gdensity->getDensity(pos) * radian;
}

} // namespace hermes 
