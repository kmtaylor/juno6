/*
 * Copyright(c) 2000 UltraMaster Group
 *
 * License to use UltraMaster Juno-6 is provided free of charge subject to the 
 * following restrictions:
 *
 * 1.) This license is for your personal use only.
 *
 * 2.) No portion of this software may be redistributed by you to any other 
 *     person in any form. 
 *
 * 3.) You may not sell UltraMaster Juno-6 to any person.
 *
 * 4.) UltraMaster Juno-6 is provided without any express or implied warranty. 
 *     In no event shall UltraMaster Group be held liable for any damages 
 *     arising from the use of UltraMaster Juno-6.
 */
#ifndef _LIBMOOG_BILINEAR_H
#define _LIBMOOG_BILINEAR_H

#ifdef __cplusplus
extern "C" {
#endif

void szxform(
    double a0, double a1, double a2, /* numerator coefficients */
    double b0, double b1, double b2, /* denominator coefficients */
    double  fc,                         /* Filter cutoff frequency */
    double  fs,                         /* sampling rate */
    double *k,                          /* overall gain factor */
    double *coef                        /* pointer to 4 iir coefficients */);

#ifdef __cplusplus
}
#endif

#endif /* _LIBMOOG_BILINEAR_H */
