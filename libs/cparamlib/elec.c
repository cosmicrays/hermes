/*
 * $Source: /home/nkarlsson/usr/cvsroot/cparamlib/cparamlib/elec.c,v $
 * $Author: niklas $ $Date: 2007/12/20 23:43:20 $ $Revision: 1.7 $
 *
 * Change log:
 *
 * 2007-12-17:
 * Modified for documentation generation with doxygen.
 *
 */

/**
 * @file elec.c
 *
 * @brief Functions to calculate electron parameters
 *
 * File provides functions to calculate the parameters describing electron
 * inclusive cross sections. The functions fill struct ::PARAMSET according to
 * the functions listed in Table 4 of Kamae et al. (2006).
 *
 */

#include <stdio.h>
#include <math.h>
#include "cparamlib.h"

/**
 * Calculates parameters a<sub>0</sub>,...,a<sub>8</sub> describing the
 * nondiffraction interaction electron inclusive cross section as a function
 * of the proton kinetic energy T<sub>p</sub>.
 *
 * @param Tp     Proton kinetic energy in GeV.
 * @param params Pointer to a ::PARAMSET struct where the calculated parameters
 *               will be stored.
 */
void elec_param_nd(double Tp, PARAMSET* params)
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
        params->a[0] = z*(-0.018639 + z*(2.4315 + z*(-0.57719 + 0.063435*z)));
        params->a[1] = 7.1827e-6 + y*(-3.5067e-6 + y*(1.3264e-6 + y*(-3.3481e-7 + y*(2.3551e-8 + 3.4297e-9*y))));
        z = y + 7.9031;
        params->a[2] = 563.91 - 362.18*log10(2.7187*(y + 3.4)) - 2.8924e4/(z*z);
        params->a[3] = 0.52684 + y*(0.57717 + y*(0.0045336 - 0.0089066*y));
        z = y + 3.32;
        params->a[4] = z*(0.36108 + z*(1.6963 + z*(-0.074456 + z*(-0.071455 + 0.010473*z))));
        params->a[5] = 9.7387e-5 + 7.8573e-5*log10(0.0036055*(y + 4.3)) + 0.00024660/(y + 4.9390) - 3.8097e-7*y*y;
        params->a[6] = -273.00 - 106.22*log10(0.34100*(y + 3.4)) + 89.037*y - 12.546*y*y;
        z = y + 8.5518;
        params->a[7] = 432.53 - 883.99*log10(0.19737*(y + 3.9)) - 4.1938e4/(z*z);
        params->a[8] = -0.12756 + y*(0.43478 + y*(-0.0027797 - 0.0083074*y));
    } else {
        for (i = 0; i < 9; i++)
            params->a[i] = 0;
    }
}

/**
 * Calculates parameters b<sub>0</sub>,...,b<sub>7</sub> describing the
 * diffraction dissociation electron inclusive cross section as a function
 * of the proton kinetic energy T<sub>p</sub>.
 *
 * @param Tp     Proton kinetic energy in GeV.
 * @param params Pointer to a ::PARAMSET struct where the calculated parameters
 *               will be stored.
 */
