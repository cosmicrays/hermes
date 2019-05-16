/*
 * $Source: /home/nkarlsson/usr/cvsroot/cparamlib/cparamlib/cparamlib.h,v $
 * $Author: niklas $ $Date: 2007/12/20 23:43:20 $ $Revision: 1.3 $
 *
 * Change log:
 *
 * 2007-12-14:
 * Modified for documentation generation with doxygen.
 *
 * 2007-12-20:
 * Definition of struct PARAMSET_PT changed. Now contains only double arrays.
 * This was done to simplify the documentation. Clutters the code a little bit
 * but the general user of this library should not need to use this struct
 * directly.
 *
 */

/**
 * @file cparamlib.h
 *
 * @brief Header file for cparamlib
 *
 */

#ifndef CPARAMLIB_H
#define CPARAMLIB_H

/* make the library linkable with c++ programs */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Definition of particle ids.
 */
enum PARTICLE_IDS
{
    ID_GAMMA,    /**< Id for gamma ray */
    ID_ELECTRON, /**< Id for electron */
    ID_POSITRON, /**< Id for positron */
    ID_NUE,      /**< Id for electron neutrino */
    ID_NUMU,     /**< Id for muon neutrino */
    ID_ANTINUE,  /**< Id for electron antineutrino */
    ID_ANTINUMU  /**< Id for muon antineutrino */
};

/*
 * struct Definitions
 */

/**
 * c struct containing the four double arrays used for calculating inclusive
 * cross sections, dsigma/dlog(E). Each double array corresponds to a parameter
 * array (a, b, c and d) in Kamae et al. (2006).
 */
typedef struct {
    /**
     * Double array (sizeof = 9) which stores parameters
     * a<sub>0</sub>,...,a<sub>8</sub> describing the inclusive cross section
     * for secondary particles produced by the nondiffractive interaction.
     */
    double a[9];
    /**
     * Double array (sizeof = 8) which stores parameters
     * b<sub>0</sub>,...,b<sub>7</sub> describing the inclusive cross section
     * for secondary particles produced by diffraction dissociation.
     */
    double b[8];
    /**
     * Double array (sizeof = 5) which stores parameters 
     * c<sub>0</sub>,...,c<sub>4</sub> describing the inclusive cross section
     * for secondary particles produced by the Delta(1232) resonance.
     */
    double c[5];
    /**
     * Double array (sizeof = 5) which stores parameters
     * d<sub>0</sub>,...,d<sub>4</sub> describing the inclusive cross section
     * for secondary particles produced by the res(1600) resonance.
     */
    double d[5];
} PARAMSET;

/** 
 * c struct containing doubles and double arrays used for calculating pT
 * distributions, i.e. the differential cross section
 * d<sup>2</sup>sigma/dlog(E)dp<sub>t</sub>,
 * as defined in Karlsson and Kamae (2008).
 *
 * Doubles correspond to parameters describing the differential cross
 * section as functions of the secondary particle energy (E) and and
 * transverse momentum (pT). Double arrays describe above parameters as
 * functions of proton kinetic energy (Tp).
 */
typedef struct {
    /**
     * Double array (sizeof = 6) which stores parameters a<sub>0</sub>,
     * a<sub>1</sub> and a<sub>10</sub>,...,a<sub>13</sub> (in that order)
     * describing the p<sub>t</sub> distribution for gamma rays produced by
     * nonresonance interaction.
     */
    double a[6];
    /**
     * Array of three double arrays (sizeof = 3x4) which stores parameters
     * b<sub>0</sub>, b<sub>1</sub>, b<sub>2</sub>,
     * b<sub>10</sub>,...,b<sub>13</sub> and b<sub>20</sub>,...,b<sub>23</sub>
     * describing the p<sub>t</sub> distribution for gamma rays produced by
     * the Delta(1232) resonance.
     *
     * Layout:
     * - b[0][..] = b<sub>0</sub>, b<sub>1</sub>, b<sub>2</sub>
     * - b[1][..] = b<sub>10</sub>,...,b<sub>13</sub>
     * - b[2][..] = b<sub>20</sub>,...,b<sub>23</sub>
     *
     * The element b[0][3] is not used and always zero.
     */
    double b[3][4];
    /**
     * Array of three double arrays (sizeof = 3x4) which stores parameters
     * c<sub>0</sub>, c<sub>1</sub>, c<sub>2</sub>,
     * c<sub>10</sub>,...,c<sub>13</sub> and c<sub>20</sub>,...,c<sub>23</sub>
     * describing the p<sub>t</sub> distribution for gamma rays produced by
     * the Delta(1232) resonance.
     *
     * Layout:
     * - c[0][..] = c<sub>0</sub>, c<sub>1</sub>, c<sub>2</sub>
     * - c[1][..] = c<sub>10</sub>,...,c<sub>13</sub>
     * - c[2][..] = c<sub>20</sub>,...,c<sub>23</sub>
     *
     * The element c[0][3] is not used and always zero.
     */
    double c[3][4];

    PARAMSET params;
} PARAMSET_PT;

