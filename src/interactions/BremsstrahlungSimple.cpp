#include "hermes/interactions/BremsstrahlungSimple.h"
#include "hermes/Common.h"

#include <functional>

#include <gsl/gsl_math.h>
#include <gsl/gsl_integration.h>

// Following units from Koch and Motz, 1959
#define mc_units	(m_electron*c_light)
#define mc2_units 	(m_electron*c_squared)

#define LIMIT 1000
#define EPSINT 1e-5
#define KEYINT 3

namespace hermes {

/* Template wrapper to expose lambda with capture to gsl_function
 * according to https://stackoverflow.com/a/18413206/6819103 */
template<typename F> 
class gsl_function_pp : public gsl_function {
private:
	const F& _func;
	static double invoke(double x, void *params) {
		return static_cast<gsl_function_pp*>(params)->_func(x);
	}
public:
	gsl_function_pp(const F& func) : _func(func) {
		function = &gsl_function_pp::invoke;
		params=this;
	}
};

BremsstrahlungSimple::BremsstrahlungSimple() :
	cachingEnabled(true), cache(std::make_unique<CacheStorageCrossSection>()) {
	
	auto f = [this](QEnergy E_proton, QEnergy E_gamma) {
			return this->getDiffCrossSectionDirectly(E_proton, E_gamma); };
	cache->setFunction(f);
}

void BremsstrahlungSimple::enableCaching() {
	cachingEnabled = true;
};

void BremsstrahlungSimple::disableCaching() {
	cachingEnabled = false;
};

QDifferentialCrossSection BremsstrahlungSimple::getDiffCrossSection(
		const QEnergy &E_proton,
		const QEnergy &E_gamma) const {
	if (cachingEnabled)
		return cache->getValue(E_proton, E_gamma);
	return getDiffCrossSectionDirectly(E_proton, E_gamma);
}

QDifferentialCrossSection BremsstrahlungSimple::getDiffCrossSection(
			const QEnergy &E_electron,
			const QEnergy &E_photon,
			const QEnergy &E_gamma) const {
	return QDifferentialCrossSection(0);
}

QNumber BremsstrahlungSimple::ElwertFactor(const QNumber &beta_i, const QNumber &beta_f, int Z) const {
	return beta_i * (1. - exp(-2.0_pi * Z * alpha_fine / beta_i)) / 
		beta_f * (1. - exp(-2.0_pi * Z * alpha_fine / beta_f));
}

QNumber BremsstrahlungSimple::xiFunc(const QNumber &T_electron_i, const QNumber &k, int Z, int N) const {
	constexpr QNumber b = 0.07_MeV / mc2_units;
	constexpr QNumber c = 0.33_MeV / mc2_units;
	return 1. + N / pow<2>(Z) * (1. - exp((b - T_electron_i)/(9.*b))) * (1. - 0.3 * exp(-k / c));
}
	
QNumber BremsstrahlungSimple::Phi_u(const QNumber &gamma_i, const QNumber &gamma_f, const QNumber &k) const {
	return 4. * (log(2. * gamma_i * gamma_f / k) - 0.5);
}


inline double BremsstrahlungSimple::R_1(double q, double Z) const {
	double F_1 = 1. / pow<2>(1. + pow<2>(q) / pow<2>(2. * static_cast<double>(alpha_fine) * Z));
	return 1 - F_1;
}

inline double BremsstrahlungSimple::R_2(double q, double Z) const {
	double F_2 = 1. / pow<2>(1. + pow<2>(q) / pow<2>(2. * static_cast<double>(alpha_fine) * (Z - 5. / 16.)));
	return 2. * (1. - F_2) - (1. - pow<2>(F_2)) / Z;
}

QNumber BremsstrahlungSimple::I_Phi_1(const QNumber &delta_, int Z, int N) const {
	double result, error;
	double delta = static_cast<double>(delta_);

	auto R_N = [this, delta, Z, N](double q){
		return ((N == 1) ? R_1(q, Z) : R_2(q, Z)) / pow<3>(q) * pow<2>(q - delta); };

	gsl_function_pp<decltype(R_N)> Fp(R_N);
	gsl_function *F = static_cast<gsl_function*>(&Fp);

	gsl_integration_workspace * w = gsl_integration_workspace_alloc(LIMIT);
	gsl_integration_qags(F, delta, 1, 0, EPSINT, LIMIT, w, &result, &error);
	gsl_integration_workspace_free(w);

	return result;
}

QNumber BremsstrahlungSimple::I_Phi_2(const QNumber &delta_, int Z, int N) const {
	double result, error;
	double delta = static_cast<double>(delta_);

	auto R_N = [this, delta, Z, N](double q){
		return ((N == 1) ? R_1(q, Z) : R_2(q, Z)) / pow<4>(q) *
			(pow<3>(q) - 6. * pow<2>(delta) * q * std::log(q / delta) + 
			 3. * pow<2>(delta) * q - 4. * pow<3>(delta)); };

	gsl_function_pp<decltype(R_N)> Fp(R_N);
	gsl_function *F = static_cast<gsl_function*>(&Fp);

	gsl_integration_workspace * w = gsl_integration_workspace_alloc(LIMIT);
	gsl_integration_qags(F, delta, 1, 0, EPSINT, LIMIT, w, &result, &error);
	gsl_integration_workspace_free(w);

	return result;
}

QNumber BremsstrahlungSimple::Phi_1(const QNumber &gamma_i, const QNumber &gamma_f,
		const QNumber &k, const QNumber &delta, int Z, int N) const {
	QNumber I = I_Phi_1(delta, Z, N);
	return pow<2>(Z - N) * Phi_u(gamma_i, gamma_f, k) +
		8.0 * Z * (1.0_num - QNumber((N - 1.) / Z) + I);
}

QNumber BremsstrahlungSimple::Phi_2(const QNumber &gamma_i, const QNumber &gamma_f,
		const QNumber &k, const QNumber &delta, int Z, int N) const {
	QNumber I = I_Phi_2(delta, Z, N);
	return pow<2>(Z - N) * Phi_u(gamma_i, gamma_f, k) +
		8.0 * Z * (5.0_num / 6. * (1. - (N - 1.) / Z) + I);
}

QArea BremsstrahlungSimple::dsdk_LowEnergy(const QNumber &p_i, const QNumber &p_f, QNumber k, int Z) const {
	return 16.*pow<2>(Z * r_electron) * alpha_fine / 
		(3.* k * pow<2>(p_i)) * log((p_i + p_f) / (p_i - p_f));
}

QArea BremsstrahlungSimple::dsdk_IntermediateEnergy(const QNumber &gamma_i, const QNumber &gamma_f,
		const QNumber &p_i, const QNumber &p_f, const QNumber &k, int Z) const {
        
	QNumber L = 2. * log((gamma_i * gamma_f + p_i * p_f - 1_num) / k);
	QNumber epsilon_i = log((gamma_i + p_i) / (gamma_i - p_i));
        QNumber epsilon_f = log((gamma_f + p_f) / (gamma_f - p_f));
        
        QNumber ininner_factor = epsilon_i * (gamma_i * gamma_f + pow<2>(p_i)) / pow<3>(p_i);
        ininner_factor -= epsilon_f * (gamma_i * gamma_f + pow<2>(p_f)) / pow<3>(p_f);
        ininner_factor += 2. * k * gamma_i * gamma_f / pow<2>(p_i * p_f);
	
	QNumber inner_factor = 8. / 3. * gamma_i * gamma_f / p_i / p_f;
        inner_factor += pow<2>(k) * (pow<2>(gamma_i * gamma_f) + pow<2>(p_i * p_f)) / pow<3>(p_i * p_f);
        inner_factor += k / 2. / p_i / p_f * ininner_factor;
        
	QNumber factor = 4. / 3.;
        factor -= 2. * gamma_i * gamma_f * (pow<2>(p_f) + pow<2>(p_i)) / pow<2>(p_f * p_i);
        factor += epsilon_i * gamma_f / pow<3>(p_i) + epsilon_f * gamma_i / pow<3>(p_f);
        factor -= epsilon_i * epsilon_f / p_i / p_f;
        factor += L * inner_factor;
        
        return pow<2>(Z * r_electron) * alpha_fine * (p_f / p_i) / k * factor;
}
    
QArea BremsstrahlungSimple::dsdk_HighEnergy(const QNumber &gamma_i, const QNumber &gamma_f, const QNumber &k, int Z, int N) const {
	QNumber delta = k / 2. / gamma_i / gamma_f;
       	
       	QNumber phi_1, phi_2;	
	if (N == 0) {
		phi_1 = pow<2>(Z) * Phi_u(gamma_i, gamma_f, k);
	        phi_2 = pow<2>(Z) * Phi_u(gamma_i, gamma_f, k);
	} else {
		phi_1 = Phi_1(gamma_i, gamma_f, k, delta, Z, N);
	        phi_2 = Phi_2(gamma_i, gamma_f, k, delta, Z, N);
	}
	
	QNumber factor = (1_num + pow<2>(gamma_f / gamma_i)) * phi_1 -
		2. / 3. * gamma_f / gamma_i * phi_2;
        return pow<2>(r_electron) * alpha_fine / k * factor;
}

QDifferentialCrossSection BremsstrahlungSimple::getDiffCrossSectionDirectly(
		const QEnergy &E_electron,
		const QEnergy &E_gamma) const {
	int Z = 1, N = 1;

	QNumber k = E_gamma / mc2_units;
	QNumber E_electron_i = E_electron / mc2_units;
	QNumber E_electron_f = E_electron_i - k;

	QNumber T_electron_i = E_electron_i - 1_num;
	QNumber T_electron_f = E_electron_f - 1_num;
	
	if (T_electron_i <= k)
		return QDifferentialCrossSection(0);

	if (T_electron_i < 0.01_MeV / mc2_units)
		return QDifferentialCrossSection(0);

	QNumber p_i = sqrt(T_electron_i * (T_electron_i + 2_num));
        QNumber p_f = sqrt(T_electron_f * (T_electron_f + 2_num));

        QNumber gamma_i = E_electron_i;
        QNumber gamma_f = E_electron_f;

	QNumber beta_i = sqrt(1_num - 1./(gamma_i*gamma_i));
        QNumber beta_f = sqrt(1_num - 1./(gamma_f*gamma_f));
	
	if (T_electron_i < 0.07_MeV / mc2_units)
        	return ElwertFactor(beta_i, beta_f, Z) * dsdk_LowEnergy(p_i, p_f, k, Z) / E_gamma;
	
	if (T_electron_i < 2.0_MeV / mc2_units)
                return ElwertFactor(beta_i, beta_f, Z) * xiFunc(T_electron_i, k, Z, N) 
	   		* dsdk_IntermediateEnergy(gamma_i, gamma_f, p_i, p_f, k, Z) / E_gamma;

	return dsdk_HighEnergy(gamma_i, gamma_f, k, Z, N) / E_gamma;
}

} // namespace hermes 
