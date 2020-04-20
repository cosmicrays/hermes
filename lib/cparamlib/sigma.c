/*
 * $Source: /home/nkarlsson/usr/cvsroot/cparamlib/cparamlib/sigma.c,v $
 * $Author: niklas $ $Date: 2008/01/16 18:35:27 $ $Revision: 1.12 $
 *
 * Change log:
 *
 * 2006-10-11:
 * On request we have added functions for calculating sigma_pp for the for
 * components as given in the paper. We also changed sigma to sigma_incl.
 *
 * 2007-05-01:
 * Functionality to calculate pT distributions added.
 *
 * 2007-05-22:
 * Changed all functions to take a pointer to a struct containing parameters
 * rather than taking pointers to double arrays.
 *
 * 2007-12-14:
 * Modified for documentation generation with doxygen.
 *
 * 2007-12-20:
 * Modified to reflect change of PARAMSET_PT definition.
 *
 */

/**
 * @file sigma.c
 *
 * @brief Main part of cparamlib; functions to calculate of differential
 * cross sections. 
 *
 * File provides functions for the calculation of inclusive cross sections for
 * stable secondary particles (gamma rays, electrons, positrons, electron
 * neutrinos, electron antineutrinos, muon neutrinos and muon antineturinos)
 * and transverse momentum distributions for gamma rays, as described in
 * <a href="http://www.arxiv.org/abs/astro-ph/0605581">Kamae et al. (2006)</a>
 * and
 * <a href="http://www.arxiv.org/abs/0709.0233">Karlsson and Kamae (2008)</a>.
 * It also provides functions for the calculation of the inelastic
 * proton-proton cross section given in
 * <a href="http://www.arxiv.org/abs/astro-ph/0605581">Kamae et al. (2006)</a>.
 *
 */

#include <stdio.h>
#include <math.h>
#include "cparamlib.h"

/* 
 * Coefficients for the kinematic cutoff functions as given in Table 2 of
 * Kamae et al. (2006)
 *
 */
double L_MAX[7] = {0.96, 0.96, 0.94, 0.98, 0.98, 0.94, 0.98};
double W_NDL[7] = {15.0, 20.0, 15.0, 15.0, 15.0, 20.0, 15.0};
double W_NDH[7] = {44.0, 45.0, 47.0, 42.0, 40.0, 45.0, 40.0};

/**
 * Calculates the inclusive cross section for the non-diffractive interaction
 * (equation 6 of Kamae et al. 2006). The inclusive cross section is defined as
 * dsigma/dlog(E) and returned in the units of mb.
 *
 * @param particle Secondary particle id number (see cparamlib.h).
 * @param E        Secondary particle energy in GeV.
 * @param Tp       Proton kinetic energy in GeV.
 * @param params   Pointer to a ::PARAMSET struct. The struct should be
 *                 initialized before being passed to this function.
 * @return         Inclusive cross section dsigma/dlogE in mb.
 *
 */
