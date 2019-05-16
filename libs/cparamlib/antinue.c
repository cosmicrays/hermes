/*
 * $Source: /home/nkarlsson/usr/cvsroot/cparamlib/cparamlib/antinue.c,v $
 * $Author: niklas $ $Date: 2007/12/20 23:43:20 $ $Revision: 1.8 $
 *
 * Change log:
 *
 * 2007-12-17:
 * Modified for documentation generation with doxygen.
 *
 */

/**
 * @file antinue.c
 *
 * @brief Functions to calculate electron antineutrino parameters
 *
 * File provides functions to calculate the parameters describing electron
 * antineutrino inclusive cross sections. The functions fill struct ::PARAMSET
 * according to the functions listed in Table 4 of Kamae et al. (2006).
 *
 */

#include <stdio.h>
#include <math.h>
#include "cparamlib.h"

/**
 * Calculates parameters a<sub>0</sub>,...,a<sub>8</sub> describing the
 * non-diffraction interaction electron antineutrino inclusive cross section as
 * a function of the proton kinetic energy T<sub>p</sub>.
 *
 * @param Tp     Proton kinetic energy in GeV.
 * @param params Pointer to a ::PARAMSET struct where the calculated parameters
 *               will be stored.
 */
void antinue_param_nd(double Tp, PARAMSET* params)
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
        z = y + 3.31;
        params->a[0] = 0.0013113 + z*(0.36538 + z*(1.5178 + z*(-0.20668 + 0.024255*z)));
        params->a[1] = -4.7833e-6 + 4.5837e-5*exp(-0.42980*(y + 3.4)) + 6.1559e-6/(y + 4.1731) + 1.1928e-6*y;
        params->a[2] = -245.22 + y*(73.223 + y*(-19.652 + y*(0.83138 + 0.71564*y)));
        params->a[3] = 0.45232 + y*(0.52934 + y*(0.010078 - 0.0017092*y));
        z = y + 3.32;
        params->a[4] = -0.0025734 + z*(0.38424 + z*(1.5517 + z*(0.17336 + z*(-0.17160 + 0.021059*z))));
        params->a[5] = 4.7673e-5 + 5.4936e-5*log10(0.0067905*(y + 4.3)) + 0.00020740/(y + 4.9772);
        params->a[6] = -270.30 - 114.47*log10(0.34352*(y + 3.4)) + y*(80.085 - 7.9240*y);
        params->a[7] = 3271.9 - 2.9161e5/(y + 87.847) - 6.2330*y*y;
        params->a[8] = -0.17787 + y*(0.36771 + y*(-0.025397 + y*(0.0019238 + 0.0032725*y)));
    } else {
        for (i = 0; i < 9; i++)
            params->a[i] = 0;
    }
}

/**
 * Calculates parameters b<sub>0</sub>,...,b<sub>7</sub> describing the
 * diffraction dissociation electron antineutrino inclusive cross section as a
 * function of the proton kinetic energy T<sub>p</sub>.
 *
 * @param Tp     Proton kinetic energy in GeV.
 * @param params Pointer to a ::PARAMSET struct where the calculated parameters
 *               will be stored.
 */
void antinue_param_diff(double Tp, PARAMSET* params)
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
        if (Tp > 11.0) {
            z1 = y + 0.55505;
            z2 = y + 9.2685;
            params->b[0] = 41.307*tanh(-0.37411*(y + 2.2)) - 4.1223*z1*z1 + 0.0042652*z2*z2*z2*z2;
            pow = (y + 1.9196)/(1.0 + 11.530*(y + 1.9196));
            params->b[1] = -132.50 + 142.12*exp(-8.0289*pow*pow);
            z1 = y + 250.77;
            params->b[2] = -17.223 + 0.011285*tanh(69.746*(y + 1.9)) - 0.048233*y + 0.00025881*z1*z1;
            pow = (y + 1.9292)/(1.0 + 16.682*(y + 1.9292));
            params->b[3] = 8.1991 - 9.6437*exp(-45.261*pow*pow);
        } else {
            params->b[0] = 0;
            params->b[1] = 0;
            params->b[2] = 0;
            params->b[3] = 0;
        }
        z1 = y + 2.95;
        pow = (y + 2.2) + 0.43867*(y + 2.2)*(y + 2.2);
        params->b[4] = 0.55919 + z1*(0.36647 + 0.056194*z1) + 0.49957*exp(-5.5317*pow*pow);
        params->b[5] = 1.2544 - 0.52362*tanh(2.7638*(y + 1.9)) - 0.055837*(y - 17.638);
        params->b[6] = 1.4788 + y*(1.0278 + y*(-0.092852 + y*(-0.0062734 + 0.011920*y)));
        z1 = y - 2.7889;
        params->b[7] = 5.1651 + 5.7398*tanh(-0.37356*(y + 2.1)) - 0.22234*z1*z1;
    } else {
        for (i = 0; i < 8; i++)
            params->b[i] = 0;
    }
}

/**
 * Calculates parameters c<sub>0</sub>,...,c<sub>4</sub> describing the
 * Delta(1232) electron antineutrino inclusive cross section as a function of
 * the proton kinetic energy T<sub>p</sub>. Since the negative charged
 * Delta(1232) resonance is not formed, no electron antineutrinos are produced
 * and thus this function only sets the ::PARAMSET array to zero.
 *
 * @param Tp     Proton kinetic energy in GeV.
 * @param params Pointer to a ::PARAMSET struct where the calculated parameters
 *               will be stored.
 */
void antinue_param_delta(double Tp, PARAMSET* params)
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
 * red(1600) electron antineutrino inclusive cross section as a function of
 * the proton kinetic energy T<sub>p</sub>.
 *
 * @param Tp     Proton kinetic energy in GeV.
 * @param params Pointer to a ::PARAMSET struct where the calculated parameters
 *               will be stored.
 */
void antinue_param_res(double Tp, PARAMSET* params)
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
        pow = (y + 2.9537)/(1.0 + 1.4320*(y + 2.9537));
        params->d[0] = 0.36459*exp(-58.210*pow*pow) - (0.11283 + 0.046244*y);
        params->d[1] = -9.5066 - y*(5.4655 + 0.31769*y);
        params->d[2] = -7.1831 - 7.1551*tanh(30.354*(y + 2.1)) + 0.33757*y;
        params->d[3] = 2.7938 + y*(1.6992 + 0.20161*y);
        params->d[4] = 0.61878 + y*(0.62371 + y*(0.18913 + 0.019118*y));
    }
}

/**
 * Calculates all parameters a<sub>0</sub>,...,a<sub>8</sub>,
 * b<sub>0</sub>,...,b<sub>7</sub>, c<sub>0</sub>,...,c<sub>4</sub> and
 * d<sub>0</sub>,...,d<sub>4</sub> describing electron antineutrino inclusive
 * cross sections as a function of the proton kinetic energy.
 *
 * @param Tp     Proton kinetic energy in GeV.
 * @param params Pointer to a ::PARAMSET struct where the calculated parameters
 *               will be stored.
 */
void antinue_param(double Tp, PARAMSET* params)
{
    /* check whether params is a null pointer or not */
    if (params == NULL)
        return;

    antinue_param_nd(Tp, params);
    antinue_param_diff(Tp, params);
    antinue_param_delta(Tp, params);
    antinue_param_res(Tp, params);
}