void elec_param_diff(double Tp, PARAMSET* params) {
    double y, z1, z2, pow;
    int i;

    /* check whether params is a null pointer or not */
    if (params == NULL)
        return;

    y = log10(Tp*0.001);

    /* 06/06/06: removed unneccessary use of pow() to increase performance
                 also added use of z = y + constant and pow = <expression> */
    if ((Tp > 1.94) && (Tp < 512000.1)){
        if (Tp > 5.51) {
            z1 = y + 1.6878;
            z2 = y + 9.6400;
            params->b[0] = 0.20463*tanh(-6.2370*(y + 2.2)) - 0.16362*z1*z1 + 3.5183e-4*z2*z2*z2*z2;
            pow = (y + 2.0154)/(1.0 + 0.62779*(y + 2.0154));
            params->b[1] = 1.6537 + 3.8530*exp(-3.2027*pow*pow);
            z1 = y + 256.63;
            params->b[2] = -10.722 + 0.082672*tanh(1.8879*(y + 2.1)) + 0.00014895*z1*z1;
            pow = (y + 1.9877)/(1.0 + 0.40300*(y + 1.988));
            params->b[3] = -0.023752 - 0.51734*exp(-3.3087*pow*pow);
        } else {
            params->b[0] = 0;
            params->b[1] = 0;
            params->b[2] = 0;
            params->b[3] = 0;
        }
        z1 = y + 2.9;
        params->b[4] = 0.94921 + 0.12280*z1*z1 - 7.1585e-4*z1*z1*z1*z1 + 0.52130*log10(z1);
        params->b[5] = -4.2295 - 1.0025*tanh(9.0733*(y + 1.9)) - 0.11452*(y - 62.382);
        params->b[6] = 1.4862 + y*(0.99544 + y*(-0.042763 + y*(-0.0040065 + 0.0057987*y)));
        z1 = y - 2.8542;
        params->b[7] = 6.2629 + 6.9517*tanh(-0.36480*(y + 2.1)) - 0.26033*z1*z1;
    } else {
        for (i = 0; i < 8; i++)
            params->b[i] = 0;
    }
}

/**
 * Calculates parameters c<sub>0</sub>,...,c<sub>4</sub> describing the
 * Delta(1232) electron inclusive cross section as a function of the proton
 * kinetic energy T<sub>p</sub>. Since the negative charged Delta(1232)
 * resonance is not formed, no electrons are produced and thus this function
 * only sets the ::PARAMSET array to zero.
 *
 * @param Tp     Proton kinetic energy in GeV.
 * @param params Pointer to a ::PARAMSET struct where the calculated parameters
 *               will be stored.
 */
void elec_param_delta(double Tp, PARAMSET* params)
{
    int i;

    /* check whether params is a null pointer or not */
    if (params == NULL)
        return;

    for (i = 0; i < 5; i++)
        params->c[i] = 0;
}

/**
 * Calculates parameters d<sub>0</sub>,...,d<sub>4</sub> describing the
 * res(1600) electron inclusive cross section as a function of the proton
 * kinetic energy T<sub>p</sub>.
 *
 * @param Tp     Proton kinetic energy in GeV.
 * @param params Pointer to a ::PARAMSET struct where the calculated parameters
 *               will be stored.
 */
void elec_param_res(double Tp, PARAMSET* params)
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
        pow = (y + 2.9537)/(1.0 + 1.5221*(y + 2.9537));
        params->d[0] = 0.37790*exp(-56.826*pow*pow) - 0.059458 + 0.0096583*y*y;
        params->d[1] = -5.5135 - 3.3988*y;
        params->d[2] = -7.1209 - 7.1850*tanh(30.801*(y + 2.1)) + 0.35108*y;
        params->d[3] = -6.7841 - 4.8385*y - 0.91523*y*y;
        params->d[4] = -134.03 - 139.63*y - 48.316*y*y - 5.5526*y*y*y;
    }
}

/**
 * Calculates all parameters a<sub>0</sub>,...,a<sub>8</sub>,
 * b<sub>0</sub>,...,b<sub>7</sub>, c<sub>0</sub>,...,c<sub>4</sub> and
 * d<sub>0</sub>,...,d<sub>4</sub> describing electron inclusive cross
 * sections as a function of the proton kinetic energy.
 *
 * @param Tp     Proton kinetic energy in GeV.
 * @param params Pointer to a ::PARAMSET struct where the calculated parameters
 *               will be stored.
 */
void elec_param(double Tp, PARAMSET* params)
{
    /* check whether params is a null pointer or not */
    if (params == NULL)
        return;

    elec_param_nd(Tp, params);
    elec_param_diff(Tp, params);
    elec_param_delta(Tp, params);
    elec_param_res(Tp, params);
}
