#include "hermes/interactions/BreitWheeler.h"

#include <gsl/gsl_integration.h>

#include <functional>

#include "hermes/Common.h"

#define GSL_LIMIT 10000
#define GSL_EPSINT 1e-2
#define GSL_KEYINT 3

namespace hermes { namespace interactions {

BreitWheeler::BreitWheeler() {}

QArea BreitWheeler::getCrossSection(const QEnergy &Egamma, const QEnergy &Eph,
                                    const QAngle &theta) const {
	QNumber costheta = cos(theta);
	QNumber chi = Egamma * Eph / (2 * pow<2>(m_electron * c_squared));
	QNumber x = chi * (1_num - costheta);

	if (x < 1_num) return QArea(0);

	QNumber beta = sqrt(1_num - 1 / x);

	return 3. / 16. * sigma_Thompson * (1_num - beta * beta) *
	       (2 * beta * (beta * beta - 2_num) +
	        (3_num - pow<4>(beta)) * log((1_num + beta) / (1_num - beta)));
}

QArea BreitWheeler::integratedOverTheta(const QEnergy &Egamma,
                                        const QEnergy &Eph) const {
	QNumber chi = Egamma * Eph / (2 * pow<2>(m_electron * c_squared));

	if (chi < 0.5_num) return QArea(0);

	QAngle a = acos(1_num - 1 / chi);
	QAngle b = pi * 2.0_rad - a;
	double abs_error = 0.0;  // disabled
	double rel_error = 1.0e-2;
	int key = GSL_INTEG_GAUSS51;  // GSL_INTEG_GAUSS15;
	double result = 0;
	double error = 0;
	int N = 3000;

	auto integrand = [this, Egamma, Eph](double th) {
		return (1 - std::cos(th)) *
		       static_cast<double>(getCrossSection(Egamma, Eph, QAngle(th)));
	};

	gsl_function_pp<decltype(integrand)> Fp(integrand);
	gsl_function *F = static_cast<gsl_function *>(&Fp);

	gsl_integration_workspace *w = gsl_integration_workspace_alloc(GSL_LIMIT);
	gsl_integration_qag(F, static_cast<double>(a), static_cast<double>(b),
	                    abs_error, rel_error, N, key, w, &result, &error);
	gsl_integration_workspace_free(w);

	return result;
}

}}  // namespace hermes::interactions
