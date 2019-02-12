#include "hermes/Common.h"

#include <iostream>

namespace hermes {

bool isWithinAngle(QDirection a, QDirection b, QAngle d) {
	Vector3d v1, v2;
	v1.setRThetaPhi(1, a[0], a[1]);
	v2.setRThetaPhi(1, b[0], b[1]);
	return (v1.getAngleTo(v2) < d);
}

QLength distanceFromGC(QDirection direction, QLength distanceFromSun,
                                Vector3QLength vecGCSun) {
        Vector3QLength vecSunTarget;
        vecSunTarget.setRThetaPhi(distanceFromSun, direction[0], direction[1]);
        Vector3QLength vecGCTarget = vecSunTarget - vecGCSun;

        return vecGCTarget.getR();
}

QLength distanceToGalBorder(Vector3QLength positionSun, QDirection direction) {

        Vector3QLength positionGC(0, 0, 0);
        QLength galacticBorder = 20_kpc; // for example JF12 is zero for r > 20kpc
        Vector3QLength vecSunToGalBorder;
        vecSunToGalBorder.setRThetaPhi(1_m, direction[0], direction[1]);

        QLength a = (positionSun-positionGC).getR();
        QLength c = galacticBorder;
        QAngle gamma = vecSunToGalBorder.getAngleTo(positionSun-positionGC);

        return (2*a*cos(gamma) + std::sqrt(2)*sqrt(2*c*c - a*a + a*a*cos(2*gamma)))/2.0;
}

QNumber getLorentzFactor(QMass m, QEnergy E) {
        return E / (m * c_squared);
}

} // namespace hermes 
