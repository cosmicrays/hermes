#include "hermes/cosmicrays/DummyCR.h"

namespace hermes { namespace cosmicrays {

DummyCR::DummyCR()
    : CosmicRayDensity(Proton), minE(1_GeV), maxE(10_TeV), steps(20) {
	makeEnergyRange();
}

DummyCR::DummyCR(const PID &pid_)
    : CosmicRayDensity(pid_), minE(1_GeV), maxE(10_TeV), steps(20) {
	makeEnergyRange();
}

DummyCR::DummyCR(const PID &pid_, const QEnergy &minE_,
                               const QEnergy &maxE_, int steps_)
    : CosmicRayDensity(pid_), minE(minE_), maxE(maxE_), steps(steps_) {
	makeEnergyRange();
}

void DummyCR::makeEnergyRange() {
	QEnergy energy = minE;
	double energyRatio =
	    exp(1. / static_cast<double>(steps - 1) * log(maxE / minE));

	for (int i = 0; i < steps; ++i) {
		energyRange.push_back(energy);
		energy = energy * energyRatio;
	}
}

QPDensityPerEnergy DummyCR::getDensityPerEnergy(
    const QEnergy &E_, const Vector3QLength &pos_) const {
	return QPDensityPerEnergy(1);
}

}}  // namespace hermes::cosmicrays