double sigma_incl_nd(int particle, double E, double Tp, PARAMSET* params)
{
    double Wl, Wh, Lmin, Lmax;
    double x, y;
    double xa3, xa8;
    double pow1, pow2;
    double sigma;
    double cutoff, r_factor;

    /* check whether params is a null pointer or not */
    if (params == NULL)
        return 0;

    /* calculate log(E) and log(Tp) */
    x = log10(E);
    y = log10(Tp*0.001);

    /* init some variables, given in table 2 */
    Lmin = -2.6;
    Lmax = L_MAX[particle]*(y + 3.0);
    Wl = W_NDL[particle];
    Wh = W_NDH[particle];

    /* calculate the flux due to non-diffractive process for given gamma-ray energy */
    /* 06/12/06: replaced code involving pow() */
    xa3 = x - params->a[3];
    pow1 = xa3*(1 + params->a[2]*xa3);
    xa8 = x - params->a[8];
    pow2 = xa8*(1 + xa8*(params->a[6] + params->a[7]*xa8));
    sigma = params->a[0]*exp(-params->a[1]*pow1*pow1) + params->a[4]*exp(-params->a[5]*pow2*pow2);

    /* cutoff is the kinematic limit function as in the paper */
    cutoff = (1/(1 + exp(Wl*(Lmin - x))))*(1/(1 + exp(Wh*(x - Lmax))));   
    sigma = sigma*cutoff;
    
    if (sigma < 0)
        sigma = 0;
    
    /* renormalization
       this is different for each particle, thus we must use if statements
     */
    r_factor = 1;
    switch (particle) {
        /* gamma */
        case ID_GAMMA:
            if (Tp <= 1.95) {
                pow1 = (y + 3.25)/(1 + 8.08*(y + 3.25));
                r_factor = 3.05*exp(-107.0*pow1*pow1);
            } else
                r_factor = 1.01;
            break;
        /* electron */
        case ID_ELECTRON:
            if (Tp <= 15.6) {
                pow1 = (y + 3.26)/(1 + 9.21*(y + 3.26));
                r_factor = 3.63*exp(-106*pow1*pow1) + y*(-0.182 - 0.175*y);
            } else
                r_factor = 1.01;
            break;
        /* positron */
        case ID_POSITRON:
            if (Tp <= 5.52) {
                pow1 = (y + 3.25)/(1 + 10.4*(y + 3.25));
                r_factor = 2.22*exp(-98.9*pow1*pow1);
            }
            break;
        /* electron neutrino */
        case ID_NUE:
            if (Tp <= 7.81) {
                pow1 = (y + 3.26)/(1 + 6.56*(y + 3.26));
                r_factor = 0.329*exp(-247*pow1*pow1) + y*(-0.959 - 0.229*y);
            }
            break;
        /* muon neutrino */
        case ID_NUMU:
            if (Tp <= 15.6) {
                pow1 = (y + 3.25)/(1 + 8.38*(y + 3.25));
                r_factor = 2.23*exp(-93.4*pow1*pow1) + y*(-0.376 - 0.121*y);
            }
            break;
        /* electron antineutrino */
        case ID_ANTINUE:
            if (Tp <= 15.6) {
                pow1 = (y + 3.27)/(1 + 6.59*(y + 3.27));
                r_factor = 2.67*exp(-45.7*pow1*pow1) + y*(-0.301 - 0.208*y);
            }
            break;
        /* muon antineutrino */
        case ID_ANTINUMU:
            if (Tp <= 15.6) {
                pow1 = (y + 3.25)/(1 + 8.34*(y + 3.25));
                r_factor = 2.56*exp(-107*pow1*pow1) + y*(-0.385 - 0.125*y);
            }
            break;
    }
    sigma = sigma*r_factor;

    return sigma;
}

/**
 * Calculates the inclusive cross section for the diffraction dissociation
 * process (equation 9 of Kamae et al. 2006). The inclusive cross section is
 * defined as dsigma/dlog(E) and is returned in units of mb.
 *
 * @param particle Secondary particle id number (see cparamlib.h).
 * @param E        Secondary particle energy in GeV.
 * @param Tp       Proton kinetic energy in GeV.
 * @param params   Pointer to a ::PARAMSET struct. The struct should be
 *                 initialized before being passed to this function.
 * @return         Inclusive cross section dsigma/dlogE in mb.
 *
 */
double sigma_incl_diff(int particle, double E, double Tp, PARAMSET* params)
{
    double Wdiff, Lmax;
    double x, y;
    double pow1, pow2;
    double sigma;
    double cutoff;

    /* check whether params is a null pointer or not */
    if (params == NULL)
        return 0;

    /* calculate log(E) and log(Tp) */
    x = log10(E);
    y = log10(Tp*0.001);

    /* init some variables */
    Lmax = y + 3.0;
    Wdiff = 75.0;

    /* calculate the sigma due to diffractive process for given gamma-ray energy */
    /* 06/12/06: replaced code involving pow() */
    pow1 = (x - params->b[2])/(1 + params->b[3]*(x - params->b[2]));
    pow2 = (x - params->b[6])/(1 + params->b[7]*(x - params->b[6]));
    sigma = params->b[0]*exp(-params->b[1]*pow1*pow1) + params->b[4]*exp(-params->b[5]*pow2*pow2);

    /* cutoff is the kinematic limit function as in the paper */
    cutoff = 1/(1 + exp(Wdiff*(x - Lmax)));
    sigma = sigma*cutoff;

    if (sigma < 0)
        sigma = 0;

    return sigma;
}

