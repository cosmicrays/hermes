/*
 * posi.c
 *
 * Parameter calculation for positrons
 *
 * $Source: /home/nkarlsson/usr/cvsroot/cparamlib/cparamlib/posi.c,v $
 * $Author: niklas $ $Date: 2007/12/20 23:43:20 $ $Revision: 1.7 $
 *
 * Change log:
 *
 * 2007-12-17:
 * Modified for documentation generation with doxygen.
 *
 */

/**
 * @file posi.c
 *
 * @brief Functions to calculate positron parameters
 *
 * File provides functions to calculate the parameters describing positron
 * inclusive cross sections. The functions fill struct ::PARAMSET according to
 * the functions listed in Table 5 of Kamae et al. (2006).
 *
 */

#include <stdio.h>
#include <math.h>
#include "cparamlib.h"

/**
 * Calculates parameters a<sub>0</sub>,...,a<sub>8</sub> describing the
 * non-diffraction interaction positron inclusive cross section as a function
 * of the proton kinetic energy T<sub>p</sub>.
 *
 * @param Tp     Proton kinetic energy in GeV.
 * @param params Pointer to a ::PARAMSET struct where the calculated parameters
 *               will be stored.
 */
void posi_param_nd(double Tp, PARAMSET* params)
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
        params->a[0] = z*(-0.79606 + z*(7.7496 + z*(-3.9326 + z*(0.80202 - 0.054994*z))));
        params->a[1] = 6.7943e-6 + y*(-3.5345e-6 + y*(6.0927e-7 + y*(2.0219e-7 + y*(5.1005e-8 - 4.2622e-8*y))));
        params->a[2] = 44.827 + 81.378*log10(0.027733*(y + 3.5)) - 1.3886e4/((y + 8.4417)*(y + 8.4417));
        params->a[3] = 0.52010 + y*(0.59336 + y*(0.012032 - 0.0064242*y));
        z = y + 3.32;
        params->a[4] = z*(2.1361 + z*(1.8514 + z*(-0.47872 + z*(0.0032043 + 0.0082955*z))));
        params->a[5] = 1.0845e-6 + 1.4336e-6*log10(0.0077255*(y + 4.3)) + 0.00013018/((y + 4.8188)*(y + 4.8188)) + 9.3601e-8*y;
        params->a[6] = -267.74 + 14.175*log10(0.35391*(y + 3.4)) + y*(64.669 - 7.7036*y);
        params->a[7] = 138.26 - 529.84*log10(0.12467*(y + 3.9)) - 1.9869e4/((y + 7.6884)*(y + 7.6884)) + 1.0675*y*y;
        params->a[8] = -0.14707 + y*(0.40135 + y*(0.0039899 - 0.0016602*y));
    } else {
        for (i = 0; i < 9; i++)
            params->a[i] = 0;
    }
}

/**
 * Calculates parameters b<sub>0</sub>,...,b<sub>7</sub> describing the
 * diffraction dissociation positron inclusive cross section as a function
 * of the proton kinetic energy T<sub>p</sub>.
 *
 * @param Tp     Proton kinetic energy in GeV.
 * @param params Pointer to a ::PARAMSET struct where the calculated parameters
 *               will be stored.
 */
void posi_param_diff(double Tp, PARAMSET* params)
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
            z1 = y + 0.67500;
            z2 = y + 9.0824;
            params->b[0] = 29.192*tanh(-0.37879*(y + 2.2)) - 3.2196*z1*z1 + 3.6687e-3*z2*z2*z2*z2;
            pow = (y + 1.8781)/(1.0 + 3.8389*(y + 1.8781));
            params->b[1] = -142.97 + 147.86*exp(-0.37194*pow*pow);
            z1 = y + 234.65;
            params->b[2] = -14.487 - 4.2223*tanh(-13.546*(y + 2.2)) + 0.00016988*z1*z1;
            pow = (y + 1.8194)/(1.0 + 0.99946*(y + 1.8194));
            params->b[3] = -0.0036974 - 0.41976*exp(-6.1527*pow*pow);
        } else {
            params->b[0] = 0;
            params->b[1] = 0;
            params->b[2] = 0;
            params->b[3] = 0;
        }
        z1 = y + 2.95;
        pow = y + 2.29 - 0.18967*(y + 2.29);
        params->b[4] = 1.8108 + z1*z1*(0.18545 - 2.0049e-3*z1*z1) + 0.85084*exp(-14.987*pow*pow);
        params->b[5] = 2.0404 - 0.51548*tanh(2.2758*(y + 1.9)) - 0.035009*(y - 6.6555);
        params->b[6] = 1.5258 + y*(1.0132 + y*(-0.064388 + y*(-0.0040209 + 0.0082772*y)));
        z1 = y - 2.7718;
        params->b[7] = 3.0551 + 3.5240*tanh(-0.36739*(y + 2.1)) - 0.13382*z1*z1;
    } else {
        for (i = 0; i < 8; i++)
            params->b[i] = 0;
    }
}

