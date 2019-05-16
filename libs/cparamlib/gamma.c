/*
 * $Source: /home/nkarlsson/usr/cvsroot/cparamlib/cparamlib/gamma.c,v $
 * $Author: niklas $ $Date: 2007/12/20 23:43:20 $ $Revision: 1.11 $
 *
 * Change log:
 *
 * 2007-12-17:
 * Modified for documentation generation with doxygen.
 *
 * 2007-12-20:
 * The definition of struct PARAMSET_PT was changed in header file. Code updated
 * to reflect that change. Also added Doxygen comments for the functions
 * calculating pT parameters.
 *
 */

/**
 * @file gamma.c
 *
 * @brief Functions to calculate gamma-ray parameters
 *
 * File provides functions to calculate the parameters describing gamma-ray
 * inclusive cross sections and transverse momentum distributions. The functions
 * fill struct ::PARAMSET according to the functions listed in Table 3 of Kamae
 * et al. (2006) or struct ::PARAMSET_PT according to Table 1 of Karlsson and
 * Kamae (2008).
 *
 */

#include <stdio.h>
#include <math.h>
#include "cparamlib.h"

/**
 * Calculates parameters a<sub>0</sub>,...,a<sub>8</sub> describing the
 * nondiffraction interaction gamma-ray inclusive cross section as a function
 * of the proton kinetic energy T<sub>p</sub>.
 *
 * @param Tp     Proton kinetic energy in GeV.
 * @param params Pointer to a ::PARAMSET struct where the calculated parameters
 *               will be stored.
 */
void gamma_param_nd(double Tp, PARAMSET* params)
{
    double y, z;
    int i;

    /* check whether params is a null pointer or not */
    if (params == NULL)
        return;

    y = log10(Tp*0.001);

    /* 06/06/06: removed unneccessary use of pow() to increase performance
                 also added use of z = y + constant */
    if ((Tp > 0.487) && (Tp < 512000.1)) {
        z = y + 3.3;
        params->a[0] = z*(-0.51187 + z*(7.6179 + z*(-2.1332 + 0.22184*z)));
        params->a[1] = -1.2592e-5 + 1.4439e-5*exp(-0.29360*(y + 3.4)) + 5.9363e-5/(y + 4.1485) + y*(2.2640e-6 - 3.3723e-7*y);
        params->a[2] = -174.83 + 152.78*log10(1.5682*(y + 3.4)) - 808.74/(y + 4.6157);
        params->a[3] = 0.81177 + y*(0.56385 + y*(0.0040031 + y*(-0.0057658 + 0.00012057*y)));
        z = y + 3.32;
        params->a[4] = z*(0.68631 + z*(10.145 + z*(-4.6176 + z*(0.86824 - 0.053741*z))));
        z = y + 4.7171;
        params->a[5] = 9.0466e-7 + 1.4539e-6*log10(0.015204*(y + 3.4)) + 0.00013253/(z*z) + y*(-4.1228e-7 + 2.2036e-7*y);
        params->a[6] = -339.45 + 618.73*log10(0.31595*(y + 3.9)) + 250.20/((y + 4.4395)*(y + 4.4395));
        params->a[7] = -35.105 + y*(36.167 + y*(-9.3575 + 0.33717*y));
        params->a[8] = 0.17554 + y*(0.37300 + y*(-0.014938 + y*(0.0032314 + 0.0025579*y)));
    } else {
        for (i = 0; i < 9; i++)
            params->a[i] = 0;
    }
}

/**
 * Calculates parameters b<sub>0</sub>,...,b<sub>7</sub> describing the
 * diffraction dissociation gamma-ray inclusive cross section as a function
 * of the proton kinetic energy T<sub>p</sub>.
 *
 * @param Tp     Proton kinetic energy in GeV.
 * @param params Pointer to a ::PARAMSET struct where the calculated parameters
 *               will be stored.
 */
