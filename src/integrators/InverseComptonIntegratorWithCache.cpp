#include "hermes/integrators/InverseComptonIntegratorWithCache.h"
#include "hermes/integrators/LOSIntegrationMethods.h"
#include "hermes/Common.h"

#include <memory>
#include <functional>
#include <stdexcept>
#include <iostream>

namespace hermes {

InverseComptonIntegratorWithCache::InverseComptonIntegratorWithCache(
	const std::shared_ptr<CosmicRayDensity> crdensity_,
	const std::shared_ptr<PhotonField> phdensity_,
	const std::shared_ptr<KleinNishina> crossSec_) : 
	crdensity(crdensity_), phdensity(phdensity_), crossSec(crossSec_) {
	
	cacheStoragePresent = false;
}

void InverseComptonIntegratorWithCache::setCacheStorage(std::unique_ptr<CacheStorageIC> cache_) {
	cache = std::move(cache_);	
	cacheStoragePresent = true;
	constexpr QLength cache_block = 10_pc;
	auto f = [this,cache_block](int rho, int z, QEnergy E_gamma) {
			return this->integrateOverEnergy(Vector3QLength(cache_block*rho, 0, cache_block*z), E_gamma); };
	cache->setFunction(f);
};

InverseComptonIntegratorWithCache::~InverseComptonIntegratorWithCache() { }

QDifferentialIntensity InverseComptonIntegratorWithCache::integrateOverLOS(
		QDirection direction) const {
	return integrateOverLOS(direction, 1_GeV);
}

QDifferentialIntensity InverseComptonIntegratorWithCache::integrateOverLOS(
		QDirection direction_, QEnergy Egamma_) const {
	
	QDifferentialIntensity tolerance = 1e10; // / (1_GeV * 1_cm2 * 1_s); // sr^-1 
	
	auto integrand = [this, direction_, Egamma_](const QLength &dist) {
		return this->axialCacheForIoE(getGalacticPosition(this->positionSun, dist, direction_), Egamma_); };

	return simpsonIntegration<QDifferentialIntensity, QICOuterIntegral>(
			[integrand](QLength dist) {return integrand(dist);}, 0, getMaxDistance(direction_), 500);
}

QICOuterIntegral InverseComptonIntegratorWithCache::axialCacheForIoE(Vector3QLength pos_, QEnergy Egamma_) const {
	if (!cacheStoragePresent)
		throw std::runtime_error("Cache storage is not present.");
	QLength rho = sqrt(pos_.x*pos_.x + pos_.y*pos_.y);
	constexpr QLength cache_block = 10_pc;
	int irho = static_cast<int>(floor(rho / cache_block));
	int iz = static_cast<int>(floor(pos_.z / cache_block));

	return cache->getValue(irho, iz, Egamma_);
}

QICOuterIntegral InverseComptonIntegratorWithCache::integrateOverEnergy(Vector3QLength pos_, QEnergy Egamma_) const {
	if (crdensity->existsScaleFactor())
		return integrateOverLogEnergy(pos_, Egamma_);
	else
		return integrateOverSumEnergy(pos_, Egamma_);
}

QICOuterIntegral InverseComptonIntegratorWithCache::integrateOverSumEnergy(Vector3QLength pos_, QEnergy Egamma_) const {

	QICOuterIntegral integral(0);
	QEnergy deltaE;

	for (auto itE = std::next(crdensity->begin()); itE != crdensity->end(); ++itE) {
		deltaE = (*itE) - *std::prev(itE);
		integral += integrateOverPhotonEnergy(pos_, Egamma_, (*itE)) *
			crdensity->getDensityPerEnergy(*itE, pos_) * c_light / 4_pi * deltaE;
        }
	
	return integral;
}

QICOuterIntegral InverseComptonIntegratorWithCache::integrateOverLogEnergy(Vector3QLength pos_, QEnergy Egamma_) const {

	QICOuterIntegral integral(0);

	for (auto itE = crdensity->begin(); itE != crdensity->end(); ++itE) {
		integral += integrateOverPhotonEnergy(pos_, Egamma_, (*itE)) * 
	                        crdensity->getDensityPerEnergy((*itE), pos_) * (*itE) * c_light / 4_pi;
        }

	return integral * log(crdensity->getEnergyScaleFactor());
}

QICInnerIntegral InverseComptonIntegratorWithCache::integrateOverPhotonEnergy(
		Vector3QLength pos_, QEnergy Egamma_, QEnergy Eelectron_) const {
	
	QICInnerIntegral integral(0);
	
	for (auto itE = phdensity->begin(); itE != phdensity->end(); ++itE) {
		integral += crossSec->getDiffCrossSection(Eelectron_, static_cast<double>(*itE), Egamma_) *
				phdensity->getEnergyDensity(pos_, static_cast<int>(itE - phdensity->begin())) / (*itE);
	}

	return integral * log(phdensity->getEnergyScaleFactor());
}

} // namespace hermes 
