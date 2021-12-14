#include "hermes/integrators/PiZeroCmbAbsorptionIntegrator.h"

namespace hermes {

auto cmbPhotonField(const QEnergy &eps) {
	constexpr auto K = 1. / (M_PI * M_PI) / (h_planck_bar * h_planck_bar * h_planck_bar * c_light * c_light * c_light);
	constexpr auto cmbTemperature = 2.725_K;
	constexpr auto kT = k_boltzmann * cmbTemperature;
	return K * (eps * eps);  //  / std::expm1(QNumber(eps / kT));
}

QInverseLength PiZeroCmbAbsorptionIntegrator::absorptionCoefficient(const QEnergy &Egamma) const {
	QEnergy epsMin = 1e-5_eV;
	QEnergy epsMax = 5e-3_eV;

	double abs_error = 0.0;  // disabled
	double rel_error = 1.0e-4;
	int key = GSL_INTEG_GAUSS51;  // GSL_INTEG_GAUSS15;
	double result = 0;
	double error = 0;

	// auto integrand = [this, Egamma](QEnergy eps) { return cmbPhotonField(eps) * integratedOverTheta(Egamma, eps); };

	// gsl_function_pp<decltype(integrand)> Fp(integrand);
	// gsl_function *F = static_cast<gsl_function *>(&Fp);

	// gsl_integration_workspace *w = gsl_integration_workspace_alloc(GSL_LIMIT);
	// gsl_integration_qag(F, static_cast<double>(epsMin), static_cast<double>(epsMax), abs_error, rel_error, GSL_LIMIT,
	//                     key, w, &result, &error);
	// gsl_integration_workspace_free(w);

	return result;
}

}  // namespace hermes
