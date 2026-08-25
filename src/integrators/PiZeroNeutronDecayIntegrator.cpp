#include "hermes/integrators/PiZeroNeutronDecayIntegrator.h"

#include <cmath>
#include <stdexcept>

namespace hermes {

PiZeroNeutronDecayIntegrator::PiZeroNeutronDecayIntegrator(
    const std::shared_ptr<cosmicrays::CosmicRayDensity> &crDensity_,
    const std::shared_ptr<neutralgas::RingModel> &ngdensity_,
    const std::shared_ptr<interactions::DifferentialCrossSection> &crossSec_)
    : PiZeroIntegrator(crDensity_, ngdensity_, crossSec_) {
	setDescription("PiZeroNeutronDecayIntegrator");
}

PiZeroNeutronDecayIntegrator::PiZeroNeutronDecayIntegrator(
    const std::vector<std::shared_ptr<cosmicrays::CosmicRayDensity>> &crList_,
    const std::shared_ptr<neutralgas::RingModel> &ngdensity_,
    const std::shared_ptr<interactions::DifferentialCrossSection> &crossSec_)
    : PiZeroIntegrator(crList_, ngdensity_, crossSec_) {
	setDescription("PiZeroNeutronDecayIntegrator");
}

PiZeroNeutronDecayIntegrator::~PiZeroNeutronDecayIntegrator() { setDescription("PiZeroNeutronDecayIntegrator"); }

QDiffIntensity PiZeroNeutronDecayIntegrator::integrateOverLOS(const QDirection &direction) const {
	return integrateOverLOS(direction, 1_GeV);
}

QDiffIntensity PiZeroNeutronDecayIntegrator::integrateOverLOS(const QDirection &direction_,
                                                              const QEnergy &Eneutron_) const {
	const auto K = decayInverseLength(Eneutron_);
	return integrateOverLOSWithAttenuation(direction_, Eneutron_, K);
}

QInverseLength PiZeroNeutronDecayIntegrator::decayInverseLength(const QEnergy &Eneutron_) const {
	const double energyValue = static_cast<double>(Eneutron_ / 1_GeV);
	if (!std::isfinite(energyValue) || Eneutron_ <= QEnergy(0))
		throw std::invalid_argument("PiZeroNeutronDecayIntegrator: neutron energy must be positive and finite");

	const QTime decayTime = 878.4 * 1_s;
	const QEnergy neutronRestMassEnergy = m_neutron * c_squared;
	const auto gamma = Eneutron_ / neutronRestMassEnergy;
	const QInverseLength result = 1. / gamma / decayTime / c_light;
	if (!std::isfinite(static_cast<double>(result)) || result <= QInverseLength(0))
		throw std::runtime_error("PiZeroNeutronDecayIntegrator: invalid neutron decay length");
	return result;
}

}  // namespace hermes
