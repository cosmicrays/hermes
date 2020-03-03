#include "hermes/chargedGasDensity/YMW16.h"

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

namespace hermes {

YMW16::YMW16() : ChargedGasDensity(1e4_K) {
	initParameters();
}

YMW16::YMW16(const QTemperature &t) : ChargedGasDensity(t) {
	initParameters();
}

QPDensity YMW16::getDensity(const Vector3QLength& pos) const {
	auto conversion = [](QLength x) { return static_cast<double>(x/parsec); };

	double x = -1*conversion(pos.getY()); // change coordinates X<>Y since in YMW16
	double y = conversion(pos.getX()); // the sun position is at (0,+8.3_kpc,6.0_pc)
	double z = conversion(pos.getZ());
	QPDensity density;
	
	Vector3QLength new_pos(x,y,z);
	density = QPDensity(ne_ymw16(new_pos))/1_cm3*1_m3;

	return density;
};

void YMW16::initParameters() {

	t0 = {
		.Gamma_w = P_Gamma_w,
		.z_Sun = P_z_Sun,
	};

	t1 = {
		.Ad = P_Ad,
		.Bd = P_Bd,
		.n1 = P_n1,
		.H1 = P_H1,
	};

	t2 = {
		P_A2, // A2
		P_B2, // B2
		P_n2, // n2
		P_K2, // K2
	};	

	double Ele_arm[] = { // Ele arm
		        P_Ele_arm_0,
                        P_Ele_arm_1,
                        P_Ele_arm_2,
                        P_Ele_arm_3,
                        P_Ele_arm_4,
	};
	double Wid_arm[] = { // Wid arm
			P_Wid_arm_0,
			P_Wid_arm_1,
			P_Wid_arm_2,
			P_Wid_arm_3,
			P_Wid_arm_4,
	};

	t3.Ads = 0;
	t3.Bds = 0;
	t3.B2s = P_B2s;
	t3.Ka  = P_Ka;
	t3.Aa = P_Aa;
	t3.ncn = P_ncn;
	t3.wcn = P_wcn;
	t3.thetacn = P_thetacn;
	t3.nsg = P_nsg;
	t3.wsg = P_wsg;
	t3.thetasg = P_thetasg;

	for(int i = 0; i < 5; ++i) {
		t3.narm[i] = Ele_arm[i];
		t3.warm[i] = Wid_arm[i];
	}

	t4 = {
		.ngc = P_ngc,
		.Agc = P_Agc,
		.Hgc = P_Hgc,
	};

	t5 = {
		.Kgn = P_Kgn,
		.ngn = P_ngn,
		.Wgn = P_Wgn,
		.Agn = P_Agn,
		
	};
	
	t6 = {
		.J_LB = P_J_LB,
		.nlb1 = P_nlb1,
		.detlb1 = P_detlb1,
		.wlb1 = P_wlb1,
		.hlb1 = P_hlb1,
		.thetalb1 = P_thetalb1,
		.nlb2 = P_nlb2,
		.detlb2 = P_detlb2,
		.wlb2 = P_wlb2,
		.hlb2 = P_hlb2,
		.thetalb2 = P_thetalb2,
	};
	
	t7 = {
		P_nLI,
		P_RLI,
		P_WLI,
		P_detthetaLI,
		P_thetaLI,
	};

	t8 = {
		P_J_FB,
	};

	t9 = {
		P_nlmc,
	};

	t10 = {
		P_n30D,
	};

	t11 = {
		P_nsmc,
	};
}



double YMW16::thick(double xx, double yy, double zz, double *gd, double rr) const {
	double gdd, gg;

	if (std::fabs(zz) > mc*t1.H1 || (rr-t1.Bd) > mc*t1.Ad) {
		return 0;
	}else{
		if(rr<t1.Bd){
			gdd=1;
		}else{
			gg=std::exp(-(rr-t1.Bd)/t1.Ad)+std::exp((rr-t1.Bd)/t1.Ad);
			gdd=std::pow(2/gg,2);
		}
	}
	*gd=gdd;
	return t1.n1*gdd*std::pow(2/(std::exp(-std::fabs(zz)/t1.H1)+std::exp(std::fabs(zz)/t1.H1)), 2);
}

double YMW16::thin(double xx, double yy, double zz, double gd, double rr) const {
	double g2, Hg, ex1, ex2, g3, HH;

	Hg=32+0.0016*rr+0.0000004*std::pow(rr, 2);
	HH=t2.K2*Hg;
	if((rr-t2.B2)>(mc*t2.A2)||(fabs(zz)>(mc*HH))) 
	{
		return 0; 
	}
	else
	{
		g3=(rr-t2.B2)/t2.A2;
		g2=std::pow(2/(std::exp(-g3)+std::exp(g3)), 2);
	}
	return t2.n2*gd*g2*std::pow(2/(std::exp(-zz/HH)+std::exp(zz/HH)), 2);
}

double YMW16::galcen(double xx, double yy, double zz) const {
	double ne4 = 0;
	double Xgc=50, Ygc=0, Zgc=-7;
	double Az, Ar;
	double gc;

	double Rgc = std::sqrt((xx-Xgc)*(xx-Xgc)+(yy-Ygc)*(yy-Ygc));

	if(Rgc > (mc*t4.Agc) || 
			std::fabs(zz) > (mc*t4.Hgc)) {
		return 0;
	} else {
		gc = 1;
		Ar = std::exp(-(Rgc*Rgc) / (t4.Agc * t4.Agc));
		Az = std::pow(2./(std::exp((zz-Zgc)/t4.Hgc) +
					std::exp(-(zz-Zgc)/t4.Hgc)), 2);
	}
	return t4.ngc * Ar * Az * gc;
}

double YMW16::spiral(double xx,  double yy,  double zz,  double gd, double rr, int *ww, int *m_3) const {
	int i, which_arm;
	static double rmin[5], thmin[5], tpitch[5], cspitch[5], sspitch[5];
	double detrr=1e10;
	double armr1, armr2, smin, sminmin, saxis, uu, Aaa, HH, Hg;
	double ne3s=0;
	double theta, alpha, armr;
	double sech2=0;
	double ga=0;
	double g1=0;

	if(*m_3 >= 1)
		return 0;

	Hg = 32+0.0016*rr+0.0000004*std::pow(rr, 2);
	HH = t3.Ka * Hg;

	if(*ww==1){
		rmin[0] = sp_1_1; thmin[0] = sp_1_2; tpitch[0] = sp_1_3; cspitch[0] = sp_1_4; sspitch[0] = sp_1_5;
		rmin[1] = sp_2_1; thmin[1] = sp_2_2; tpitch[1] = sp_2_3; cspitch[1] = sp_2_4; sspitch[1] = sp_2_5;
		rmin[2] = sp_3_1; thmin[2] = sp_3_2; tpitch[2] = sp_3_3; cspitch[2] = sp_3_4; sspitch[2] = sp_3_5;
		rmin[3] = sp_4_1; thmin[3] = sp_4_2; tpitch[3] = sp_4_3; cspitch[3] = sp_4_4; sspitch[3] = sp_4_5;
		rmin[4] = sp_5_1; thmin[4] = sp_5_2; tpitch[4] = sp_5_3; cspitch[4] = sp_5_4; sspitch[4] = sp_5_5;
		(*ww)++;
	}

	theta = std::atan2(yy,xx);
	if(theta < 0)
		theta = 2*pi + theta;

	if(std::fabs(zz/300)<10){
		sminmin=1e10;
		ne3s=0.;
		for(i=0;i<=4;i++){
			ga=0;
			//Norma-Outer
			if(i==0)
			{
				if(theta>=0&&theta<0.77)
				{
					armr = rmin[i]*exp((theta+2*pi-thmin[i])*tpitch[i]);
					detrr = std::fabs(rr-armr);
				}
				if(theta>=0.77&&theta<6.28)
				{
					armr1 = rmin[i]*std::exp((theta-thmin[i])*tpitch[i]);
					armr2 = rmin[i]*std::exp((theta+2*pi-thmin[i])*tpitch[i]);
					detrr = MIN(std::fabs(rr-armr1), std::fabs(rr-armr2));
				}
			}
			//Perseus
			if(i==1)
			{
				if(theta>=0&&theta<2.093)
				{
					armr=rmin[i]*std::exp((theta+2*pi-thmin[i])*tpitch[i]);
					detrr=std::fabs(rr-armr);
				}
				if(theta>=2.093&&theta<6.28)
				{
					armr1=rmin[i]*std::exp((theta-thmin[i])*tpitch[i]);
					armr2=rmin[i]*std::exp((theta+2*pi-thmin[i])*tpitch[i]);
					detrr=MIN(std::fabs(rr-armr1), std::fabs(rr-armr2));
				}
			}
			//Carina-Sagittarius
			if(i==2)
			{
				if(theta>=0&&theta<3.81)
				{
					armr1=rmin[i]*std::exp((theta+2*pi-thmin[i])*tpitch[i]);
					armr2=rmin[i]*std::exp((theta+4*pi-thmin[i])*tpitch[i]);
					detrr=MIN(std::fabs(rr-armr1), std::fabs(rr-armr2));
				}
				if(theta>=3.81&&theta<6.28)
				{
					armr1=rmin[i]*std::exp((theta-thmin[i])*tpitch[i]);
					armr2=rmin[i]*std::exp((theta+2*pi-thmin[i])*tpitch[i]);
					detrr=MIN(std::fabs(rr-armr1), std::fabs(rr-armr2));
				}
			}
			//Crux_Scutum
			if(i==3)
			{
				if(theta>=0&&theta<5.76)
				{
					armr1=rmin[i]*std::exp((theta+2*pi-thmin[i])*tpitch[i]);
					armr2=rmin[i]*std::exp((theta+4*pi-thmin[i])*tpitch[i]);
					detrr=MIN(std::fabs(rr-armr1), std::fabs(rr-armr2));
				}
				if(theta>=5.76&&theta<6.28)
				{
					armr1=rmin[i]*std::exp((theta-thmin[i])*tpitch[i]);
					armr2=rmin[i]*std::exp((theta+2*pi-thmin[i])*tpitch[i]);
					detrr=MIN(std::fabs(rr-armr1), std::fabs(rr-armr2));
				}
			}
			//Local
			if(i==4)
			{
				if(theta>=0&&theta<0.96)
				{
					detrr=1e10;
				}
				if(theta>=0.96&&theta<2)
				{
					armr=rmin[i]*std::exp((theta-thmin[i])*tpitch[i]);
					detrr=std::fabs(rr-armr);
				}
				if(theta>=2&&theta<6.28)
				{
					detrr=1e10;
				}
			}
			if(detrr > mc * t3.warm[i]) {
				ga=0;
				continue;
			} else {

				smin=detrr*cspitch[i];
				saxis=detrr*sspitch[i];
				if(i==2)
				{
					ga=(1-(t3.nsg)*(std::exp(-((theta*RAD-t3.thetasg)*(theta*RAD-t3.thetasg))/(t3.wsg*t3.wsg))))*(1+t3.ncn*std::exp(-((theta*RAD-t3.thetacn)*(theta*RAD-t3.thetacn))/(t3.wcn*t3.wcn)))*std::pow(2/(std::exp(-smin/t3.warm[i])+std::exp(smin/t3.warm[i])), 2);
					if(rr>6000 && theta*RAD>t3.thetacn) ga=(1-(t3.nsg)*(std::exp(-((theta*RAD-t3.thetasg)*(theta*RAD-t3.thetasg))/(t3.wsg*t3.wsg))))*(1+t3.ncn)*std::pow(2/(std::exp(-smin/t3.warm[i])+std::exp(smin/t3.warm[i])), 2);

				} else {
					ga=std::pow(2/(std::exp(-smin/t3.warm[i])+std::exp(smin/t3.warm[i])), 2);
				}
				
				if(smin < sminmin) {
					sminmin=smin;
					which_arm=i;
				}
			}
			if(gd == 0 || std::fabs(zz) > (mc*HH)) { 
				(*m_3)++;
				return 0;
			}
			sech2 = std::pow((2/(std::exp((rr-t3.B2s)/t3.Aa)+std::exp(-(rr-t3.B2s)/t3.Aa))), 2);
			ga = ga*sech2*gd;
			uu = std::pow(2/(std::exp((std::fabs(zz))/HH)+std::exp(-std::fabs(zz)/HH)), 2);
			ne3s += t3.narm[i]*ga*uu;
		}
		return ne3s;
	}
	return 0;
}

double YMW16::gum(double xx, double yy, double zz, int *m_5) const {
	double slc, clc, sbc, cbc, xc, yc, zc, rgalc;
	double rp, RR, xyp, zp;
	double theta, alpha;
	double Dmin=1e5;

	//center of Gum Nebula
	const double lc=264;
	const double bc=-4;
	const double dc=450;

	if(*m_5 >= 1)
		return 0;

	slc = std::sin(lc/RAD);
	clc = std::cos(lc/RAD);
	sbc = std::sin(bc/RAD);
	cbc = std::cos(bc/RAD);

	rgalc=dc*cbc;
	xc=rgalc*slc;
	yc=R0*1000-rgalc*clc;
	zc=dc*sbc;

	theta=std::fabs(std::atan((zz-zc)/std::sqrt((xx-xc)*(xx-xc)+(yy-yc)*(yy-yc))));
	zp=((t5.Agn)*(t5.Agn)*(t5.Kgn))/std::sqrt(((t5.Agn)*(t5.Agn))+((t5.Agn)*(t5.Agn)*(t5.Kgn)*(t5.Kgn))/(std::tan(theta)*std::tan(theta)));
	xyp=zp/std::tan(theta);
	if((t5.Agn-std::fabs(xyp))<1e-15)alpha=pi/2;
	else alpha=-std::atan((-(t5.Agn)*(t5.Kgn)*xyp)/((t5.Agn)*std::sqrt((t5.Agn)*(t5.Agn)-xyp*xyp)));
	RR=std::sqrt((xx-xc)*(xx-xc)+(yy-yc)*(yy-yc)+(zz-zc)*(zz-zc));
	rp=std::sqrt((zp)*(zp)+(xyp)*(xyp));
	Dmin=std::fabs((RR-rp)*sin(theta+alpha));

	if(Dmin > (mc*t5.Wgn)) {
		if(RR > 500)
			(*m_5)++;
		return 0;
	}
	return (t5.ngn)*exp(-Dmin*Dmin/((t5.Wgn)*(t5.Wgn)));
}

double YMW16::localbubble(double xx, double yy, double zz, double gl,double gb, double *WW, int *m_6) const {
	double g4=0;
	double g5=0;
	double g6=0;
	double g7=0;
	double glb1, glb2;
	double UU;
	double VVV=0;
	double WWW=0;
	double nelb1=1e10;
	double nelb2=1e10;
	double Y_C=1e10;
	double COS_A=1e10;
	double COS_E=cos(pi/4);
	//  printf("m_6=%d\n\n",m_6);

	Y_C=8340+(0.34/0.94)*zz;//Center of local bubble
	COS_A=(xx*xx+(8300-yy)*(Y_C-yy))/(sqrt(xx*xx+(8300-yy)*(8300-yy))*sqrt(xx*xx+(Y_C-yy)*(Y_C-yy)));
	UU=sqrt(((yy-8340)*0.94-0.34*zz)*((yy-8340)*0.94-0.34*zz)+xx*xx);
	*WW=UU;
	if(*m_6>=1)return 0; 
	if((UU-Rlb)>(mc*t6.wlb1)||std::fabs(zz)>(mc*t6.hlb1)){
		nelb1=0;
	}else{
		glb1=1;
		g4=(UU-Rlb)/t6.wlb1;
		g5=std::pow(2/(std::exp(-zz/t6.hlb1)+std::exp(zz/t6.hlb1)), 2);
		VVV=MIN(std::fabs(gl+360-t6.thetalb1),std::fabs(t6.thetalb1-gl));
		if(VVV>(mc*t6.detlb1)) glb1=0;
		else glb1=1;
		nelb1=glb1*std::pow(2/(std::exp(-VVV/t6.detlb1)+std::exp(VVV/t6.detlb1)), 2)*t6.nlb1*std::pow(2/(std::exp(g4)+std::exp(-g4)), 2)*g5;
	}
	if((UU-Rlb)>(mc*t6.wlb2)||std::fabs(zz)>(mc*t6.hlb2)) nelb2=0;
	if(nelb1==0&&nelb2==0){
		(*m_6)++;
		return 0;
	}else{
		glb2=1;
		g6=(UU-Rlb)/t6.wlb2;
		g7=std::pow(2/(std::exp(-zz/t6.hlb2)+std::exp(zz/t6.hlb2)), 2);
		WWW=MIN(std::fabs(gl+360-t6.thetalb2),std::fabs(t6.thetalb2-gl));
		if(WWW>(mc*t6.detlb2)) glb2=0;
		else glb2=1;
		nelb2=glb2*std::pow(2/(std::exp(-WWW/t6.detlb2)+std::exp(WWW/t6.detlb2)), 2)*t6.nlb2*std::pow(2/(std::exp(g6)+std::exp(-g6)), 2)*g7;
	}

	if(COS_A<COS_E)nelb1=0; 
	if((gb==90)&&(gl==180))nelb1=0;

	return nelb1+nelb2;
}

double YMW16::nps(double xx, double yy, double zz, int *WLI, int *m_7) const {
	double x_c, y_c, z_c;
	double gLI;
	double theta_LI;

	if(*m_7>=1) return 0;

	theta_LI = (t7.thetaLI)/RAD;
	x_c = -10.156;
	y_c = 8106.206;
	z_c = 10.467;
	double rr,theta;
	rr = std::sqrt((xx-x_c)*(xx-x_c)+(yy-y_c)*(yy-y_c)+(zz-z_c)*(zz-z_c));
	theta = std::acos(((xx-x_c)*(std::cos(theta_LI))+(zz-z_c)*(std::sin(theta_LI)))/rr)*RAD;
	*WLI=1;
	if(std::fabs(rr-t7.RLI)>(mc*t7.WLI)||std::fabs(theta)>(mc*t7.detthetaLI))
	{ if(rr>500) (*m_7)++;
		return 0;
	}
	else gLI=1;
	
	return gLI*(t7.nLI)*exp(-((rr-(t7.RLI))*(rr-(t7.RLI)))/((t7.WLI)*(t7.WLI)))*exp((-theta*theta)/((t7.detthetaLI)*(t7.detthetaLI)));
}

double YMW16::fermibubble(double xx, double yy, double zz) const {
	double fbnz, fbsz, na, nb, sa, sb, N, S;
	//center of Fermi bubble
	fbnz=0.5*8300*std::tan(50/RAD);
	fbsz=-(0.5*(8300*std::tan(50/RAD)-8300*std::tan(0/RAD))+(8300*std::tan(0/RAD)));
	//min_axis and max_axis of Fermi bubble
	na = fbnz;
	nb = 8300*std::tan(20/RAD);
	sa = 0.5*(8300*std::tan(50/RAD)-8300*std::tan(0/RAD));
	sb = 8300*std::tan(20/RAD);
	N = (std::pow(xx, 2)/(nb*nb))+(std::pow(yy, 2)/(nb*nb))+(std::pow(zz-fbnz, 2)/(na*na));
	S = (std::pow(xx, 2)/(sb*sb))+(std::pow(yy, 2)/(sb*sb))+(std::pow(zz-fbsz, 2)/(sa*sa));
	if( N<1 || S<1 )
		return 1;
	else
		return 0;
}

double YMW16::lmc(double l, double b, double d, int *w_lmc) const {
	//coordinate system
	double X, Y, Z;
	//parameter
	double dag, pag, npag, iag;
	npag=(116+90)/RAD;
	iag=32/RAD;
	double l_cp, alpha_cp, delta_cp, alpha, delta;
	//constant
	l_cp=122.9/RAD;
	alpha_cp=192.85/RAD;
	delta_cp=27.13/RAD;
	//parameter of lmc
	double alpha_l=81/RAD;
	double delta_l=-69.75/RAD;
	double D_l=49700; //pc
	double Rl;
	double Al=3000;//pc
	double Hl=800;//pc
	int gl, gD;
	double s_delta, c_delta, s_alpha_cp, c_alpha_cp;
	double s_cpl,c_cpl, c_dag,s_dag,sc_dp,ss_dp;
	//(gl,gb,d)-----get(alpha,delta,d) for a point
	s_delta=sin(b)*sin(delta_cp)+cos(b)*cos(delta_cp)*cos(l_cp-l);
	c_delta=cos(asin(s_delta));
	s_alpha_cp=(sin(l_cp-l)*cos(b))/c_delta;
	c_alpha_cp=(sin(b)*cos(delta_cp)-cos(b)*sin(delta_cp)*cos(l_cp-l))/c_delta;
	s_cpl=sin(alpha_cp-alpha_l);
	c_cpl=cos(alpha_cp-alpha_l);
	// ((alpha,delta,d)----((alpha_l,delta_l,D_l)-----get(dag,pag)
	c_dag=c_delta*cos(delta_l)*(c_alpha_cp*c_cpl-s_alpha_cp*s_cpl)+s_delta*sin(delta_l);
	s_dag=sin(acos(c_dag));
	sc_dp=-c_delta*(s_alpha_cp*c_cpl+c_alpha_cp*s_cpl);
	ss_dp=s_delta*cos(delta_l)-c_delta*sin(delta_l)*(c_alpha_cp*c_cpl-s_alpha_cp*s_cpl);
	//(xc,yc,zc,iag,npag)------get(X,Y,Z) is (x',y',z')
	X=d*(sc_dp*cos(npag)+ss_dp*sin(npag));
	Y=d*(cos(iag)*(ss_dp*cos(npag)-sc_dp*sin(npag))+c_dag*sin(iag))-D_l*sin(iag);
	Z=d*(sin(iag)*(ss_dp*cos(npag)-sc_dp*sin(npag))-c_dag*cos(iag))+D_l*cos(iag);
	Rl=sqrt(X*X+Y*Y);
	if(Rl>mc*Al||fabs(Z)>mc*Hl)
	{
		return 0;
	}
	else
	{
		gl=1;
		*w_lmc=1;

	}
	return gl * (t9.nlmc)*std::exp(-(Rl*Rl)/(Al*Al))*std::pow(2/(std::exp(-Z/Hl)+std::exp(Z/Hl)),2);
}

double YMW16::dora(double l, double b, double d) const {
	//paramter
	double dag, pag, npag, iag;
	npag=(116+90)/RAD;
	iag=32/RAD;
	double l_cp, alpha_cp, delta_cp, alpha, delta;
	//constant
	l_cp=122.9/RAD;
	alpha_cp=192.85/RAD;
	delta_cp=27.13/RAD;
	//parameter of lmc
	double alpha_l=81/RAD;
	double delta_l=-69.75/RAD;
	double D_l=49700; //pc
	double Rl;
	double Al=3000;//pc
	double Hl=800;//pc
	int gl,gD;
	//parameter for 30D
	double alpha_D=85/RAD;
	double delta_D=-69/RAD;
	double dag_D, pag_D, xc_D, yc_D, zc_D, X_D, Y_D, Z_D, R_D, A_D;
	A_D=450 ;//pc
	double D_D=49045;
	double s_delta, c_delta, s_alpha_cp, c_alpha_cp;
	double s_cpl,c_cpl, c_dag,s_dag,sc_dp,ss_dp;
	double c_dagD,s_dagD,sc_dpD,ss_dpD;
	//(gl,gb,d)-----get(alpha,delta,d) for a point
	s_delta=std::sin(b)*std::sin(delta_cp)+std::cos(b)*std::cos(delta_cp)*std::cos(l_cp-l);
	c_delta=std::cos(std::asin(s_delta));
	s_alpha_cp=(std::sin(l_cp-l)*std::cos(b))/c_delta;
	c_alpha_cp=(std::sin(b)*std::cos(delta_cp)-std::cos(b)*std::sin(delta_cp)*std::cos(l_cp-l))/c_delta;
	s_cpl=std::sin(alpha_cp-alpha_l);
	c_cpl=std::cos(alpha_cp-alpha_l);
	// ((alpha,delta,d)----((alpha_l,delta_l,D_l)-----get(dag,pag)
	c_dag=c_delta*std::cos(delta_l)*(c_alpha_cp*c_cpl-s_alpha_cp*s_cpl)+s_delta*std::sin(delta_l);
	s_dag=std::sin(std::acos(c_dag));
	sc_dp=-c_delta*(s_alpha_cp*c_cpl+c_alpha_cp*s_cpl);
	ss_dp=s_delta*std::cos(delta_l)-c_delta*std::sin(delta_l)*(c_alpha_cp*c_cpl-s_alpha_cp*s_cpl);
	//(xc,yc,zc,iag,npag)------get(X,Y,Z) is (x',y',z')
	double X = d*(sc_dp*std::cos(npag)+ss_dp*std::sin(npag));
	double Y = d*(std::cos(iag)*(ss_dp*std::cos(npag)-sc_dp*std::sin(npag))+c_dag*std::sin(iag))-D_l*std::sin(iag);
	double Z = d*(std::sin(iag)*(ss_dp*std::cos(npag)-sc_dp*std::sin(npag))-c_dag*std::cos(iag))+D_l*std::cos(iag);
	//Calculation for 30 Doradus
	//1. ((alpha_D,delta_D,D_l)----((alpha_l,delta_l,D_l)-----get(dag_D,pag_D)
	c_dagD=std::cos(delta_D)*std::cos(delta_l)*std::cos(alpha_D-alpha_l)+std::sin(delta_D)*std::sin(delta_l);
	s_dagD=std::sin(std::acos(c_dagD));
	sc_dpD=-std::cos(delta_D)*std::sin(alpha_D-alpha_l);
	ss_dpD=std::sin(delta_D)*std::cos(delta_l)-std::cos(delta_D)*std::sin(delta_l)*std::cos(alpha_D-alpha_l);

	//3. (xc_D,yc_D,zc_D,iag,npag)------get(X_D,Y_D,Z_D) is (x',y',z')
	X_D=D_D*(sc_dpD*std::cos(npag)+ss_dpD*std::sin(npag));
	Y_D=D_D*(std::cos(iag)*(ss_dpD*std::cos(npag)-sc_dpD*std::sin(npag))+c_dagD*std::sin(iag))-D_l*std::sin(iag);
	Z_D=D_D*(std::sin(iag)*(ss_dpD*std::cos(npag)-sc_dpD*std::sin(npag))-c_dagD*std::cos(iag))+D_l*std::cos(iag);
	R_D=std::sqrt((X-X_D)*(X-X_D)+(Y-Y_D)*(Y-Y_D)+(Z-Z_D)*(Z-Z_D));
	if(R_D>(mc*A_D)) {
		return 0;
	} else gD = 1;
	
	return gD * (t10.n30D)*exp(-(R_D*R_D)/(A_D*A_D));
}          

double YMW16::smc(double xx, double yy, double zz, int *w_smc) const {
	double rad=57.295779;
	double R=8.3;//kpc
	double ls=303.728914;
	double bs=-44.299212;
	double ds=59700;//pc
	double Asmc=3000;//pc
	int gsmc;
	double xc, yc, zc, sls, cls, sbs, cbs, rgals, Rsmc;
	sls=sin(ls/rad);
	cls=cos(ls/rad);
	sbs=sin(bs/rad);
	cbs=cos(bs/rad);
	rgals=ds*cbs;
	xc=rgals*sls;
	yc=R*1000-rgals*cls;
	zc=ds*sbs;
	Rsmc=sqrt((xx-xc)*(xx-xc)+(yy-yc)*(yy-yc)+(zz-zc)*(zz-zc));
	if(Rsmc>(mc*Asmc))
	{
		return 0;
	}
	else 
	{
		gsmc=1;
		*w_smc=1; 
	}
	return (t11.nsmc)*gsmc*exp(-(Rsmc*Rsmc)/(Asmc*Asmc));
}


double YMW16::ne_ymw16(const Vector3QLength &pos) const {

	double x = static_cast<double>(pos.getX());
	double y = static_cast<double>(pos.getY());
	double z = static_cast<double>(pos.getZ());

	double ne = 0;

	double ne8=0;
	double ne9=0;
	double ne10=0;

	double gl, gb, dd;
	double xx, yy, zz, glr, gbr, dist;
	double x_s, y_s, z_s, ll, bb, hh, r, sl, cl, sb, cb;

	double rr = static_cast<double>(pos.getRho());
	double R_g = static_cast<double>(pos.getR());
	double gd = 0;

	//The localtion of Sun relative to GP and Warp
	double z_warp, zz_w, R_warp, theta_warp, theta_max;
	R_warp=8400;//pc
	theta_max=0.0; //In +x direction

	int WGN=0;
	int WLB=0; 
	int WLI=0;
	int WFB=0;
	int np=2;

	int m_3=0;
	int ww=1;
	int m_5=0;
	int m_6=0;
	int m_7=0;

	//parameters of MC
	int w_lmc=0;
	int w_smc=0;

	xx=x;
	yy=y;
	zz=z;
	/* (x_s, y_s, z_s) the heliocentric coordinates of the point */
	x_s=xx;
	y_s=yy-R0*1000;
	z_s=zz-t0.z_Sun;

	//D the heliocentric distance (pc)
	dist = sqrt(x_s*x_s+y_s*y_s+z_s*z_s);
	r = sqrt(x_s*x_s+y_s*y_s);

	if(dist<1e-15){
		gbr=0;
	}else{
		gbr=asin(z_s/dist);
	}
	gb=gbr*RAD;

	if(r<1e-15){
		glr=0;
	}else{
		if(x_s>=0){
			glr=acos(-y_s/r);
		}else{
			glr=acos(y_s/r)+pi;
		}
	}
	gl=glr*RAD;
	dd=dist;


	/* Definition of warp */
	if(rr<R_warp){
		zz_w=zz; 
	}
	else{
		theta_warp=std::atan2(yy,xx);
		z_warp=t0.Gamma_w*(rr-R_warp)*cos(theta_warp-theta_max);
		zz_w=zz-z_warp;
	}



	if(R_g<=30000){
		np=1;
	} else if (gl>265. && gl<315. && gb>-60. && gb<-20.)
		np=0;


	if (R_g<100000) {

		if(np==1){

			double ne1 = thick(xx, yy, zz_w, &gd, rr);
			double ne2 = thin(xx, yy, zz_w, gd, rr);
			double ne3 = spiral(xx, yy, zz_w, gd, rr, &ww, &m_3);
			double ne4 = galcen(xx, yy, zz);
			double ne5 = gum(xx, yy, zz, &m_5);
			double ne6 = localbubble(xx, yy, zz, gl, gb, &hh, &m_6);
			double ne7 = nps(xx, yy, zz, &WLI, &m_6);
			double WFB = fermibubble(xx, yy, zz);
			
			if(WFB == 1){
				ne1 = t8.J_FB * ne1;
			}
			
			double ne0 = ne1 + MAX(ne2, ne3);

			if(hh > 110){       /* Outside LB */
				if(ne6>ne0 && ne6>ne5){
					WLB=1;
				}
				else{
					WLB=0;
				}
			}
			else{            /* Inside LB */
				if(ne6 > ne0){
					WLB=1;
				}else{
					ne1 = t6.J_LB*ne1;
					ne0 = ne1+MAX(ne2,ne3);
					WLB = 0;
				}
			}
			if (ne7 > ne0) {     /* Loop I */
				WLI = 1;
			} else {
				WLI = 0;
			}        
			if (ne5 > ne0){     /* Gum Nebula */
				WGN = 1;
			}else{
				WGN = 0;
			}
      
			/* Galactic ne */
			ne = (1-WLB) * 
				( (1-WGN) * (
					     (1-WLI) * (ne0+ne4) + WLI * ne7
					    ) + WGN*ne5) + WLB*ne6;
		}
		else if(np==0){
			   double ne8 = lmc(glr,gbr,dist,&w_lmc);
			   double ne9 = dora(glr,gbr,dist);
			   double ne10 = smc(xx, yy, zz,&w_smc);
			/* MC ne */
			ne = ne8 + ne9 + ne10;
		}
		else{
			ne = 0; // ("The point is located outside of MW and MC.\n");
		}
	}
	else{
		ne = 0; // ("The point is located outside of MW and MC.\n");
	}
	return ne;
}

} // namespace hermes
