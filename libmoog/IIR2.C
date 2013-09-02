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
#include "IIR2.h"
#include "Scheduler.h"

IIR2::IIR2()
{
    gain = 0;
    cx[0] = cx[1] = cy[0] = cy[1] = 0;
    x[0] = x[1] = y[0] = y[1] = 0;

    addInput("sig", NULL,0,0);
    addOutput("sig", true);

    output = getOutput(0);

    Scheduler::scheduleSampleRate(this, true);
}

void IIR2::connectTo(ConnectionInfo *info)
{
    MoogObject::connectTo(info);
    in = inputs[0].data;
}
void IIR2::disconnectTo(ConnectionInfo *info)
{
    MoogObject::disconnectTo(info);
    in = inputs[0].data;
}


void IIR2::sampleGo()
{
    double out = gain * *in;
    out = out + cx[0] * x[0] + cx[1] * x[1] - cy[0] * y[0] - cy[1] * y[1];

    if (isnan(out))
	out = 0;

    x[1] = x[0];
    x[0] = *in;

    y[1] = y[0];
    y[0] = out;

    output->setData(out);
    MOOG_DEBUG("in %f out %f", *in, out);
}