void gamma_param_diff(double Tp, PARAMSET* params)
{
    double y, z1, z2, pow;
    int i;

    /* check whether params is a null pointer or not */
    if (params == NULL)
        return;

    y = log10(Tp*0.001);

    /* 06/06/06: removed unneccessary use of pow() to increase performance
                 also added use of z = y + constant and pow = <expression> */
    if ((Tp > 1.94) && (Tp < 512000.1)) {
        if (Tp > 5.51) {
            z1 = y + 0.59913;
            z2 = y + 9.4773;
            params->b[0] = 60.142*tanh(-0.37555*(y + 2.2)) - 5.9564*z1*z1 + 0.0060162*z2*z2*z2*z2;
            z1 = y + 369.13;
            params->b[1] = 35.322 + 3.8026*tanh(-2.4979*(y + 1.9)) - 0.00021870*z1*z1;
            z1 = y + 252.43;
            params->b[2] = -15.732 - 0.082064*tanh(-1.9621*(y + 2.1)) + 0.00023355*z1*z1;
            pow = (y + 1.0444)/(1.0 + 0.27437*(y + 1.0444));
            params->b[3] = -0.086827 + 0.37646*exp(-0.53053*pow*pow);
        } else {
            params->b[0] = 0;
            params->b[1] = 0;
            params->b[2] = 0;
            params->b[3] = 0;
        }
        z1 = y + 2.95;
        pow = (y + 2.45) - 0.19717*(y + 2.45)*(y + 2.45);
        params->b[4] = 2.5982 + 0.39131*z1*z1 - 0.0049693*z1*z1*z1*z1 + 0.94131*exp(-24.347*pow*pow);
        z1 = (y - 0.83562)/(1.0 + 0.33933*(y - 0.83562));
        params->b[5] = 0.11198 + y*(-0.64582 + 0.16114*y) + 2.2853*exp(-0.0032432*z1*z1);
        params->b[6] = 1.7843 + y*(0.91914 + y*(0.050118 + y*(0.038096 + y*(-0.027334 + y*(-0.0035556 + 0.0025742*y)))));
        z1 = y + 1.8441;
        params->b[7] = -0.19870 + y*(-0.071003 + 0.019328*y) - 0.28321*exp(-6.0516*z1*z1);
    } else {
        for (i = 0; i < 8; i++)
            params->b[i] = 0;
    }
}

/**
 * Calculates parameters c<sub>0</sub>,...,c<sub>4</sub> describing the
 * Delta(1232) gamma-ray inclusive cross section as a function of the proton
 * kinetic energy T<sub>p</sub>.
 *
 * @param Tp     Proton kinetic energy in GeV.
 * @param params Pointer to a ::PARAMSET struct where the calculated parameters
 *               will be stored.
 */
void gamma_param_delta(double Tp, PARAMSET* params)
{
    double y, pow;
    int i;

    /* check whether params is a null pointer or not */
    if (params == NULL)
        return;

    y = log10(Tp*0.001);

    /* 06/06/06: removed unneccessary use of pow() to increase performance
                 also added use of pow = <expression> */
    if ((Tp < 0.488) || (Tp > 1.95))
        for (i = 0; i < 5; i++)
            params->c[i] = 0;
    else {
        pow = ((y + 3.1301)/(1.0 + 0.14921*(y + 3.1301)));
        params->c[0] = 2.4316*exp(-69.484*pow*pow) - (6.3003 + 9.5349/y - 0.38121*y*y);
        params->c[1] = 56.872 + y*(40.627 + 7.7528*y);
        params->c[2] = -5.4918 - 6.7872*tanh(4.7128*(y + 2.1)) + 0.68048*y;
        params->c[3] = -0.36414 + 0.039777*y;
        params->c[4] = -0.72807 + y*(-0.48828 - 0.092876*y);
    }
}

/**
 * Calculates parameters d<sub>0</sub>,...,d<sub>4</sub> describing the
 * res(1600) gamma-ray inclusive cross section as a function of the proton
 * kinetic energy T<sub>p</sub>.
 *
 * @param Tp     Proton kinetic energy in GeV.
 * @param params Pointer to a ::PARAMSET struct where the calculated parameters
 *               will be stored.
 */
void gamma_param_res(double Tp, PARAMSET* params)
{
    double y, pow;
    int i;

    /* check whether params is a null pointer or not */
    if (params == NULL)
        return;

    y = log10(Tp*0.001);

    /* 06/06/06: removed unneccessary use of pow() to increase performance
                 also added use of pow = <expression> */
    if ((Tp < 0.69) || (Tp > 2.76)) {
        for (i = 0; i < 5; i++)
            params->d[i] = 0;
    } else {
        pow = ((y + 2.9507)/(1.0 + 1.2912*(y + 2.9507)));
        params->d[0] = 3.2433*exp(-57.133*pow*pow) - (1.0640 + 0.43925*y);
        params->d[1] = 16.901 + y*(5.9539 + y*(-2.1257 - 0.92057*y));
        params->d[2] = -6.6638 - 7.5010*tanh(30.322*(y + 2.1)) + 0.54662*y;
        params->d[3] = -1.50648 + y*(-0.87211 - 0.17097*y);
        params->d[4] = 0.42795 + y*(0.55136 + y*(0.20707 + 0.027552*y));
    }
}

