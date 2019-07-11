#include "hermes/interactions/KleinNishina.h"
#include "hermes/Common.h"

namespace hermes {

KleinNishina::KleinNishina() { }

QDiffCrossSection KleinNishina::getDiffCrossSection(
                const QEnergy &E_proton,
                const QEnergy &E_gamma) const {
	return QDiffCrossSection(0);
}

void KleinNishina::setCacheStorage(std::unique_ptr<CacheStorageKN> cache_) {
	cache = std::move(cache_);	
	cacheStoragePresent = true;
	auto f = [this](QEnergy E_electron, QEnergy E_photon, QEnergy E_gamma) {
			return this->getDiffCrossSection(E_electron, E_photon, E_gamma); };
	cache->setFunction(f);
};

QDiffCrossSection KleinNishina::getDiffCrossSectionFromCache(
		const QEnergy &E_electron,
		const QEnergy &E_photon,
		const QEnergy &E_gamma) const {
	if (cacheStoragePresent)
		return cache->getValue(E_electron, E_photon, E_gamma);
	return getDiffCrossSection(E_electron, E_photon, E_gamma);
}

QDiffCrossSection KleinNishina::getDiffCrossSection(
		const QEnergy &E_electron,
		const QEnergy &E_photon,
		const QEnergy &E_gamma) const {

	static constexpr QLength r_e =
		pow<2>(e_plus)/(4_pi*epsilon0*m_electron*pow<2>(c_light));
	
	QNumber x_in = E_photon / (m_electron*c_squared);
	QNumber x_out = E_gamma / (m_electron*c_squared);
	QNumber electronLorentz = getLorentzFactor(m_electron, E_electron);
	QNumber eLSquared = electronLorentz*electronLorentz;
	QNumber q = (x_out / x_in) / (4*eLSquared) * (QNumber(1) + 4*x_in*electronLorentz);

	if (q < 1.0/(4*eLSquared) || q > QNumber(1))
		return QDiffCrossSection(0);

	return 2_pi*r_e*r_e/(E_photon*eLSquared) * 
		(2*q*log(q) + (QNumber(1)+2*q)*(QNumber(1)-q) +
		(QNumber(1)-q)/2.0 * pow<2>(4*x_in*electronLorentz*q) / 
			(QNumber(1)+4*x_in*electronLorentz*q));
}

} // namespace hermes 
