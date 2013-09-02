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
#include <math.h>
#include "Scheduler.h"
#include "HPF.h"

void HPF_frqChanged(MoogObject *o, double data, long userData)
{
    ((HPF*)o)->frqChanged(data);
}

void HPF_QChanged(MoogObject *o, double data, long userData)
{
    ((HPF*)o)->QChanged(data);
}

HPF::HPF()
{
    addInput("frq", HPF_frqChanged, 0, 1);
    addInput("Q",   HPF_QChanged,   0, 1);
}

void HPF::updateCoef()
{
    /* omega is really 2 * PI * frequency / sample_rate,
     * but our frq is scaled already to 0->1 equals 0hz -> nyquist
     * so it all reduces out
     */
    //FIXME: we've fixed frq on an exponential from 20hz to ~2500hz
    double omega = M_PI * CPS(20) * pow(2.0, frq * 7.0);
    //FIXME: Q is scaled by 100... is this correct?
    double alpha = sin(omega) / (2.0 * Q * 100.0 + 1.0);
    double cs = cos(omega);
    
    gain = ((1.0 + cs)/2.0)/(1.0 + alpha);
    cx[0] = -(1.0 + cs)/(1.0 + alpha);
    cx[1] = ((1.0 + cs)/2.0)/(1.0 + alpha);
    cy[0] = (-2.0 * cs)/(1.0 + alpha);
    cy[1] = (1.0 - alpha)/(1.0 + alpha);
}