/**
 * Calculates all parameters a<sub>0</sub>,...,a<sub>8</sub>,
 * b<sub>0</sub>,...,b<sub>7</sub>, c<sub>0</sub>,...,c<sub>4</sub> and
 * d<sub>0</sub>,...,d<sub>4</sub> describing gamma-ray inclusive cross
 * sections as a function of the proton kinetic energy.
 *
 * @param Tp     Proton kinetic energy in GeV.
 * @param params Pointer to a ::PARAMSET struct where the calculated parameters
 *               will be stored.
 */
void gamma_param(double Tp, PARAMSET* params)
{
    /* check whether params is a null pointer or not */
    if (params == NULL)
        return;

    gamma_param_nd(Tp, params);
    gamma_param_diff(Tp, params);
    gamma_param_delta(Tp, params);
    gamma_param_res(Tp, params);
}

/**
 * Calculates parameters a<sub>0</sub>, a<sub>1</sub> and
 * a<sub>10</sub>,...,a<sub>13</sub> describing the nonresonance gamma-ray
 * p<sub>t</sub> distribution as a function of the proton kinetic energy
 * T<sub>p</sub> and the gamma-ray energy E.
 *
 * @param E         Gamma-ray energy in GeV.
 * @param Tp        Proton kinetic energy in GeV.
 * @param pt_params Pointer to a ::PARAMSET_PT struct where the calculated
 *                  parameters will be stored.
 * @param flag      Flag if Tp has changed since last call to this function. If
 *                  set to 0 then only a<sub>0</sub> and a<sub>1</sub> are
 *                  recalculated using the new gamma-ray energy E.
 */
void gamma_pt_param_nr(double E, double Tp, PARAMSET_PT* pt_params, int flag)
{
    double a14;
    double x, xp, xa, pow;
    double y;
    double sigma_incl;
    int i;

    /* check whether pt_params is a null pointer or not */
    if (pt_params == NULL)
        return;

    x = log10(E);
    y = log10(Tp*0.001);

    if ((Tp > 0.579) && (Tp < 512000.1)) {
        /* first calculate a10,...,a14 using functions from Table 1 
           but only do this if flag = 1 */
        if (flag == 1) {
            pt_params->a[2] = 0.043775 + 0.010271*exp(-0.55808*y);
            pt_params->a[3] = 0.8;
            pt_params->a[4] = 0.34223 + 0.027134*y - 0.0089229*y*y + 4.9996e-4*y*y*y;
            pt_params->a[5] = -0.20480 + 0.013372*y + 0.13087*exp(0.0044021*(y - 11.467)*(y - 11.467));

            gamma_param_nd(Tp, &(pt_params->params));
            gamma_param_diff(Tp, &(pt_params->params));
        }

        /* then calculate a1 using a10,...,a14; when x > -0.75 coefficent a14 = a1(x = -0.75) */
        if (x > -0.75) {
            xa = -0.75 + pt_params->a[4];
            a14 = pt_params->a[2]*exp(-pt_params->a[3]*xa*xa);
            
            pt_params->a[1] = pt_params->a[5]*(x + 0.75) + a14;
        } else {
            xa = x + pt_params->a[4];
            pt_params->a[1] = pt_params->a[2]*exp(-pt_params->a[3]*xa*xa);
        }

        sigma_incl = sigma_incl_nd(ID_GAMMA, E, Tp, &(pt_params->params));
        sigma_incl += sigma_incl_diff(ID_GAMMA, E, Tp, &(pt_params->params));

        /* finally calculate a0 from a1 and sigma_incl*/
        if (pt_params->a[1] != 0) {
            pt_params->a[0] = 1/(pt_params->a[1]*pt_params->a[1])*sigma_incl;
        } else {
            pt_params->a[0] = 0;
        }
    } else {
        for (i = 0; i < 6; i++) {
            pt_params->a[i] = 0;
        }
    }
}

