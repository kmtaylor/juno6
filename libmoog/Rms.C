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
#include "Rms.h"
#include "Scheduler.h"

Rms::Rms()
{
    addPorts( "sig", INPUT,  NULL, 
	      "pow", OUTPUT, true,
	      NULL);

    output = &outputs[ O_RMS_POWER ];
    
    double   b;
    double twoPiDivSampleRate = 2 * M_PI / Scheduler::sampleRate;
    double halfPower = 10; 
    
    b = 2.0 - cos( halfPower * twoPiDivSampleRate );
    c2 = b - sqrt( b * b - 1.0 );
    c1 = 1.0 - c2;

    inSig = inputs[0].data;

    Scheduler::scheduleSampleRate( this, true );
}

void Rms::connectTo(ConnectionInfo *info)
{
    MoogObject::connectTo(info);
    inSig = inputs[0].data;
}

void Rms::disconnectTo(ConnectionInfo *info)
{
    MoogObject::disconnectTo(info);
    inSig = inputs[0].data;
}


void Rms::sampleGo()
{
    q = c1 * *inSig * *inSig + c2 * q;
    output->setData(sqrt( q ));
}
