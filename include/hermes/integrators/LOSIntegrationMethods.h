#ifndef HERMES_LOSINTEGRATIONMETHODS_H
#define HERMES_LOSINTEGRATIONMETHODS_H

#include "hermes/Units.h"
#include "hermes/Grid.h"
#include "hermes/Common.h"

#include <fstream>
#include <sstream>
#include <cassert>
#include <functional>

namespace hermes {

// dim(QPXL) = dim(INTTYPE) * dim(L)
template <typename QPXL, typename INTTYPE>
QPXL sumIntegration(const QDirection &dir,
		std::function<INTTYPE(Vector3QLength)> f, int N = 100) {
	
	Vector3QLength positionSun(8.5_kpc, 0, 0);
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

template <typename QPXL, typename INTTYPE>
QPXL simpsonIntegration(const QDirection &dir,
		std::function<INTTYPE(Vector3QLength)> f, int N = 100) {
	
	Vector3QLength positionSun(8.5_kpc, 0, 0);
	QLength maxDistance = distanceToGalBorder(positionSun, dir);
	
	auto fw = [f, dir, positionSun](const QLength &dist) {
		return f(getGalacticPosition(positionSun, dist, dir)); };
	
	QLength a = 0_pc;
	QLength b = maxDistance;

        QLength h = (b - a)/N;
        INTTYPE XI0 = fw(a) + fw(b);
        QLength X = 0;
	INTTYPE XI1 = 0, XI2 = 0;

        for (int i = 1; i < N; ++i) {
                X = a + i*h;
                if (i % 2 == 0)
                        XI2 = XI2 + fw(X);
                else
                        XI1 = XI1 + fw(X);
        }

        return h*(XI0 + 2*XI2 + 4*XI1)/3.0;
}


template <typename QPXL, typename INTTYPE, typename QSTEP>
QPXL sumIntegration(const QDirection &dir,
		std::function<INTTYPE(Vector3QLength, QSTEP)> f, const QSTEP &step, int N = 100) {
	
	Vector3QLength positionSun(8.5_kpc, 0, 0);
	// distance from the (spherical) galactic border in the given direction
	QLength maxDistance = distanceToGalBorder(positionSun, dir);
	QLength delta_d = maxDistance/N;

	QPXL total(0);
	Vector3QLength pos(0.0);

	for(QLength dist = 0; dist <= maxDistance; dist += delta_d) {
		pos = getGalacticPosition(positionSun, dist, dir);
		total += f(pos, step) * delta_d;
	}

	return total;
}

template <typename QPXL, typename INTTYPE, typename QSTEP>
QPXL simpsonIntegration(const QDirection &dir,
		std::function<INTTYPE(Vector3QLength, QSTEP)> f, const QSTEP &step, int N = 100) {
	
	Vector3QLength positionSun(8.5_kpc, 0, 0);
	// distance from the (spherical) galactic border in the given direction
	QLength maxDistance = distanceToGalBorder(positionSun, dir);
	
	auto fw = [f, dir, positionSun, step](const QLength &dist) {
		return f(getGalacticPosition(positionSun, dist, dir), step); };
	
	QLength a = 0_pc;
	QLength b = maxDistance;

        QLength h = (b - a)/N;
        INTTYPE XI0 = fw(a) + fw(b);
        QLength X = 0;
	INTTYPE XI1 = 0, XI2 = 0;

        for (int i = 1; i < N; ++i) {
                X = a + i*h;
                if (i % 2 == 0)
                        XI2 = XI2 + fw(X);
                else
                        XI1 = XI1 + fw(X);
        }

        return h*(XI0 + 2*XI2 + 4*XI1)/3.0;
}

template <typename QPXL, typename INTTYPE, typename QSTEP>
QPXL adaptiveSimpsonIntegration(const QDirection &dir,
		std::function<INTTYPE(Vector3QLength, QSTEP)> f, const QSTEP &step,
		QPXL tolerance, int N = 30) {
	
	Vector3QLength positionSun(8.5_kpc, 0, 0);
	QLength maxDistance = distanceToGalBorder(positionSun, dir);

	QPXL total(0);

	auto fw = [f, dir, positionSun, step](const QLength &dist) {
		return f(getGalacticPosition(positionSun, dist, dir), step); };
	
	QLength a = 0_pc;
	QLength b = maxDistance;

        std::vector<QPXL> tolerance_i(N, 10*tolerance);
        std::vector<QLength> a_i(N, a);
        std::vector<QLength> h_i(N, (b-a)/2.);
        std::vector<double> L_i(N, 1);

        INTTYPE FA = fw(a);
        INTTYPE FB = fw(b);
        INTTYPE FD, FE;
	QPXL S1, S2;
	INTTYPE v2, v3, v4;
        QLength v1, v5;
	QPXL v6, v7;
       	double v8;
        std::vector<INTTYPE> FA_i(N, FA);
        std::vector<INTTYPE> FB_i(N, FB);
        std::vector<INTTYPE> FC_i(N, 0);
        std::vector<QPXL> S_i(N, 0);
        for (int j = 0; j < N; ++j) {
                FC_i[j] = fw(a + h_i[j]);
                S_i[j] = h_i[j] * (FA_i[j] + 4*FC_i[j] + FB_i[j])/3.0;
        }

        int i = 0;
        while (i >= 0) {
                FD = fw(a_i[i] + h_i[i]/2);
                FE = fw(a_i[i] + 3*h_i[i]/2);
                S1 = h_i[i] * (FA_i[i] + 4*FD + FC_i[i])/6.0;
                S2 = h_i[i] * (FC_i[i] + 4*FE + FB_i[i])/6.0;
                v1 = a_i[i];
                v2 = FA_i[i];
                v3 = FC_i[i];
                v4 = FB_i[i];
                v5 = h_i[i];
                v6 = tolerance_i[i];
                v7 = S_i[i];
                v8 = L_i[i];

                i -= 1;

                if (fabs(S1 + S2 - v7) < v6) {
                        total += (S1 + S2);
                } else {
                        assert(v8 < N);

                        i += 1; // data for the right half subinterval
                        a_i[i] = v1 + v5;
                        FA_i[i] = v3;
                        FC_i[i] = FE;
                        FB_i[i] = v4;
                        h_i[i] = v5/2.0;
                        tolerance_i[i] = v6/2.0;
                        S_i[i] = S2;
                        L_i[i] = v8 + 1;

                        i += 1; // data for the left half subinterval
                        a_i[i] = v1;
                        FA_i[i] = v2;
                        FC_i[i] = FD;
                        FB_i[i] = v3;
                        h_i[i] = h_i[i-1];
                        tolerance_i[i] = tolerance_i[i-1];
                        S_i[i] = S1;
                        L_i[i] = L_i[i-1];
                }
        }

        return total;
}

template <typename QPXL, typename INTTYPE, typename QSTEP>
QPXL saveIntegrationProfile(const QDirection &dir,
		std::function<INTTYPE(Vector3QLength, QSTEP)> f, const QSTEP &step, int N = 200) {
	Vector3QLength positionSun(8.5_kpc, 0, 0);
	// distance from the (spherical) galactic border in the given direction
	QLength maxDistance = distanceToGalBorder(positionSun, dir);
	QLength delta_d = maxDistance/N;

	QPXL total(0);
	QPXL tmp(0);
	Vector3QLength pos(0.0);

	std::ostringstream filename;
	filename << "integral_profiles/integral-"
		 << "b" << static_cast<double>(dir[0])
		 << "-"
		 << "l" << static_cast<double>(dir[1])
		 << ".txt";

	std::ofstream integraldump(filename.str());
	for(QLength dist = 0; dist <= maxDistance; dist += delta_d) {
		pos = getGalacticPosition(positionSun, dist, dir);
		tmp = f(pos, step) * delta_d;
		integraldump << dist << "\t" << tmp << "\n";
	}
	integraldump.close();

	return total;
}

} // namespace hermes

#endif // HERMES_LOSINTEGRATIONMETHODS_H
