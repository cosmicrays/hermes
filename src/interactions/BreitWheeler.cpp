#include "hermes/interactions/BreitWheeler.h"
#include "hermes/Common.h"

#include <gsl/gsl_integration.h>
#define GSL_LIMIT 1000

namespace hermes {
namespace interactions {

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

double BreitWheeler::integrateOverTheta(const QEnergy &Egamma, const QEnergy &Eph) const {
    
    QNumber chi = Egamma * Eph / (2 * pow<2>(m_electron * c_squared));

    double a = static_cast<double>(acos(1-1/static_cast<double>(chi)));
    double b = static_cast<double>(2_pi-acos(-1/static_cast<double>(chi)));
    double abs_error = 0.0; // disabled
    double rel_error = 1.0e-3;
    int key = GSL_INTEG_GAUSS21; // GSL_INTEG_GAUSS15;
    double result = 0;
    double error = 0;
/*
	auto f = [chi](QAngle theta) -> double {
		QNumber costheta = cos(theta);
		QNumber x = chi * (1_num - costheta);
        QNumber beta = sqrt(1_num - 1 / x);
		return static_cast<double>((1_num - costheta) * 3. / 16. * sigma_Thompson * (1_num - beta * beta) *
       (2 * beta * (beta * beta - 2_num) +
        (3_num - pow<4>(beta)) * log((1_num + beta) / (1_num - beta))));
	};

    gsl_function F = {.function = [](double x, void *vf) -> double {
			  auto &func =
			      *static_cast<std::function<double(double)> *>(vf);
			  return func(x);
		      },
		      .params = &f};

    gsl_integration_workspace *workspace_ptr =
	gsl_integration_workspace_alloc(GSL_LIMIT);
    gsl_integration_qag(&F, a, b, abs_error, rel_error, N, key, workspace_ptr,
			&result, &error);
    gsl_integration_workspace_free(workspace_ptr);
*/
    return result;
}
/*
QArea BreitWheeler::getCrossSectionIsotropic(const QEnergy &Egamma,
											 const QEnergy &Eph) const {

	return 
}*/

} // namespace interactions
} // namespace hermes
