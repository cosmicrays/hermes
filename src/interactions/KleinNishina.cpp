#include "hermes/interactions/KleinNishina.h"
#include "hermes/Common.h"

namespace hermes {
namespace interactions {

KleinNishina::KleinNishina() {}

QDiffCrossSection
KleinNishina::getDiffCrossSection(const QEnergy &E_electron,
				  const QEnergy &E_photon,
				  const QEnergy &E_gamma) const {

    constexpr QEnergy mc2_el = m_electron * c_squared;

    QNumber gamma_el = E_electron / mc2_el; // Lorentz_electron
    QNumber gamma_el_2 = gamma_el * gamma_el;
    QNumber p = 4 * E_photon * E_electron / (mc2_el * mc2_el);
    QNumber q = (E_gamma / E_photon) / (4 * gamma_el_2) /
		(1_num - E_gamma / E_electron);

    if (q < 1.0 / (4 * gamma_el_2) || q > 1_num)
	return QDiffCrossSection(0);

    // gamma_el >> 1 is requred for the approximation to work,
    // i.e., a relativistic electron, see (eq. 2.48) in Blumenthal et. al.,
    // 1970
    return 3. / 4. * sigma_Thompson / (E_photon * gamma_el_2) *
	   (2 * q * log(q) + (1_num + 2 * q) * (1_num - q) +
	    0.5 * (p * q) * (p * q) * (1_num - q) / (1_num + p * q));
}

} // namespace interactions
} // namespace hermes