/**
 * Calculates parameters c<sub>0</sub>,...,c<sub>4</sub> describing the
 * Delta(1232) positron inclusive cross section as a function of the proton
 * kinetic energy T<sub>p</sub>.
 *
 * @param Tp     Proton kinetic energy in GeV.
 * @param params Pointer to a ::PARAMSET struct where the calculated parameters
 *               will be stored.
 */
void posi_param_delta(double Tp, PARAMSET* params)
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
        pow = ((y + 3.1272)/(1.0 + 0.22831*(y + 3.1272)));
        params->c[0] = 2.9841*exp(-67.857*pow*pow) - (6.5855 + 9.6984/y - 0.41256*y*y);
        params->c[1] = 6.8276 + 5.2236*y + 1.4630*y*y;
        params->c[2] = -6.0291 - 6.4581*tanh(5.0830*(y + 2.1)) + 0.46352*y;
        params->c[3] = 0.59300 + 0.36093*y;
        params->c[4] = 0.77368 + 0.44776*y + 0.056409*y*y;
    }
}

/**
 * Calculates parameters d<sub>0</sub>,...,d<sub>4</sub> describing the
 * res(1600) positron inclusive cross section as a function of the proton
 * kinetic energy T<sub>p</sub>.
 *
 * @param Tp     Proton kinetic energy in GeV.
 * @param params Pointer to a ::PARAMSET struct where the calculated parameters
 *               will be stored.
 */
void posi_param_res(double Tp, PARAMSET* params)
{
    double y, pow;
    int i;

    /* check whether params is a null pointer or not */
    if (params == NULL)
        return;

    y = log10(Tp*0.001);

    /* 06/06/06: removed unneccessary use of pow() to increase performance
                 also added use of pow = <expression> */
    if ((Tp < 0.69) || (Tp >= 2.76)) {
        for (i = 0; i < 5; i++)
            params->d[i] = 0;
    } else {
        pow = ((y + 2.9485)/(1.0 + 1.2892*(y + 2.9485)));
        params->d[0] = 1.9186*exp(-56.544*pow*pow) - (0.23720 - 0.041315*y*y);
        params->d[1] = -4.9866 - 3.1435*y;
        params->d[2] = -7.0550 - 7.2165*tanh(31.033*(y + 2.1)) + 0.38541*y;
        params->d[3] = -2.8915 - 2.1495*y - 0.45006*y*y;
        params->d[4] = -1.2970 - 0.13947*y + 0.41197*y*y + 0.10641*y*y*y;
    }
}

/**
 * Calculates all parameters a<sub>0</sub>,...,a<sub>8</sub>,
 * b<sub>0</sub>,...,b<sub>7</sub>, c<sub>0</sub>,...,c<sub>4</sub> and
 * d<sub>0</sub>,...,d<sub>4</sub> describing positron inclusive cross
 * sections as a function of the proton kinetic energy.
 *
 * @param Tp     Proton kinetic energy in GeV.
 * @param params Pointer to a ::PARAMSET struct where the calculated parameters
 *               will be stored.
 */
void posi_param(double Tp, PARAMSET* params)
{
    /* check whether params is a null pointer or not */
    if (params == NULL)
        return;

    posi_param_nd(Tp, params);
    posi_param_diff(Tp, params);
    posi_param_delta(Tp, params);
    posi_param_res(Tp, params);
}
