#include "hermes/cosmicrays/DummyCRDensity.h"

namespace hermes { namespace cosmicrays {

DummyCRDensity::DummyCRDensity() :
	CosmicRayDensity(Proton), minE(1_GeV), maxE(10_TeV), steps(20) {
	makeEnergyRange();
}

DummyCRDensity::DummyCRDensity(const PID &pid_) :
	CosmicRayDensity(pid_), minE(1_GeV), maxE(10_TeV), steps(20) {
	makeEnergyRange();
}

DummyCRDensity::DummyCRDensity(const PID &pid_, const QEnergy &minE_, const QEnergy &maxE_, int steps_) :
	CosmicRayDensity(pid_), minE(minE_), maxE(maxE_), steps(steps_) {
	makeEnergyRange();
}

void DummyCRDensity::makeEnergyRange() {
	QEnergy energy = minE;
	double energyRatio =
		exp(1./static_cast<double>(steps-1) * log(maxE/minE));

	for (int i = 0; i < steps; ++i) {
		energyRange.push_back(energy);
		energy = energy * energyRatio;
	}
}

QPDensityPerEnergy DummyCRDensity::getDensityPerEnergy(const QEnergy& E_, const Vector3QLength& pos_) const {
	return QPDensityPerEnergy(1);
}

} // namespace cosmicrays
} // namespace hermes
