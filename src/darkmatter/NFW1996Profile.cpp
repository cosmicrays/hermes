#include "hermes/darkmatter/NFW1996Profile.h"

#include <cmath>
#include <gsl/gsl_integration.h>

namespace hermes { namespace darkmatter {

static const double f_NFW(double x, double gamma) {
	return 1. / std::pow(x, gamma) / std::pow(1 + x, 3 - gamma);
}

NFW1996Profile::NFW1996Profile(double gamma, double concentration, QMass M_200) :
		gamma(gamma), concentration(concentration), M_200(M_200) {
	init();
}

double I_func(double x, void * params) {
	double gamma = *(double *) params;
	return x * x * f_NFW(x, gamma);
}

double I(double c, double gamma) {
	gsl_integration_workspace * w = gsl_integration_workspace_alloc(1000);
	double result, error;
	gsl_function F;
	F.function = &I_func;
	F.params = &gamma;
	gsl_integration_qags(&F, 0, c, 0, 1e-7, 1000, w, &result, &error);
	gsl_integration_workspace_free(w);

	return result;
}

void NFW1996Profile::init() {
	//TODO(adundovi): potentially improve the units system to handle the 1/3 power
	R_200 = pow<1,3>(3. * M_200 / 4. / M_PI / 200. / rho_cr); // definition of virial radius at 200 kpc
	r_s = R_200 / ((2. - gamma) * concentration);
	rho_s = 200. * rho_cr * std::pow(concentration, 3) / 3. / I(concentration, gamma);
}

QMassDensity NFW1996Profile::getMassDensity(QLength r) const {
	auto x = r / r_s;
	return rho_s * f_NFW(static_cast<double>(x), static_cast<double>(gamma));
}

/** @}*/
}}  // namespace hermes::darkmatter
