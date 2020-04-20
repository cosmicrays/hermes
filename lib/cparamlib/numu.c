/*
 * $Source: /home/nkarlsson/usr/cvsroot/cparamlib/cparamlib/numu.c,v $
 * $Author: niklas $ $Date: 2007/12/20 23:43:20 $ $Revision: 1.8 $
 *
 * Change log:
 *
 * 2007-12-17:
 * Modified for documentation generation with doxygen.
 *
 */

/**
 * @file numu.c
 *
 * @brief Functions to calculate muon neutrino parameters
 *
 * File provides functions to calculate the parameters describing muon
 * neutrino inclusive cross sections. The functions fill struct ::PARAMSET
 * according to the functions listed in Table 6 of Kamae et al. (2006).
 *
 */

#include <stdio.h>
#include <math.h>
#include "cparamlib.h"

/**
 * Calculates parameters a<sub>0</sub>,...,a<sub>8</sub> describing the
 * non-diffraction interaction muon neutrino inclusive cross section as a
 * function of the proton kinetic energy T<sub>p</sub>.
 *
 * @param Tp     Proton kinetic energy in GeV.
 * @param params Pointer to a ::PARAMSET struct where the calculated parameters
 *               will be stored.
 */
void numu_param_nd(double Tp, PARAMSET* params)
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
        params->a[0] = z*(-0.63611 + z*(9.9015 + z*(-4.5897 + z*(0.91778 - 0.060724*z))));
        params->a[1] = 6.8700e-6 + y*(-2.8245e-6 + y*(7.6032e-7 + y*(-3.2953e-7 + 7.4292e-8*y)));
        params->a[2] = -240.46 + y*(58.405 + y*(-9.8556 + y*(3.1401 - 0.88932*y)));
        params->a[3] = 0.49935 + y*(0.60919 + y*(0.0024963 - 0.0099910*y));
        z = y + 3.32;
        params->a[4] = z*(2.5094 + z*(4.1350 + z*(-0.89534 + z*(- 2.7577e-3 + 0.014511*z))));
        z = y + 4.7136;
        params->a[5] = 8.2046e-7 + 1.4085e-6*log10(0.016793*(y + 4.3)) + 0.00013340/(z*z);
        params->a[6] = -267.55 - 0.21018*log10(0.35217*(y + 3.4)) + y*(69.586 - 9.9930*y);
        params->a[7] = 2741.8 + 222.01*log10(9.7401e-13*(y + 3.9)) - 4772.5/(y + 19.773) - 6.1001*y*y;
        params->a[8] = -0.11857 + y*(0.39072 + y*(-0.037813 + y*(0.0022265 + 0.0046931*y)));
    } else {
        for (i = 0; i < 9; i++)
            params->a[i] = 0;
    }
}

/**
 * Calculates parameters b<sub>0</sub>,...,b<sub>7</sub> describing the
 * diffraction dissociation muon neutrino inclusive cross section as a
 * function of the proton kinetic energy T<sub>p</sub>.
 *
 * @param Tp     Proton kinetic energy in GeV.
 * @param params Pointer to a ::PARAMSET struct where the calculated parameters
 *               will be stored.
 */
