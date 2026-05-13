#include "hermes/interactions/UheYields.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "hermes/interactions/KelnerAharonianGamma.h"
#include "hermes/interactions/UheYieldsGamma.h"
#include "hermes/interactions/UheYieldsNeutrons.h"

namespace hermes { namespace interactions {

namespace {

struct NeutronFitParameters {
	double logA;
	double alpha;
	double beta;
	double logB;
	double mu;
	double logSigma;
};

struct GammaFitParameters {
	double logA;
	double alpha;
	double logXc;
	double logK;
};

NeutronFitParameters neutronFitParameters(UheHadronicModel model) {
	switch (model) {
		case UheHadronicModel::Sibyll23e:
			return {-1.519123325281e+00, 2.548711652707e-01,  5.126624943702e-01,
			        -7.704327877311e-01, -3.470481355404e-01, -1.334972927203e+00};
		case UheHadronicModel::QGSJetII04:
			return {-9.847768002355e-01, -2.089384415902e-01, 1.038481312054e-01,
			        -6.179081253600e-01, -2.857861003677e+00, -2.344404768575e-01};
		case UheHadronicModel::EposLHC:
			return {-1.933908572028e-01, -2.614540343856e+00, 1.178716545609e+00,
			        -5.560421623046e-01, -3.207762027465e+00, 4.350779310769e-01};
	}

	throw std::invalid_argument("unknown neutron UHE hadronic model");
}

GammaFitParameters gammaFitParameters(UheHadronicModel model) {
	switch (model) {
		case UheHadronicModel::Sibyll23e:
			return {-5.457322296752e-01, 4.001202977786e-01, -7.116796573833e-01, 2.192118272122e-01};
		case UheHadronicModel::QGSJetII04:
			return {2.782456687797e-01, 2.402864125498e-01, -1.541730400425e+00, -4.639125025078e-01};
		case UheHadronicModel::EposLHC:
			return {2.854702069756e-01, 2.125197220766e-01, -1.510127472023e+00, -5.492456475264e-01};
	}

	throw std::invalid_argument("unknown gamma UHE hadronic model");
}

double neutronXDNdX(double x, const NeutronFitParameters &p) {
	if (!std::isfinite(x) || x <= 0.0) return 0.0;

	const double A = std::pow(10.0, p.logA);
	const double B = std::pow(10.0, p.logB);
	const double sigma = std::exp(p.logSigma);
	const double oneMinusX = std::max(1.0 - x, 1.0e-12);

	const double continuum = A * std::pow(x, -p.alpha) * std::pow(oneMinusX, p.beta);
	const double z = (std::log10(x) - p.mu) / sigma;
	const double bump = B * std::exp(-0.5 * z * z);

	return continuum + bump;
}

double gammaXDNdX(double x, const GammaFitParameters &p) {
	if (!std::isfinite(x) || x <= 0.0) return 0.0;

	const double A = std::pow(10.0, p.logA);
	const double xc = std::pow(10.0, p.logXc);
	const double k = std::exp(p.logK);

	return A * std::pow(x, -p.alpha) * std::exp(-std::pow(x / xc, k));
}

double neutronXDNdX(double x, UheHadronicModel model) { return neutronXDNdX(x, neutronFitParameters(model)); }

double gammaXDNdX(double x, UheHadronicModel model) { return gammaXDNdX(x, gammaFitParameters(model)); }

}  // namespace

UheYields::UheYields(UheHadronicModel model_) : DifferentialCrossSection(), model(model_) {}

QArea UheYields::sigmaInelastic(const QEnergy &E_proton) { return KelnerAharonianGamma::sigmaInelastic(E_proton); }

QDiffCrossSection UheYields::getDiffCrossSection(const QEnergy &E_proton, const QEnergy &E_secondary) const {
	if (E_proton <= QEnergy(0) || E_secondary <= QEnergy(0)) return QDiffCrossSection(0);
	if (E_secondary > E_proton) return QDiffCrossSection(0);

	constexpr double minX = 1e-3;
	const double x = static_cast<double>(E_secondary / E_proton);
	if (x <= minX || x > 1.) return QDiffCrossSection(0);

	const double xDNdX = getXDNdX(x);
	if (!std::isfinite(xDNdX) || xDNdX <= 0.) return QDiffCrossSection(0);

	return sigmaInelastic(E_proton) * xDNdX / E_secondary;
}

QDiffCrossSection UheYields::getDiffCrossSection(const PID &projectile, const PID &target, const QEnergy &E_proj,
                                                 const QEnergy &E_secondary) const {
	return nuclearScaling(projectile, target) * getDiffCrossSection(E_proj, E_secondary);
}

UheYieldsGamma::UheYieldsGamma(UheHadronicModel model_) : UheYields(model_) {}

double UheYieldsGamma::getXDNdX(double x) const { return gammaXDNdX(x, model); }

UheYieldsNeutrons::UheYieldsNeutrons(UheHadronicModel model_) : UheYields(model_) {}

double UheYieldsNeutrons::getXDNdX(double x) const { return neutronXDNdX(x, model); }

}}  // namespace hermes::interactions
