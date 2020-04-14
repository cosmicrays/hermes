#ifndef HERMES_YMW16_H
#define HERMES_YMW16_H

#include "hermes/chargedgas/ChargedGasDensity.h"

/*Copyright (C) 2016, 2017  J. M. Yao, R. N. Manchester, N. Wang.

This file is part of the YMW16 program. YMW16 is a model for the
distribution of free electrons in the Galaxy, the Magellanic Clouds
and the inter-galactic medium that can be used to estimate distances
for real or simulated pulsars and fast radio bursts (FRBs) based on
their position and dispersion measure.

YMW16 is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

YMW16 is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License,
available at http://www.gnu.org/licenses/, for more details. 

Please report any issues or bugs at
https://bitbucket.org/psrsoft/ymw16/issues/new/ or directly to the
authors. Please provide an example illustrating the problem.

Jumei Yao (yaojumei@xao.ac.cn), Richard N Manchester
(dick.manchester@csiro.au), Na Wang (na.wang@xao.ac.cn).

Modifed and optimized for C++ and multithreading execution
by Andrej Dundovic (andrej.dundovic@gssi.it, 2020)
*/


namespace hermes { namespace chargedgas {

// Thick disk
#define P_Ad 2500
#define P_Bd 15000 
#define P_n1 0.01132
#define P_H1 1673

// Thin disk

#define P_A2 1200
#define P_B2 4000  
#define P_n2 0.404
#define P_K2 1.54

// Spiral arm
#define P_B2s 4000
#define P_Ka 5.015
#define P_Ele_arm_0 0.135000
#define P_Ele_arm_1 0.129000
#define P_Ele_arm_2 0.103000
#define P_Ele_arm_3 0.116000
#define P_Ele_arm_4 0.005700
#define P_Wid_arm_0 300
#define P_Wid_arm_1 500
#define P_Wid_arm_2 300
#define P_Wid_arm_3 500
#define P_Wid_arm_4 300
#define P_Aa 11680
#define P_ncn 2.4
#define P_wcn 8.2
#define P_thetacn 109
#define P_nsg 0.626
#define P_wsg 20
#define P_thetasg 75.8

// Warp
#define P_Gamma_w 0.14

// Sun
#define P_z_Sun 6.0

// Galactic center
#define P_ngc 6.2
#define P_Agc 160 
#define P_Hgc 35

// Gum nebula
#define P_ngn 1.84
#define P_Wgn 15.1
#define P_Agn 125.8
#define P_Kgn 1.4

// Local enhancement
#define P_J_LB 0.480
#define P_nlb1 1.094
#define P_detlb1 28.4
#define P_wlb1 14.2
#define P_hlb1 112.9
#define P_thetalb1 195.4
#define P_nlb2 2.33
#define P_detlb2 14.7 
#define P_wlb2 15.6
#define P_hlb2 43.6
#define P_thetalb2 278.2

// Loop I
#define P_nLI 1.907
#define P_RLI 80
#define P_WLI 15 
#define P_detthetaLI 30
#define P_thetaLI 40
 
// Fermi Bubble
#define P_J_FB 1

// LMC
#define P_nlmc 0.066

// 30 Doradus
#define P_n30D 0.32

// SMC
#define P_nsmc 0.045

// Spiral arms
#define sp_1_1	3.3500e+03
#define sp_1_2	7.7000e-01
#define sp_1_3	2.0200e-01
#define sp_1_4	9.8000e-01
#define sp_1_5	1.9800e-01
#define sp_2_1	3.7070e+03
#define sp_2_2	2.0930e+00
#define sp_2_3	1.7300e-01
#define sp_2_4	9.8500e-01
#define sp_2_5	1.7090e-01
#define sp_3_1	3.5600e+03
#define sp_3_2	3.8100e+00
#define sp_3_3	1.8300e-01
#define sp_3_4	9.8360e-01
#define sp_3_5	1.8020e-01
#define sp_4_1	3.6700e+03
#define sp_4_2	5.7600e+00
#define sp_4_3	1.8600e-01
#define sp_4_4	9.8300e-01
#define sp_4_5	1.8290e-01
#define sp_5_1	8.2100e+03
#define sp_5_2	9.6000e-01
#define sp_5_3	4.8300e-02
#define sp_5_4	9.9880e-01
#define sp_5_5	4.8300e-02	

// Local bubble
#define Rlb 110

#define R0 8.3
#define RAD 57.295779
#define N0 0.013
#define mc 6
#define MAX(a,b) ( ((a)>(b)) ? (a):(b) )
#define MIN(a,b) ( ((a)>(b)) ? (b):(a) )

class YMW16: public ChargedGasDensity {
private:
	QPDensity fne1, fne2;
	QLength	  H1, A1, H2, A2, R2;

	struct Warp_Sun {
		double Gamma_w;
		double z_Sun;
	} t0;

	struct Thick {
		double Ad;
		double Bd; 
		double n1; 
		double H1;
	} t1;

	struct Thin {
		double A2;
		double B2; 
		double n2;
		double K2;
	} t2;

	struct Spiral
	{
		double Ads;
		double Bds;
		double B2s;
		double Ka;
		double narm[5];
		double warm[5];
		double Aa;
		double ncn;
		double wcn;
		double thetacn;
		double nsg;
		double wsg;
		double thetasg;
	} t3;

	struct GC
	{
		double ngc;
		double Agc;
		double Hgc;
	} t4;

	struct Gum
	{
		double Kgn;
		double ngn;
		double Wgn;
		double Agn;
	} t5;

	struct LB
	{
		double J_LB;
		double nlb1;
		double detlb1;
		double wlb1;
		double hlb1;
		double thetalb1;
		double nlb2;
		double detlb2;
		double wlb2;
		double hlb2;
		double thetalb2;
	} t6;

	struct LI
	{
		double nLI;
		double RLI;
		double WLI;
		double detthetaLI;
		double thetaLI;
	} t7;

	struct FB
	{
		double J_FB;
	} t8;

	struct LMC
	{
		double nlmc;
	} t9;

	struct Dora
	{
		double n30D;
	} t10;

	struct SMC
	{
		double nsmc;
	} t11;

	void initParameters();

public:
	YMW16();
	YMW16(const QTemperature &t);
	QPDensity getDensity(const Vector3QLength& pos) const override;

	double ne_ymw16(const Vector3QLength &pos) const;
	double thick(double xx, double yy, double zz, double *gd, double rr) const;
	double thin(double xx, double yy, double zz, double gd, double rr) const;
	double galcen(double xx, double yy, double zz) const;
	double spiral(double xx,  double yy,  double zz,  double gd,  double rr, int *ww, int *m_3) const;
	double gum(double xx, double yy, double zz, int *m_5) const;
	double localbubble(double xx, double yy, double zz, double gl,double gb, double *WW, int *m_6) const;
	double nps(double xx, double yy, double zz, int *WLI, int *m_7) const;
	double fermibubble(double xx, double yy, double zz) const;
	double lmc(double l, double b, double d, int *w_lmc) const;
	double dora(double l, double b, double d) const;
	double smc(double xx, double yy, double zz, int *w_smc) const;

};

} // namespace chargedgas
} // namespace hermes

#endif // HERMES_YMW16_H
