#include "hermes/integrators/RotationMeasureIntegrator.h"
#include "hermes/integrators/LOSIntegrationMethods.h"

namespace hermes {

RotationMeasureIntegrator::RotationMeasureIntegrator(
    const std::shared_ptr<magneticfields::MagneticField> mfield,
    const std::shared_ptr<chargedgas::ChargedGasDensity> gdensity)
    : RotationMeasureIntegratorTemplate(), mfield(mfield), gdensity(gdensity) {}

RotationMeasureIntegrator::~RotationMeasureIntegrator() {}

QRotationMeasure
RotationMeasureIntegrator::integrateOverLOS(QDirection direction) const {
	auto integrand = [this, direction](const QLength &dist) {
		return this->integralFunction(
		    getGalacticPosition(getSunPosition(), dist, direction));
	};

	return simpsonIntegration<QRotationMeasure, QRMIntegral>(
	    [integrand](QLength dist) { return integrand(dist); }, 0,
	    getMaxDistance(direction), 500);
}

QRMIntegral
RotationMeasureIntegrator::integralFunction(Vector3QLength pos) const {
	const auto const_a0 =
	    pow<3>(e_plus) /
	    (8 * pi * pi * epsilon0 * squared(m_electron) * pow<3>(c_light));

	Vector3QMField B = mfield->getField(pos);
	if (B.getR() == 0_muG)
		return 0;
	// TODO: optimise
	QMField B_parallel =
	    B.getR() * cos((B.getValue()).getAngleTo(pos.getValue()));

	return const_a0 * B_parallel * gdensity->getDensity(pos) * radian;
}

} // namespace hermes
