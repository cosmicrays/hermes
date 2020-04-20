/*
 * $Source: /home/nkarlsson/usr/cvsroot/cparamlib/cparamlib/antinumu.c,v $
 * $Author: niklas $ $Date: 2007/12/20 23:43:20 $ $Revision: 1.9 $
 *
 * Change log:
 *
 * 2007-12-17:
 * Modified for documentation generation with doxygen.
 *
 */

/**
 * @file antinumu.c
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
 * non-diffraction interaction muon antineutrino inclusive cross section as
 * a function of the proton kinetic energy T<sub>p</sub>.
 *
 * @param Tp     Proton kinetic energy in GeV.
 * @param params Pointer to a ::PARAMSET struct where the calculated parameters
 *               will be stored.
 */
void antinumu_param_nd(double Tp, PARAMSET* params)
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
        params->a[0] = z*(-1.5243 + z*(10.107 + z*(-4.3126 + z*(0.80081 - 0.048724*z))));
        params->a[1] = -2.6297e-5 + 9.3858e-5*exp(-0.32384*(y + 3.4)) + 7.7821e-6/(y + 4.0560) + 7.6149e-6*y - 8.4091e-7*y*y;
        params->a[2] = -243.62 + 59.374*y - 5.7356*y*y + 1.9815*y*y*y - 1.0478*y*y*y*y;
        params->a[3] = 0.50807 + 0.60221*y + 0.0034120*y*y - 0.011139*y*y*y;
        z = y + 3.32;
        params->a[4] = z*(2.6483 + z*(4.4585 + z*(-1.2744 + z*(0.11659 + 0.0030477*z))));
        z = y + 4.7707;
        params->a[5] = 9.1101e-7 + 1.3880e-6*log10(0.016998*(y + 4.3)) + 0.00012583/(z*z);
        params->a[6] = -272.11 + 53.477*log10(0.35531*(y + 3.4)) + y*(56.041 - 6.0876*y);
        params->a[7] = 6431.8 + 893.92*log10(5.7013e-9*(y + 3.9)) + 2103.6/(y + 5.6740) - 6.1125*y*y;
        params->a[8] = -0.11120 + y*(0.38144 + y*(-0.040128 + y*(0.0047484 + 0.0054707*y)));
    } else {
        for (i = 0; i < 9; i++)
            params->a[i] = 0;
    }
}

/**
 * Calculates parameters b<sub>0</sub>,...,b<sub>7</sub> describing the
 * diffraction dissociation muon antineutrino inclusive cross section as a
 * function of the proton kinetic energy T<sub>p</sub>.
 *
 * @param Tp     Proton kinetic energy in GeV.
 * @param params Pointer to a ::PARAMSET struct where the calculated parameters
 *               will be stored.
 */
void antinumu_param_diff(double Tp, PARAMSET* params)
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
            z1 = y + 0.52273;
            z2 = y + 9.5266;
            params->b[0] = 70.430*tanh(-0.35816*(y + 2.2)) - 6.6796*z1*z1 + 0.0065659*z2*z2*z2*z2;
            pow = (y + 2.2190)/(1.0 + 81.105*(y + 2.2190));
            params->b[1] = -8.1145 + 7686.0*exp(-44046*pow*pow);
            z1 = y - 1.8683;
            params->b[2] = -1.3095 + 0.071270*tanh(0.0075463*(y + 1.9)) + 0.067759*(y + 5.3433) - 0.0044205*z1*z1;
            pow = (y + 2.8363)/(1.0 + 7.0976*(y + 2.8363));
            params->b[3] = 0.082149 - 2190.1*exp(-533.75*pow*pow);
        } else {
            params->b[0] = 0;
            params->b[1] = 0;
            params->b[2] = 0;
            params->b[3] = 0;
        }
        z1 = y + 2.95;
        pow = y + 2.28 - 0.18922*(y + 2.2)*(y + 2.2);
        params->b[4] = 2.7540 + 0.33859*z1*z1 - 0.0034274*z1*z1*z1*z1 + 1.1679*exp(-10.408*pow*pow);
        params->b[5] = 2.1817 - 0.59584*tanh(2.7054*(y + 1.9)) - 0.010909*(y - 14.880);
        params->b[6] = 1.4591 + y*(1.0275 + y*(-0.074949 + y*(-0.0060396 + 0.0097568*y)));
        z1 = y - 2.7653;
        params->b[7] = 3.7609 + 4.2843*tanh(-0.37148*(y + 2.1)) - 0.16479*z1*z1;
    } else {
        for (i = 0; i < 8; i++)
            params->b[i] = 0;
    }
}