/**
 * Calculates parameters b<sub>0</sub>, b<sub>1</sub>, b<sub>2</sub>,
 * b<sub>10</sub>,...,b<sub>13</sub> and b<sub>20</sub>,...,b<sub>23</sub>
 * describing the Delta(1232) gamma-ray p<sub>t</sub> distribution as a
 * function of the proton kinetic energy T<sub>p</sub> and the gamma-ray
 * energy E.
 *
 * @param E         Gamma-ray energy in GeV.
 * @param Tp        Proton kinetic energy in GeV.
 * @param pt_params Pointer to a ::PARAMSET_PT struct where the calculated
 *                  parameters will be stored.
 * @param flag      Flag if Tp has changed since last call to this function. If
 *                  set to 0 then only b<sub>0</sub>,...,b<sub>2</sub> are
 *                  recalculated using the new gamma-ray energy E.
 */
void gamma_pt_param_delta(double E, double Tp, PARAMSET_PT* pt_params, int flag)
{
    double x, xb;
    double y;
    double A;
    double pow;
    double sigma_incl;
    int i,j;

    /* check whether pt_params is a null pointer or not */
    if (pt_params == NULL)
        return;

    x = log10(E);
    y = log10(Tp*0.001);

    if ((Tp < 0.488) || (Tp > 1.95)) {
        for (j = 0; j < 3; j++) {
            for (i = 0; i < 4; i++) {
                pt_params->b[j][i] = 0;
                pt_params->b[j][i] = 0;
            }
        }
    } else {
        /* first calculate b10,...,b14 and b20,...,b24 using functions from Table 1
           but only do this if flag = 1 */
        if (flag == 1) {
            pt_params->b[1][0] = 18.712 + 18.030*y + 5.8239*y*y + 0.62728*y*y*y;
            pt_params->b[1][1] = 612.61 + 404.80*y + 67.406*y*y;
            pt_params->b[1][2] = 98.639 + 96.741*y + 31.597*y*y + 3.4567*y*y*y;
            pt_params->b[1][3] = -208.38 - 183.65*y - 53.283*y*y - 5.0470*y*y*y;

            pt_params->b[2][0] = 0.21977 + 0.064073*y;
            pt_params->b[2][1] = 3.3187e3 + 3.4634e3*y + 1.1982e3*y*y + 136.71*y*y*y;
            pt_params->b[2][2] = 91.410 + 91.613*y + 30.621*y*y + 3.4296*y*y*y;
            pt_params->b[2][3] = -521.40 - 529.06*y - 178.49*y*y - 19.975*y*y*y;

            gamma_param_delta(Tp, &(pt_params->params));
        }

        /* then calculate b1 and b2 using b10,...,b24
           when x > 0.5 bi = 0; i=0,1,2 */
        A = 0.81*(y + 3.32) - 0.5;
        if (x < A) {
            xb = x - pt_params->b[1][2];
            pow = xb/(1.0 + pt_params->b[1][3]*xb);
            pt_params->b[0][1] = pt_params->b[1][0]*exp(-pt_params->b[1][1]*pow*pow);
            
            xb = x - pt_params->b[2][2];
            pow = xb/(1.0 + pt_params->b[2][3]*xb);
            pt_params->b[0][2] = pt_params->b[2][0]*exp(-pt_params->b[2][1]*pow*pow);

            sigma_incl = sigma_incl_delta(ID_GAMMA, E, Tp, &(pt_params->params));

            /* calculate b0 from b1 and b2 */
            if ((pt_params->b[0][1] != 0) && (pt_params->b[0][2] != 0)) {
                pt_params->b[0][0] = 2*sigma_incl/(pt_params->b[0][1]*sqrt(M_PI*pt_params->b[0][2])*(erf(pt_params->b[0][1]/sqrt(pt_params->b[0][2])) + 1) + 
                                                   pt_params->b[0][2]*exp(-pt_params->b[0][1]*pt_params->b[0][1]/pt_params->b[0][2]));
            } else {
                pt_params->b[0][0] = 0;
            }
        } else {
            for (i = 0; i < 4; i++) {
                pt_params->b[0][i] = 0;
            }
        }
    }
}

/**
 * Calculates parameters c<sub>0</sub>, c<sub>1</sub>, c<sub>2</sub>,
 * c<sub>10</sub>,...,c<sub>13</sub> and c<sub>10</sub>,...,c<sub>13</sub>
 * describing the res(1600) gamma-ray p<sub>t</sub> distribution as a
 * function of the proton kinetic energy T<sub>p</sub> and the gamma-ray
 * energy.
 *
 * @param E         Gamma-ray energy in GeV.
 * @param Tp        Proton kinetic energy in GeV.
 * @param pt_params Pointer to a ::PARAMSET_PT struct where the calculated
 *                  parameters will be stored.
 * @param flag      Flag if Tp has changed since last call to this function. If
 *                  set to 0 then only c<sub>0</sub>,...,c<sub>2</sub> are
 *                  recalculated using the new gamma-ray energy E.
 */