/* 
 * API Definitions
 *  
 * Listed below are all the functions implemented in this library
 */

/*
 * Electron antineutrinos
 */
void antinue_param_nd(double Tp, PARAMSET* params);
void antinue_param_diff(double Tp, PARAMSET* params);
void antinue_param_delta(double Tp, PARAMSET* params);
void antinue_param_res(double Tp, PARAMSET* params);
void antinue_param(double Tp, PARAMSET* params);

/*
 * Muon antineutrinos
 */
void antinumu_param_nd(double Tp, PARAMSET* params);
void antinumu_param_diff(double Tp, PARAMSET* params);
void antinumu_param_delta(double Tp, PARAMSET* params);
void antinumu_param_res(double Tp, PARAMSET* params);
void antinumu_param(double Tp, PARAMSET* params);

/*
 * Electrons
 */
void elec_param_nd(double Tp, PARAMSET* params);
void elec_param_diff(double Tp, PARAMSET* params);
void elec_param_delta(double Tp, PARAMSET* params);
void elec_param_res(double Tp, PARAMSET* params);
void elec_param(double Tp, PARAMSET* params);

/*
 * Gamma rays
 */
void gamma_param_nd(double Tp, PARAMSET* params);
void gamma_param_diff(double Tp, PARAMSET* params);
void gamma_param_delta(double Tp, PARAMSET* params);
void gamma_param_res(double Tp, PARAMSET* params);
void gamma_param(double Tp, PARAMSET* params);

void gamma_pt_param_nr(double E, double Tp, PARAMSET_PT* pt_params, int flag);
void gamma_pt_param_delta(double E, double Tp, PARAMSET_PT* pt_params, int flag);
void gamma_pt_param_res(double E, double Tp, PARAMSET_PT* pt_params, int flag);
void gamma_pt_param(double E, double Tp, PARAMSET_PT* pt_params, int flag);

/*
 * Muon neutrinos
 */
void numu_param_nd(double Tp, PARAMSET* params);
void numu_param_diff(double Tp, PARAMSET* params);
void numu_param_delta(double Tp, PARAMSET* params);
void numu_param_res(double Tp, PARAMSET* params);
void numu_param(double Tp, PARAMSET* params);

/*
 * Electron neutrinos
 */
void nue_param_nd(double Tp, PARAMSET* params);
void nue_param_diff(double Tp, PARAMSET* params);
void nue_param_delta(double Tp, PARAMSET* params);
void nue_param_res(double Tp, PARAMSET* params);
void nue_param(double Tp, PARAMSET* params);

/*
 * Positrons
 */
void posi_param_nd(double Tp, PARAMSET* params);
void posi_param_diff(double Tp, PARAMSET* params);
void posi_param_delta(double Tp, PARAMSET* params);
void posi_param_res(double Tp, PARAMSET* params);
void posi_param(double Tp, PARAMSET* params);

/*
 * Calculation of the inclusive cross section, dsigma/dlogE, for any given secondary
 * particle and any given energy of that secondary particle
 *
 * Proton kinetic energy Tp is passed as well as a pointer to a struct holding the
 * parameters (at that Tp)
 */
double sigma_incl_delta(int particle, double E, double Tp, PARAMSET* params);
double sigma_incl_diff(int particle, double E, double Tp, PARAMSET* params);
double sigma_incl_nd(int particle, double E, double Tp, PARAMSET* params);
double sigma_incl_res(int particle, double E, double Tp, PARAMSET* params);
double sigma_incl_tot(int particle, double E, double Tp, PARAMSET* params);

/*
 * Calculation of the p-p cross section for a given proton momentum
 */
double sigma_pp_delta(double Pp);
double sigma_pp_diff(double Pp);
double sigma_pp_nd(double Pp);
double sigma_pp_res(double Pp);

/*
 * Calculation of the differential cross section dsigma/dlogEdpT, for any gamma-ray energy
 *
 * Proton kinetic energy Tp is passed as well as a pointer to a struct keeping all the 
 * parameters (at given Tp and log(E))
 */
double sigma_pt_delta(int particle, double pT, double E, double Tp, PARAMSET_PT* pt_params);
double sigma_pt_nr(int particle, double pT, double E, double Tp, PARAMSET_PT* pt_params);
double sigma_pt_res(int particle, double pT, double E, double Tp, PARAMSET_PT* pt_params);
double sigma_pt_tot(int particle, double pT, double E, double Tp, PARAMSET_PT* pt_params);

#ifdef __cplusplus
}
#endif

#endif