/**
 * Calculates the inclusive cross section for the Delta(1232) resonance
 * (equation 12 of Kamae et al. 2006). The inclusive cross section is defined as
 * dsigma/dlog(E) and is returned in units of mb.
 *
 * @param particle Secondary particle id number (see cparamlib.h)
 * @param E        Secondary particle energy in GeV
 * @param Tp       Proton kinetic energy in GeV
 * @param params   Pointer to a ::PARAMSET struct. The struct should be
 *                 initialized before being passed to this function.
 * @return         Inclusive cross section dsigma/dlogE in mb
 *
 */
double sigma_incl_delta(int particle, double E, double Tp, PARAMSET* params)
{
    double Wdiff, Lmax;
    double x, y;
    double xc2;
    double pow;
    double sigma;
    double cutoff;

    /* check whether params is a null pointer or not */
    if (params == NULL)
        return 0;

    /* calculate log(E) and log(Tp) */
    x = log10(E);
    y = log10(Tp*0.001);

    /* init some variables */
    Lmax = y + 3.0;
    Wdiff = 75.0;

    /* calculate the sigma due to resonance process for given gamma-ray energy */
    /* 06/12/06: replaced code involving pow() */
    xc2 = x - params->c[2];
    pow = xc2/(1 + xc2*(params->c[3] + params->c[4]*xc2));
    sigma = params->c[0]*exp(-params->c[1]*pow*pow);

    /* cutoff is the kinematic limit function as in the paper */
    cutoff = 1/(1 + exp(Wdiff*(x - Lmax)));
    sigma = sigma*cutoff;

    if (sigma < 0)
        sigma = 0;

    return sigma;
}

/**
 * Calculates the inclusive cross section for the res(1600) resonance
 * (equation 12 of Kamae et al. 2006). The inclusive cross section is defined as
 * dsigma/dlog(E) and is returned in units of mb.
 *
 * @param particle Secondary particle id number (see cparamlib.h).
 * @param E        Secondary particle energy in GeV.
 * @param Tp       Proton kinetic energy in GeV.
 * @param params   Pointer to a ::PARAMSET struct. The struct should be
 *                 initialized before being passed to this function.
 * @return         Inclusive cross section dsigma/dlogE in mb.
 *
 */
double sigma_incl_res(int particle, double E, double Tp, PARAMSET* params)
{
    double Wdiff, Lmax;
    double x, y;
    double xd2;
    double pow;
    double sigma;
    double cutoff;

    /* check whether params is a null pointer or not */
    if (params == NULL)
        return 0;

    /* calculate log(E) and log(Tp) */
    x = log10(E);
    y = log10(Tp*0.001);

    /* init some variables */
    Lmax = y + 3.0;
    Wdiff = 75.0;

    /* calculate the sigma due to resonance process for given gamma-ray energy */
    /* 06/12/06: replaced code involving pow() */
    xd2 = x - params->d[2];
    pow = xd2/(1 + xd2*(params->d[3] + params->d[4]*xd2));
    sigma = params->d[0]*exp(-params->d[1]*pow*pow);

    /* cutoff is the kinematic limit function as in the paper */
    cutoff = 1/(1 + exp(Wdiff*(x - Lmax)));
    sigma = sigma*cutoff;

    if (sigma < 0)
        sigma = 0;

    return sigma;
}

/**
 * Calculates the total inclusive cross section, i.e. the sum of all components
 * (non-diffractive interaction, diffraction dissociation, Delta(1232) and
 * res(1600) resonances). The inclusive cross section is defined as
 * dsigma/dlog(E) and is returned in units of mb.
 *
 * @param particle Secondary particle id number (see cparamlib.h).
 * @param E        Secondary particle energy in GeV.
 * @param Tp       Proton kinetic energy in GeV.
 * @param params   Pointer to a ::PARAMSET struct. The struct should be
 *                 initialized before being passed to this function.
 * @return         Inclusive cross section dsigma/dlogE in mb.
 *
 */
