#include "hermes/chargedgas/HII_Cordes91.h"

namespace hermes {
namespace chargedgas {

HII_Cordes91::HII_Cordes91() {
	setTemperature(1e4_K);

	fne1 = 0.025 / 1_cm3;
	H1 = 1.00_kpc;
	A1 = 20.0_kpc;
	fne2 = 0.200 / 1_cm3;
	H2 = 0.15_kpc;
	A2 = 2.0_kpc;
	R2 = 4.0_kpc;
}

QPDensity HII_Cordes91::getDensity(const Vector3QLength &pos) const {
	QLength r = pos.getR();
	QPDensity ne1 = fne1 * exp(-fabs(pos.z) / H1) * exp(-pow<2>(r / A1));
	QPDensity ne2 =
	    fne2 * exp(-fabs(pos.z) / H2) * exp(-pow<2>((r - R2) / A2));
	return ne1 + ne2;
}

} // namespace chargedgas
} // namespace hermes