void gamma_pt_param_res(double E, double Tp, PARAMSET_PT* pt_params, int flag)
{
    double x, xc;
    double y;
    double A;
    double pow;
    double sigma_incl;
    int i,j;

    /* check whether pt_params is a null pointer or not */
    if (pt_params == NULL)
        return;

    x = log10(E);
    y = log10(Tp*0.001);

    if ((Tp < 0.69) || (Tp > 2.76)) {
        for (j = 0; j < 3; j++) {
            for (i = 0; i < 4; i++) {
                pt_params->c[j][i] = 0;
                pt_params->c[j][i] = 0;
            }
        }
    } else {
        /* first calculate c10,...,c14 and c20,...,c24 using functions from Table 1
           but only do this if flag = 1 */
        if (flag == 1) {
            pt_params->c[1][0] = -1.5013 - 1.1281*y - 0.19813*y*y;
            pt_params->c[1][1] = -33.179 - 22.496*y - 3.3108*y*y;
            pt_params->c[1][2] = 116.44 + 122.11*y + 42.594*y*y + 4.9609*y*y*y;
            pt_params->c[1][3] = -545.77 - 574.80*y - 201.25*y*y - 23.400*y*y*y;
            
            pt_params->c[2][0] = 0.68849 + 0.36438*y + 0.047958*y*y;
            pt_params->c[2][1] = -1.6871e4 - 1.7412e4*y - 5.9648e3*y*y - 679.27*y*y*y;
            pt_params->c[2][2] = -88.565 - 94.034*y - 33.014*y*y - 3.8205*y*y*y;
            pt_params->c[2][3] = 1.5141e3 + 1.5757e3*y + 544.20*y*y + 62.446*y*y*y;

            gamma_param_res(Tp, &(pt_params->params));
        }

        /* then calculate c1 and c2 using c10,...,c24
           when x > 0.5 ci = 0; i=0,1,2 */
        A = 0.82*(x + 3.17) - 0.25;
        if (x < 0.5) {
            xc = x - pt_params->c[1][2];
            pow = xc/(1.0 + pt_params->c[1][3]*xc);
            pt_params->c[0][1] = pt_params->c[1][0]*exp(-pt_params->c[1][1]*pow*pow);
            
            xc = x - pt_params->c[2][2];
            pow = xc/(1.0 + pt_params->c[2][3]*xc);
            pt_params->c[0][2] = pt_params->c[2][0]*exp(-pt_params->c[2][1]*pow*pow);

            sigma_incl = sigma_incl_res(ID_GAMMA, E, Tp, &(pt_params->params));

            /* calculate c0 from c1 and c2 */
            if ((pt_params->c[0][1] != 0) && (pt_params->c[0][2] != 0)) {
                pt_params->c[0][0] = 2*sigma_incl/(pt_params->c[0][1]*sqrt(M_PI*pt_params->c[0][2])*(erf(pt_params->c[0][1]/sqrt(pt_params->c[0][2])) + 1) + 
                                              pt_params->c[0][2]*exp(-pt_params->c[0][1]*pt_params->c[0][1]/pt_params->c[0][2]));
            } else {
                pt_params->c[0][0] = 0;
            }
        } else {
            for (i = 0; i < 4; i++) {
                pt_params->c[0][i] = 0;
            }
        }
    }
}

/**
 * Calculates all parameters describing the gamma-ray p<sub>t</sub> distribution
 * as a function of the proton kinetic energy T<sub>p</sub> and the gamma-ray
 * energy.
 *
 * @param E         Gamma-ray energy in GeV.
 * @param Tp        Proton kinetic energy in GeV.
 * @param pt_params Pointer to a ::PARAMSET_PT struct where the calculated
 *                  parameters will be stored.
 * @param flag      Flag if Tp has changed since last call to this function.
 */
void gamma_pt_param(double E, double Tp, PARAMSET_PT* pt_params, int flag)
{
    /* check whether params is a null pointer or not */
    if (pt_params == NULL)
        return;

    gamma_pt_param_nr(E, Tp, pt_params, flag);
    gamma_pt_param_delta(E, Tp, pt_params, flag);
    gamma_pt_param_res(E, Tp, pt_params, flag);
}