double sigma_incl_tot(int particle, double E, double Tp, PARAMSET* params)
{
    double s_tot;
    double s_nd, s_diff, s_delta, s_res;

    /* check whether params is a null pointer or not */
    if (params == NULL)
        return 0;

    s_tot = s_nd = s_diff = s_delta = s_res = 0;

    /* calculate sigmas */
    s_nd = sigma_incl_nd(particle, E, Tp, params);
    s_diff = sigma_incl_diff(particle, E, Tp, params);
    s_delta = sigma_incl_delta(particle, E, Tp, params);
    s_res = sigma_incl_res(particle, E, Tp, params);

    s_tot = s_nd + s_diff + s_delta + s_res;

    return s_tot;
}

/**
 * Calculates the pT distribution for non-resonance (non-diffractive plus
 * diffraction dissociation). The pT distribution is defined as the differential
 * cross section d^2sigma/dlog(E)dpT and is returned in units of mb/(GeV/c).
 *
 * @param particle  Secondary particle id number (see cparamlib.h).
 * @param pT        Secondary particle transverse momentum in GeV/c.
 * @param E         Secondary particle energy in GeV.
 * @param Tp        Proton kinetic energy in GeV.
 * @param pt_params Pointer to a ::PARAMSET_PT struct. The struct should be
 *                  initialized before being passed to this function.
 * @return          Inclusive cross section d^2sigma/dlog(E)dpT in mb/(GeV/c).
 *
 */
double sigma_pt_nr(int particle, double pT, double E, double Tp, PARAMSET_PT* pt_params)
{
    double x;
    double x1, Lp, W;
    double sigma;
    double cutoff;

    /* check whether params is a null pointer or not */
    if (pt_params == NULL)
        return 0;

    sigma = 0;
    W = 75.0;
    Lp = 0;

    x = log10(E);

    if (particle == ID_GAMMA) {
        sigma = pt_params->a[0]*pT*exp(-pT/pt_params->a[1]);
        
        /* cutoff is the kinematic limit function as in the paper */
        if (x > 0.5) {
            Lp = 2.5;
        } else if (x > -1.0) {
            x1 = x + 1.0;
            Lp = -0.793 + exp(0.271*x1 + 0.363*x1*x1);
        } else {
            Lp = 0.09755 + 0.670*exp(1.81*x);
        }
        cutoff = 1/(1 + exp(W*(pT - Lp)));
        sigma = sigma*cutoff;

        if (sigma < 0)
            sigma = 0;
    } 
    
    return sigma;
}

/**
 * Calculates the pT distribution for the Delta(1232) resonance. The pT
 * distribution is defined as the differential cross section d^2sigma/dlog(E)dpT
 * and is returned in units of mb/(GeV/c).
 *
 * @param particle  Secondary particle id number (see cparamlib.h).
 * @param pT        Secondary particle transverse momentum in GeV/c.
 * @param E         Secondary particle energy in GeV.
 * @param Tp        Proton kinetic energy in GeV.
 * @param pt_params Pointer to a ::PARAMSET_PT struct. The struct should be
 *                  initialized before being passed to this function.
 * @return          Inclusive cross section d^2sigma/dlog(E)dpT in mb/(GeV/c).
 *
 */
double sigma_pt_delta(int particle, double pT, double E, double Tp, PARAMSET_PT* pt_params)
{
    double x;
    double pow;
    double x1, Lp, W;
    double sigma;
    double cutoff;

    /* check whether params is a null pointer or not */
    if (pt_params == NULL)
        return 0;

    sigma = 0;
    W = 75.0;
    Lp = 0;

    x = log10(E);

    if (particle == ID_GAMMA) {
        pow = pT - pt_params->b[0][1];
        sigma = pt_params->b[0][0]*pT*exp(-pow*pow/pt_params->b[0][2]);

        /* cutoff is the kinematic limit function as in the paper */
        if (x > 0.5) {
            Lp = 2.5;
        } else if (x > -1.0) {
            x1 = x + 1.0;
            Lp = -0.793 + exp(0.271*x1 + 0.363*x1*x1);
        } else {
            Lp = 0.09755 + 0.670*exp(1.81*x);
        }
        cutoff = 1/(1 + exp(W*(pT - Lp)));

        sigma = sigma*cutoff;

        if (sigma < 0)
            sigma = 0;
    }

    return sigma;
}

