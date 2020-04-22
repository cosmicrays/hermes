#ifndef HERMES_NE2001SIMPLE_H
#define HERMES_NE2001SIMPLE_H

#include "hermes/chargedgas/ChargedGasDensity.h"

namespace hermes {
namespace chargedgas {

class NE2001Simple : public ChargedGasDensity {
  private:
    int N_arms;
    QNumber F1, F2;
    QPDensity n1, n2, n_a, n_GC;
    QLength H1, H2, H_a, A1, A2, A_a, R2, R_Sun;
    QLength x_GC, y_GC, z_GC, r_GC, h_GC, H_GC;

    double h(const QNumber &x) const;

  public:
    NE2001Simple();
    QPDensity getDensity(const Vector3QLength &pos) const override;
    QPDensity getThickDiskDensity(const Vector3QLength &pos) const;
    QPDensity getThinDiskDensity(const Vector3QLength &pos) const;
    QPDensity getSpiralArmsDensity(const Vector3QLength &pos) const;
    QPDensity getGalacticCentreDensity(const Vector3QLength &pos) const;
};

} // namespace chargedgas
} // namespace hermes

#endif // HERMES_NE2001SIMPLE_H
