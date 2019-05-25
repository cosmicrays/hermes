#ifndef HERMES_ISRF_H
#define HERMES_ISRF_H

#include "hermes/Units.h"
#include "hermes/Grid.h"

#include <array>

namespace hermes {

class ISRF {
private:
    std::vector<double> logwavelenghts;
    std::vector<double> rs;
    std::vector<double> zs;
    std::vector<double> isrf;
    std::vector<int> r_id = {0, 2, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80,  85,  90,  95, 100, 110, 120, 140, 160, 180, 200, 250, 300};
    std::vector<int> z_id = {0, 1, 2,  3,  4,  5,  6,  8, 10, 12, 15, 20, 25, 30, 40, 50, 60, 80, 100, 120, 150, 200, 250, 300};

    double getISRF(const int& ir, const int& iz, const int& ifreq);
    QEnergyDensity getCMB(QFrequency nu);

    void loadFrequencyAxis();
    void loadSpatialAxis();
    void loadISRF();

public:
    ISRF();
    int getSize();
    QEnergyDensity getEnergyDensity(const QLength &r, const QLength &z, const QEnergy &E_photon);
    
};

} // namespace hermes

#endif // HERMES_ISRF_H