/**
 * Calculates parameters d<sub>0</sub>,...,d<sub>4</sub> describing the
 * red(1600) muon antineutrino inclusive cross section as a function of
 * the proton kinetic energy T<sub>p</sub>.
 *
 * @param Tp     Proton kinetic energy in GeV.
 * @param params Pointer to a ::PARAMSET struct where the calculated parameters
 *               will be stored.
 */
void antinumu_param_delta(double Tp, PARAMSET* params)
{
    double y, p;
    int i;

    /* check whether params is a null pointer or not */
    if (params == NULL)
        return;

    y = log10(Tp*0.001);

    /* 06/06/06: removed unneccessary use of pow() to increase performance
                 also added use of pow = <expression> */
    if ((Tp < 0.488) || (Tp > 1.95)) {
        for (i = 0; i < 5; i++)
            params->c[i] = 0;
    } else {
        p = (y + 3.1250)/(1.0 - 0.47567*(y + 3.1250));
        params->c[0] = 2.8262*exp(-62.894*p*p) + 5.6845 + 13.409/y - 0.097296*y*y;
        params->c[1] = 16.721 + 11.750*y + 2.4637*y*y;
        params->c[2] = -6.0557 - 6.3378*tanh(21.984*(y + 2.1)) + 0.43173*y;
        params->c[3] = 0.37009 + 0.27706*y;
        params->c[4] = 0.047507 + 0.061570*y + 0.0070117*y*y;
    }
}

/**
 * Calculates parameters d<sub>0</sub>,...,d<sub>4</sub> describing the
 * red(1600) muon antineutrino inclusive cross section as a function of
 * the proton kinetic energy T<sub>p</sub>.
 *
 * @param Tp     Proton kinetic energy in GeV.
 * @param params Pointer to a ::PARAMSET struct where the calculated parameters
 *               will be stored.
 */
void antinumu_param_res(double Tp, PARAMSET* params)
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
        pow = (y + 2.9492)/(1.0 + 1.2994*(y + 2.9492));
        params->d[0] = 2.2400*exp(-57.159*pow*pow) - (0.66521 + 0.27554*y);
        params->d[1] = -7.0650 - 4.2773*y - 0.17648*y*y;
        params->d[2] = -7.0410 - 7.1977*tanh(31.095*(y + 2.1)) + 0.40238*y;
        params->d[3] = -1.2354 - 0.87581*y - 0.20829*y*y;
        params->d[4] = -0.11395 + 0.34418*y + 0.27103*y*y + 0.050248*y*y*y;
    }
}

/**
 * Calculates all parameters a<sub>0</sub>,...,a<sub>8</sub>,
 * b<sub>0</sub>,...,b<sub>7</sub>, c<sub>0</sub>,...,c<sub>4</sub> and
 * d<sub>0</sub>,...,d<sub>4</sub> describing muon antineutrino inclusive
 * cross sections as a function of the proton kinetic energy.
 *
 * @param Tp     Proton kinetic energy in GeV.
 * @param params Pointer to a ::PARAMSET struct where the calculated parameters
 *               will be stored.
 */
void antinumu_param(double Tp, PARAMSET* params)
{
    /* check whether params is a null pointer or not */
    if (params == NULL)
        return;

    antinumu_param_nd(Tp, params);
    antinumu_param_diff(Tp, params);
    antinumu_param_delta(Tp, params);
    antinumu_param_res(Tp, params);
}
