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
*/
#include "cn.h"
#include "ymw16par.h"

int ymw16par(struct Warp_Sun *t0, struct Thick *t1, struct Thin *t2, struct Spiral *t3, struct GC *t4, struct Gum *t5,struct LB *t6,  struct LI *t7, struct FB *t8, struct  LMC *t9, struct Dora *t10, struct SMC *t11, char *dirname){

	(*t0).Gamma_w = P_Gamma_w;
	(*t0).z_Sun = P_z_Sun;
	(*t1).Ad = P_Ad;
	(*t1).Bd = P_Bd;
	(*t1).n1 = P_n1;
	(*t1).H1 = P_H1;
	(*t2).A2 = P_A2;
	(*t2).B2 = P_B2;
	(*t2).n2 = P_n2;
	(*t2).K2 = P_K2;
	(*t3).B2s = P_B2s;
	(*t3).Aa = P_Aa;
	(*t3).ncn = P_ncn;
	(*t3).wcn = P_wcn;
	(*t3).thetacn = P_thetacn;
	(*t3).nsg = P_nsg;
	(*t3).wsg = P_wsg;
	(*t3).thetasg = P_thetasg;
	(*t3).Ka = P_Ka;
	(*t3).narm[0] = P_Ele_arm_0;
	(*t3).narm[1] = P_Ele_arm_1;
	(*t3).narm[2] = P_Ele_arm_2;
	(*t3).narm[3] = P_Ele_arm_3;
	(*t3).narm[4] = P_Ele_arm_4;
	(*t3).warm[0] = P_Wid_arm_0;
	(*t3).warm[1] = P_Wid_arm_1;
	(*t3).warm[2] = P_Wid_arm_2;
	(*t3).warm[3] = P_Wid_arm_3;
	(*t3).warm[4] = P_Wid_arm_4;
	(*t4).Agc = P_Agc;
	(*t4).Hgc = P_Hgc;
	(*t4).ngc = P_ngc;
	(*t5).Kgn = P_Kgn;
	(*t5).ngn = P_ngn;
	(*t5).Wgn = P_Wgn;
	(*t5).Agn = P_Agn;
	(*t6).J_LB = P_J_LB;
	(*t6).nlb1 = P_nlb1;
	(*t6).detlb1 = P_detlb1;
	(*t6).wlb1 = P_wlb1;
	(*t6).hlb1 = P_hlb1;
	(*t6).thetalb1 = P_thetalb1;
	(*t6).nlb2 = P_nlb2;
	(*t6).detlb2 = P_detlb2;
	(*t6).wlb2 = P_wlb2;
	(*t6).hlb2 = P_hlb2;
	(*t6).thetalb2 = P_thetalb2;
	(*t7).nLI = P_nLI;
	(*t7).RLI = P_RLI;
	(*t7).WLI = P_WLI;
	(*t7).detthetaLI = P_detthetaLI;
	(*t7).thetaLI = P_thetaLI;
	(*t8).J_FB = P_J_FB;
	(*t9).nlmc = P_nlmc;
	(*t10).n30D = P_n30D;
	(*t11).nsmc = P_nsmc;
  
	return 0;
}

