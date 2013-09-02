#include <stdio.h>
#include <math.h>


static inline void prewarp(
    double *a0, 
    double *a1, 
    double *a2, 
    double fc, 
    double fs);

static inline void bilinear(
    double a0, double a1, double a2, /* numerator coef. */
    double b0, double b1, double b2, /* denominator coef.  */
    double *k,                       /* overall gain factor */
    double fs,                       /* sampling rate */
    double *coef);                   /* ptr. to 4 iir coef. */


/*
 * ----------------------------------------------------------
 *      Pre-warp the coefficients of a numerator or denominator.
 *      Note that a0 is assumed to be 1, so there is no wrapping
 *      of it.
 * ----------------------------------------------------------
 */
void prewarp(double *a0, double *a1, double *a2, double fc, double fs)
{
    double wp;

    if (fc <= 0)
	fc = .00001;

    wp = 2.0 * fs * tan(M_PI * fc / fs);

    *a2 = (*a2) / (wp * wp);
    *a1 = (*a1) / wp;
}


/*
 * ----------------------------------------------------------
 * bilinear()
 *
 * Transform the numerator and denominator coefficients
 * of s-domain biquad section into corresponding
 * z-domain coefficients.
 *
 *      Store the 4 IIR coefficients in array pointed by coef
 *      in following order:
 *             beta1, beta2    (denominator)
 *             alpha1, alpha2  (numerator)
 *
 * Arguments:
 *             a0-a2   - s-domain numerator coefficients
 *             b0-b2   - s-domain denominator coefficients
 *             k       - filter gain factor. initially set to 1
 *                       and modified by each biquad section in such
 *                       a way, as to make it the coefficient by
 *                       which to multiply the overall filter gain
 *                       in order to achieve a desired overall filter gain,
 *                       specified in initial value of k.
 *             fs      - sampling rate (Hz)
 *             coef    - array of z-domain coefficients to be filled in.
 *
 * Return:
 *             On return, set coef z-domain coefficients
 * ----------------------------------------------------------
 */
void bilinear(
    double a0, double a1, double a2,    /* numerator coefficients */
    double b0, double b1, double b2,    /* denominator coefficients */
    double *k,                          /* overall gain factor */
    double fs,                          /* sampling rate */
    double *coef                        /* pointer to 4 iir coefficients */
)
{
    double ad, bd;

                 /* alpha (Numerator in s-domain) */
    ad = 4. * a2 * fs * fs + 2. * a1 * fs + a0;
                 /* beta (Denominator in s-domain) */
    bd = 4. * b2 * fs * fs + 2. * b1* fs + b0;

                 /* update gain constant for this section */
    *k *= ad/bd;

                 /* Denominator */
    *coef++ = (2. * b0 - 8. * b2 * fs * fs)
                           / bd;         /* beta1 */
    *coef++ = (4. * b2 * fs * fs - 2. * b1 * fs + b0)
                           / bd; /* beta2 */

                 /* Nominator */
    *coef++ = (2. * a0 - 8. * a2 * fs * fs)
                           / ad;         /* alpha1 */
    *coef = (4. * a2 * fs * fs - 2. * a1 * fs + a0)
                           / ad;   /* alpha2 */
}


/*
 * ----------------------------------------------------------
 * Transform from s to z domain using bilinear transform
 * with prewarp.
 *
 * Arguments:
 *      For argument description look at bilinear()
 *
 *      coef - pointer to array of floating point coefficients,
 *                     corresponding to output of bilinear transofrm
 *                     (z domain).
 *
 * Note: frequencies are in Hz.
 * ----------------------------------------------------------
 */
void szxform(
    double a0, double a1, double a2, /* numerator coefficients */
    double b0, double b1, double b2, /* denominator coefficients */
    double fc,                          /* Filter cutoff frequency */
    double fs,                          /* sampling rate */
    double *k,                          /* overall gain factor */
    double *coef                        /* pointer to 4 iir coefficients */)
{
                 /* Calculate a1 and a2 and overwrite the original values */

    //printf("p1: %f %f %f %f %f %f\n", a0, a1, a2, b0, b1, b2);
    prewarp(&a0, &a1, &a2, fc, fs);
    //printf("p2: %f %f %f %f %f %f\n", a0, a1, a2, b0, b1, b2);
    prewarp(&b0, &b1, &b2, fc, fs);
    //printf("p3: %f %f %f %f %f %f\n", a0, a1, a2, b0, b1, b2);
    
    //printf("1: %f %f %f %f\n",coef[0], coef[1], coef[2], coef[3]);
    bilinear(a0, a1, a2, b0, b1, b2, k, fs, coef);
    //printf("2: %f %f %f %f\n",coef[0], coef[1], coef[2], coef[3]);
}