/**
 * Calculates the pT distribution for the res(1600) resonance. The pT
 * distribution is defined as the differential cross section d^2sigma/dlog(E)dpT
 * and is returned in units of mb/(GeV/c).
 *
 * @param particle  Secondary particle id number (see cparamlib.h).
 * @param pT        Secondary particle transverse momentum in GeV/c.
 * @param E         Secondary particle energy in GeV.
 * @param Tp        Proton kinetic energy in GeV.
 * @param pt_params Pointer to a ::PARAMSET_PT struct. The struct should be
 *                  initialized before being passed to this function.
 * @return          Inclusive cross section d^2sigma/dlog(E)dpT in mb/(GeV/c).
 *
 */
double sigma_pt_res(int particle, double pT, double E, double Tp, PARAMSET_PT* pt_params)
{
    double x;
    double pow;
    double x1, Lp, W;
    double sigma;
    double cutoff;

    /* check whether params is a null pointer or not */
    if (pt_params == NULL)
        return 0;

    sigma = 0;
    W = 75.0;
    Lp = 0;

    x = log10(E);

    if (particle == ID_GAMMA) {
        pow = pT - pt_params->c[0][1];
        sigma = pt_params->c[0][0]*pT*exp(-pow*pow/pt_params->c[0][2]);

        /* cutoff is the kinematic limit function as in the paper */
        if (x > 0.5) {
            Lp = 2.5;
        } else if (x > -1.0) {
            x1 = x + 1.0;
            Lp = -0.793 + exp(0.271*x1 + 0.363*x1*x1);
        } else {
            Lp = 0.09755 + 0.670*exp(1.81*x);
        }
        cutoff = 1/(1 + exp(W*(pT - Lp)));
        sigma = sigma*cutoff;

        if (sigma < 0)
            sigma = 0;
    }

    return sigma;
}

/**
 * Calculates the total pT distribution, i.e. sum of all components,
 * non-resonance, Delta(1232) and res(1600) resonances. The pT distribution is
 * defined as the differential cross section d^2sigma/dlog(E)dpT and is
 * returned in units of mb/(GeV/c).
 *
 * @param particle  Secondary particle id number (see cparamlib.h).
 * @param pT        Secondary particle transverse momentum in GeV/c.
 * @param E         Secondary particle energy in GeV.
 * @param Tp        Proton kinetic energy in GeV.
 * @param pt_params Pointer to a ::PARAMSET_PT struct. The struct should be
 *                  initialized before being passed to this function.
 * @return          Inclusive cross section d^2sigma/dlog(E)dpT in mb/(GeV/c).
 *
 */
double sigma_pt_tot(int particle, double pT, double E, double Tp, PARAMSET_PT* pt_params)
{
    double s_tot;
    double s_nr, s_delta, s_res;

    /* check whether params is a null pointer or not */
    if (pt_params == NULL)
        return 0;

    s_tot = s_nr = s_delta = s_res = 0;

    /* calculate sigmas */
    s_nr = sigma_pt_nr(particle, pT, E, Tp, pt_params);
    s_delta = sigma_pt_delta(particle, pT, E, Tp, pt_params);
    s_res = sigma_pt_res(particle, pT, E, Tp, pt_params);
    
    s_tot = s_nr + s_delta + s_res;

    return s_tot;
}

/**
 * Calculates the inelastic proton-proton cross section for the non-diffractive
 * interaction component, as given by equation 3 in Kamae et al. (2006). The
 * cross section sigma(pp) is returned in units of mb.
 *
 * @param Pp       Proton momentum in GeV/c.
 * @return         Inelastic cross section sigma(pp) in mb.
 *
 */
