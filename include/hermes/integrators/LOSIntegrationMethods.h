#ifndef HERMES_LOSINTEGRATIONMETHODS_H
#define HERMES_LOSINTEGRATIONMETHODS_H

#include "hermes/Units.h"
#include "hermes/Grid.h"
#include "hermes/Common.h"

#include <fstream>
#include <sstream>
#include <functional>

namespace hermes {

// dim(QPXL) = dim(INTTYPE) * dim(L)
template <typename QPXL, typename INTTYPE>
QPXL sumIntegration(const QDirection &dir,
		std::function<INTTYPE(Vector3QLength)> f, int N = 100) {
	Vector3QLength positionSun(8.5_kpc, 0, 0);
	// distance from the (spherical) galactic border in the given direction
	QLength maxDistance = distanceToGalBorder(positionSun, dir);
	QLength delta_d = maxDistance/N;

	QPXL total(0);
	Vector3QLength pos(0.0);
	for(QLength dist = 0; dist <= maxDistance; dist += delta_d) {
		pos = getGalacticPosition(positionSun, dist, dir);
		total += f(pos) * delta_d;
	}
	return total;
}

template <typename QPXL, typename INTTYPE, typename QSTEP>
QPXL sumIntegration(const QDirection &dir,
		std::function<INTTYPE(Vector3QLength, QSTEP)> f, const QSTEP &step, int N = 300) {
	Vector3QLength positionSun(8.5_kpc, 0, 0);
	// distance from the (spherical) galactic border in the given direction
	QLength maxDistance = distanceToGalBorder(positionSun, dir);
	QLength delta_d = maxDistance/N;

	QPXL total(0);
	QPXL tmp(0);
	Vector3QLength pos(0.0);

	std::ostringstream filename;
	filename << "integral_profiles/integral-"
		 << "b" << dir[0].getValue()
		 << "-"
		 << "l" << dir[1].getValue()
		 << ".txt";

	std::ofstream integraldump(filename.str());

	for(QLength dist = 0; dist <= maxDistance; dist += delta_d) {
		pos = getGalacticPosition(positionSun, dist, dir);
		tmp = f(pos, step) * delta_d;
		integraldump << dist << "\t" << tmp << "\n";
		total += tmp;
	}

	integraldump.close();

	return total;
}

} // namespace hermes

#endif // HERMES_LOSINTEGRATIONMETHODS_H
