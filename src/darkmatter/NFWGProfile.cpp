#include "hermes/darkmatter/NFWGProfile.h"

#include <gsl/gsl_errno.h>
#include <gsl/gsl_integration.h>

#include <cmath>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>

namespace hermes { namespace darkmatter {

namespace {

constexpr int integrationLimit = 1000;
std::mutex gslErrorHandlerMutex;

class ScopedGslErrorHandlerOff {
  public:
	ScopedGslErrorHandlerOff() : oldHandler(gsl_set_error_handler_off()) {}
	~ScopedGslErrorHandlerOff() { gsl_set_error_handler(oldHandler); }

  private:
	gsl_error_handler_t *oldHandler;
};

double f_NFW(double x, double gamma) {
	return 1. / std::pow(x, gamma) / std::pow(1 + x, 3 - gamma);
}

void validateParameters(double gamma, double concentration, QMass M_200) {
	if (!std::isfinite(gamma) || gamma < 0. || gamma >= 2.)
		throw std::invalid_argument(
		    "hermes::NFWGProfile: gamma must satisfy 0 <= gamma < 2");
	if (!std::isfinite(concentration) || concentration <= 0.)
		throw std::invalid_argument(
		    "hermes::NFWGProfile: concentration must be positive");
	const double mass = static_cast<double>(M_200 / 1_kg);
	if (!std::isfinite(mass) || M_200 <= QMass(0))
		throw std::invalid_argument(
		    "hermes::NFWGProfile: M_200 must be positive");
}

}  // namespace

NFWGProfile::NFWGProfile(double gamma, double concentration, QMass M_200)
    : gamma(gamma), concentration(concentration), M_200(M_200) {
	validateParameters(gamma, concentration, M_200);
	init();
}

double I_func(double x, void *params) {
	double gamma = *(double *)params;
	return x * x * f_NFW(x, gamma);
}

double I(double c, double gamma) {
	std::unique_ptr<gsl_integration_workspace,
	                decltype(&gsl_integration_workspace_free)>
	    w(gsl_integration_workspace_alloc(integrationLimit),
	      gsl_integration_workspace_free);
	if (!w)
		throw std::runtime_error(
		    "hermes::NFWGProfile: could not allocate GSL workspace");

	double result = 0.;
	double error = 0.;
	gsl_function F;
	F.function = &I_func;
	F.params = &gamma;

	int status = GSL_SUCCESS;
	{
		std::lock_guard<std::mutex> lock(gslErrorHandlerMutex);
		ScopedGslErrorHandlerOff disableGslAbort;
		status = gsl_integration_qags(&F, 0, c, 0, 1e-7, integrationLimit,
		                              w.get(), &result, &error);
	}
	if (status != GSL_SUCCESS)
		throw std::runtime_error("hermes::NFWGProfile: GSL integration failed: " +
		                         std::string(gsl_strerror(status)));
	if (!std::isfinite(result) || result <= 0.)
		throw std::runtime_error(
		    "hermes::NFWGProfile: invalid normalization integral");

	return result;
}

void NFWGProfile::init() {
	// TODO(adundovi): potentially improve the units system to handle the 1/3
	// power
	R_200 =
	    std::pow(static_cast<double>(3. * M_200 / 4. / M_PI / 200. / rho_cr),
	             1 / 3.);  // definition of virial radius at 200 kpc
	r_s = R_200 / ((2. - gamma) * concentration);
	rho_s = 200. * rho_cr * std::pow(concentration, 3) / 3. /
	        I(concentration, gamma);
}

QLength NFWGProfile::getScaleRadius() const { return r_s; }

QLength NFWGProfile::getVirialRadius() const { return R_200; }

QMassDensity NFWGProfile::getScaleDensity() const { return rho_s; }

QMassDensity NFWGProfile::getRhoSun() const { return getScaleDensity(); }

QMassDensity NFWGProfile::getMassDensity(QLength r) const {
	auto x = r / r_s;
	return rho_s * f_NFW(static_cast<double>(x), static_cast<double>(gamma));
}

}}  // namespace hermes::darkmatter