double sigma_pp_nd(double Pp)
{
    double a[9] = {0.57, 0.1176, 0.3829, 23.10, 6.454, -5.764, -23.63, 94.75, 0.02667};
    double b[2] = {11.34, 23.72};
    double c[3] = {28.5, -6.133, 1.464};
    double x, sigma;

    x = log10(Pp);
    sigma = 0;

    if ((Pp >= 1) && (Pp < 1.3)) {
        sigma = a[0]*pow(x/a[1], 1.2)*(a[3] + x*x*(a[4] + a[5]*x) + a[6]*exp(-a[7]*(x + a[8])*(x + a[8])));
    } else if ((Pp >= 1.3) && (Pp < 2.4)) {
        sigma = (b[0]*fabs(a[2] - x) + b[1]*fabs(a[1] - x))/(a[2] - a[1]);
    } else if ((Pp >= 2.4) && (Pp <= 10.0)) {
        sigma = a[3] + x*x*(a[4] + a[5]*x) + a[6]*exp(-a[7]*(x + a[8])*(x + a[8]));
    } else if (Pp > 10.0) {
        sigma = c[0] + c[1]*x + c[2]*x*x;
    }

    return sigma;
}

/**
 * Calculates the inelastic proton-proton cross section for the diffraction
 * dissociation component, as given by equation 3 in Kamae et al. (2006). The
 * cross section sigma(pp) is returned in units of mb.
 *
 * @param Pp       Proton momentum in GeV/c.
 * @return         Inelastic cross section sigma(pp) in mb.
 *
 */
double sigma_pp_diff(double Pp)
{
    double d[7] = {0.3522, 0.1530, 1.498, 2.0, 30.0, 3.155, 1.042};
    double e[2] = {5.922, 1.632};

    double x, sigma;

    x = log10(Pp);
    sigma = 0;

    if ((Pp >= 2.25) && (Pp < 3.2)) {
        sigma = sqrt((x-d[0])/d[1])*(d[2] + d[3]*log10(d[4]*(x - 0.25)) + x*x*(d[5] - d[6]*x));
    } else if ((Pp >= 3.2) && (Pp <= 100.0)) {
        sigma = d[2] + d[3]*log10(d[4]*(x - 0.25)) + x*x*(d[5] - d[6]*x);
    } else if (Pp > 100.0) {
        sigma = e[0] + e[1]*x;
    }

    return sigma;
}

/**
 * Calculates the inelastic proton-proton cross section for the Delta(1232)
 * component, as given by equation 3 in Kamae et al. (2006). The cross section
 * sigma(pp) is returned in units of mb.
 *
 * @param Pp       Proton momentum in GeV/c.
 * @return         Inelastic cross section sigma(pp) in mb.
 *
 */
double sigma_pp_delta(double Pp)
{
    double f[5] = {0.0834, 9.5, -5.5, 1.68, 3134.0};

    double Ep, sigma;

    /* proton mass 0.938 GeV/c^2 gives mass squared  0.879844 (GeV/c^2)^2 */
    Ep = sqrt(Pp*Pp + 0.879844);
    sigma = 0;

    if ((Ep >= 1.4) && (Ep < 1.6)) {
        sigma = f[0]*pow(Ep, 10);
    } else if ((Ep >= 1.6) && (Ep < 1.8)) {
        sigma = f[1]*exp(-f[2]*(Ep - f[3])*(Ep - f[3]));
    } else if ((Ep >= 1.8) && (Ep <= 10.0)) {
        sigma = f[4]*pow(Ep, -10);
    }

    return sigma;
}

/**
 * Calculates the inelastic proton-proton cross section for the res(1600)
 * component, as given by equation 4 in Kamae et al. (2006). The cross section
 * sigma(pp) is returned in units of mb.
 *
 * @param Pp       Proton momentum in GeV/c.
 * @return         Inelastic cross section sigma(pp) in mb.
 *
 */
double sigma_pp_res(double Pp)
{
    double g[5] = {0.0004257, 4.5, -7.0, 2.1, 503.5};

    double Ep, sigma;

    /* proton mass 0.938 GeV/c^2 gives mass squared  0.879844 (GeV/c^2)^2 */
    Ep = sqrt(Pp*Pp + 0.879844);
    sigma = 0;

    if ((Ep >= 1.6) && (Ep < 1.9)) {
        sigma = g[0]*pow(Ep, 14);
    } else if ((Ep >= 1.9) && (Ep < 2.3)) {
        sigma = g[1]*exp(-g[2]*(Ep - g[3])*(Ep - g[3]));
    } else if ((Ep >= 2.3) && (Ep <= 20.0)) {
        sigma = g[4]*pow(Ep, -6);
    }

    return sigma;
}
