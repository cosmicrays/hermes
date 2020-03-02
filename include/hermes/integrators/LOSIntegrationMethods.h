#ifndef HERMES_LOSINTEGRATIONMETHODS_H
#define HERMES_LOSINTEGRATIONMETHODS_H

#include "hermes/Units.h"
#include "hermes/Grid.h"
#include "hermes/Common.h"

#include <fstream>
#include <sstream>
#include <cassert>
#include <functional>

#include <gsl/gsl_integration.h>

#define GSL_LIMIT 1000

namespace hermes {

// dim(QPXL) = dim(INTTYPE) * dim(L)
template <typename QPXL, typename INTTYPE>
QPXL sumIntegration(std::function<INTTYPE(QLength)> f,
		QLength start, QLength stop, int N = 100) {
	
	QLength delta_d = (stop-start)/N;

	QPXL total(0);
	for(QLength dist = start; dist <= stop; dist += delta_d) {
		total += f(dist) * delta_d;
	}
	return total;
}

// dim(QPXL) = dim(INTTYPE) * dim(L)
template <typename QPXL, typename INTTYPE>
QPXL trapesoidIntegration(std::function<INTTYPE(QLength)> f,
		QLength start, QLength stop, int N = 100) {
	
	QLength delta_d = (stop-start)/N;

	QPXL total(0);
	for(QLength dist = start; dist <= stop - delta_d; dist += delta_d) {
		total += (f(dist) + f(dist+delta_d))/2. * delta_d;
	}
	return total;
}

// dim(QPXL) = dim(INTTYPE) * dim(L)
template <typename QPXL, typename INTTYPE>
QPXL simpsonIntegration(std::function<INTTYPE(QLength)> f,
		QLength start, QLength stop, int N = 100) {
	
	QLength a = start;
	QLength b = stop;

        QLength h = (b - a)/N;
        INTTYPE XI0 = f(a) + f(b);
        QLength X = 0;
	INTTYPE XI1 = 0, XI2 = 0;

        for (int i = 1; i < N; ++i) {
                X = a + i*h;
                if (i % 2 == 0)
                        XI2 = XI2 + f(X);
                else
                        XI1 = XI1 + f(X);
        }

        return h*(XI0 + 2*XI2 + 4*XI1)/3.0;
}
	
// 8-points Gauß-Legendre integral
static const double X[8] = {.0950125098, .2816035507, .4580167776, .6178762444, .7554044083, .8656312023, .9445750230, .9894009349};
static const double W[8] = {.1894506104, .1826034150, .1691565193, .1495959888, .1246289712, .0951585116, .0622535239, .0271524594};

template <typename QPXL, typename INTTYPE>
QPXL gaussIntegration(std::function<INTTYPE(QLength)> f, QLength start, QLength stop, int N = 1) {
	const QLength XM = 0.5 * (stop + start);
	const QLength XR = 0.5 * (stop - start);
	INTTYPE SS = 0.;
	for (int i = 0; i < 8; ++i) {
		QLength DX = XR * X[i];
		SS += W[i] * (f(XM + DX) + f(XM - DX));
	}
	return XR * SS;
}	

template <typename QPXL, typename INTTYPE>
QPXL gslQAGIntegration(std::function<INTTYPE(QLength)> f,
		QLength start, QLength stop, int N) {

	double a = static_cast<double>(start);
	double b = static_cast<double>(stop);
	double abs_error = 0.0; // disabled
	double rel_error = 1.0e-3;
	int key = GSL_INTEG_GAUSS21; //GSL_INTEG_GAUSS15;
	double result;
	double error;
	
	gsl_function F = {
  		.function = [](double x, void * vf) -> double {
		    auto& func = *static_cast<std::function<double(double)>*>(vf);
		    return func(x);
		  },
	  	.params = &f
	};
	
	gsl_integration_workspace * workspace_ptr = gsl_integration_workspace_alloc(GSL_LIMIT);
	gsl_integration_qag(&F, a, b, abs_error, rel_error, N, key, workspace_ptr, &result, &error);
	gsl_integration_workspace_free(workspace_ptr);

	return QPXL(result);
}

template <typename QPXL, typename INTTYPE>
QPXL adaptiveSimpsonIntegration(std::function<INTTYPE(QLength)> f,
		QLength start, QLength stop, QPXL tolerance, int N = 30) {
	
	QPXL total(0);

	QLength a = start;
	QLength b = stop;

        std::vector<QPXL> tolerance_i(N, 10*tolerance);
        std::vector<QLength> a_i(N, a);
        std::vector<QLength> h_i(N, (b-a)/2.);
        std::vector<double> L_i(N, 1);

        INTTYPE FA = f(a);
        INTTYPE FB = f(b);
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
                FC_i[j] = f(a + h_i[j]);
                S_i[j] = h_i[j] * (FA_i[j] + 4*FC_i[j] + FB_i[j])/3.0;
        }

        int i = 0;
        while (i >= 0) {
                FD = f(a_i[i] + h_i[i]/2);
                FE = f(a_i[i] + 3*h_i[i]/2);
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

} // namespace hermes

#endif // HERMES_LOSINTEGRATIONMETHODS_H
