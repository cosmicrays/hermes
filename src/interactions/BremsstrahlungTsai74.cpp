#include "hermes/interactions/BremsstrahlungTsai74.h"

#include "hermes/Common.h"

namespace hermes { namespace interactions {

BremsstrahlungTsai74::BremsstrahlungTsai74()
    : BremsstrahlungAbstract() {}

std::pair<double, double> BremsstrahlungTsai74::RadiationLogarithms(int Z) const {
    // elastic and inelatic radiation logarithms for light elements (where the
    // Thomas-Fermi model doesn't work): computed by using Dirac-Fock model of atom.
    const double gFelLowZet[] = {0.0, 5.3104, 4.7935, 4.7402, 4.7112, 4.6694, 4.6134, 4.5520};
    const double gFinelLowZet[] = {0.0, 5.9173, 5.6125, 5.5377, 5.4728, 5.4174, 5.3688, 5.3236};

    double Fel, Finel;
    if (Z < 5) {
        Fel = gFelLowZet[Z];
        Finel = gFinelLowZet[Z];
    } else {
        const double LogZ = std::log(static_cast<double>(Z));
        Fel = std::log(184.15) - LogZ / 3.;
        Finel = std::log(1194) - 2. * LogZ / 3.;
    }
    return {Fel, Finel};
}

double BremsstrahlungTsai74::computeCoulombCorrection(int Z, std::size_t n_max) const {
    double value = 0;
    for (size_t n = 1; n < n_max; ++n)
        value += 1. / n / (n * n + static_cast<double>(alpha_fine * alpha_fine) * static_cast<double>(Z * Z));
    return value;
}

QDiffCrossSection BremsstrahlungTsai74::getDiffCrossSectionForTarget(Target t, const QEnergy &T_electron, const QEnergy &E_gamma) const {

	if (E_gamma >= T_electron)
		return QDiffCrossSection(0);
	
	int Z, N;
	if (t == Target::HII) {
		Z = 1;
		N = 0;
	}
	if (t == Target::HI) {
		Z = 1;
		N = 1;
	}
	if (t == Target::He) {
		Z = 2;
		N = 2;
	}

    QDiffCrossSection xsecs = 4. * alpha_fine * pow<2>(r_electron) / 3. / E_gamma;

    QEnergy E_electron = T_electron + m_electron*c_squared;
    double y = static_cast<double>(E_gamma / E_electron);
    double f = computeCoulombCorrection(Z);
    std::pair<double, double> radLogs = RadiationLogarithms(Z);
    double F_el = radLogs.first;
    double F_inel = radLogs.second;

    const auto X1 = (4. - 4. * y + y * y / 3.) * (Z * Z * (F_el - f) + Z * F_inel);
    const auto X2 = (1. - y) * (Z * Z + Z) / 3.;

    return (X1 + X2) * xsecs;
}

}}  // namespace hermes::interactions