void numu_param_diff(double Tp, PARAMSET* params)
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
            z1 = y + 0.44754;
            z2 = y + 9.9165;
            params->b[0] = 64.682*tanh(-0.34313*(y + 2.2)) - 5.5955*z1*z1 + 0.0050117*z2*z2*z2*z2;
            pow = (y + 2.3066)/(1.0 + 41.612*(y + 2.3066));
            params->b[1] = -7.6016 + 3042.7*exp(-1.0134e4*pow*pow);
            z1 = y - 1.8861;
            params->b[2] = -1.4978 - 0.58163*tanh(-0.36488*(y + 1.9)) + 0.031825*(y + 2.8097) + 0.022796*z1*z1;
            pow = (y + 3.8835)/(1.0 + 0.53343*(y + 3.8835));
            params->b[3] = -0.0061483 - 65.799*exp(-4.8239*pow*pow);
        } else {
            params->b[0] = 0;
            params->b[1] = 0;
            params->b[2] = 0;
            params->b[3] = 0;
        }
        z1 = y + 2.95;
        pow = y + 2.28 - 0.19149*(y + 2.28)*(y + 2.28);
        params->b[4] = 2.8009 + z1*z1*(0.35351 - 0.0039779*z1*z1) + 1.3012*exp(-10.592*pow*pow);
        params->b[5] = 1.8016 - 0.69847*tanh(2.8627*(y + 1.9)) - 0.015722*(y - 45.410);
        params->b[6] = 1.4617 + y*(1.0167 + y*(-0.078617 + y*(-0.0038336 + 0.010141*y)));
        z1 = y - 2.4209;
        params->b[7] = 3.5599 + 4.0041*tanh(-0.41889*(y + 2.1)) - 0.18182*z1*z1;
    } else {
        for (i = 0; i < 8; i++)
            params->b[i] = 0;
    }
}

/**
 * Calculates parameters c<sub>0</sub>,...,c<sub>4</sub> describing the
 * Delta(1232) muon neutrino inclusive cross section as a function of the proton
 * kinetic energy T<sub>p</sub>.
 *
 * @param Tp     Proton kinetic energy in GeV.
 * @param params Pointer to a ::PARAMSET struct where the calculated parameters
 *               will be stored.
 */
void numu_param_delta(double Tp, PARAMSET* params)
{
    double y, pow;
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
        pow = (y + 3.1278)/(1.0 - 0.19497*(y + 3.1278));
        params->c[0] = 3.6052*exp(-60.914*pow*pow) - (0.92514 - 2.1315/y - 0.23548*y*y);
        params->c[1] = 95.310 + y*(70.497 + 13.636*y);
        params->c[2] = -6.2158 - 6.2939*tanh(21.592*(y + 2.1)) + 0.37440*y;
        params->c[3] = 2.7485 + 1.1692*y;
        params->c[4] = -2.7568 + y*(-1.8461 - 0.31376*y);
    }
}

/**
 * Calculates parameters d<sub>0</sub>,...,d<sub>4</sub> describing the
 * res(1600) muon neutrino inclusive cross section as a function of the proton
 * kinetic energy T<sub>p</sub>.
 *
 * @param Tp     Proton kinetic energy in GeV.
 * @param params Pointer to a ::PARAMSET struct where the calculated parameters
 *               will be stored.
 */
void numu_param_res(double Tp, PARAMSET* params)
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
        pow = (y + 2.9509)/(1.0 + 1.3154*(y + 2.9509));
        params->d[0] = 2.5489*exp(-58.488*pow*pow) - (0.83039 + 0.34412*y);
        params->d[1] = 88.173 + 65.148*y + 12.585*y*y;
        params->d[2] = -7.0962 - 7.1690*tanh(30.890*(y + 2.1)) + 0.38032*y;
        params->d[3] = -4.1440 + y*(-3.2717 - 0.70537*y);
        params->d[4] = 2.2624 + y*(1.1806 + y*(0.0043450 - 0.043020*y));
    }
}

/**
 * Calculates all parameters a<sub>0</sub>,...,a<sub>8</sub>,
 * b<sub>0</sub>,...,b<sub>7</sub>, c<sub>0</sub>,...,c<sub>4</sub> and
 * d<sub>0</sub>,...,d<sub>4</sub> describing muon neutrino inclusive cross
 * sections as a function of the proton kinetic energy.
 *
 * @param Tp     Proton kinetic energy in GeV.
 * @param params Pointer to a ::PARAMSET struct where the calculated parameters
 *               will be stored.
 */
void numu_param(double Tp, PARAMSET* params)
{
    /* check whether params is a null pointer or not */
    if (params == NULL)
        return;

    numu_param_nd(Tp, params);
    numu_param_diff(Tp, params);
    numu_param_delta(Tp, params);
    numu_param_res(Tp, params);
}
