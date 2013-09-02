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
#include <util/debug.h>

#include "Attenuator.h"
#include "Scheduler.h"

Attenuator::Attenuator( double _amp, double _zro ) 
{
    addPorts( "sig", INPUT, NULL, 
	      "amp", INPUT, NULL, 
	      "zro", INPUT, NULL, 
	      "sig", OUTPUT, true,
	      NULL );
    
    set( I_ATT_AMP, _amp );
    set( I_ATT_ZRO, _zro );

    output = &outputs[ O_ATT_SIG ];

    inSig = inputs[I_ATT_SIG].data;
    inAmp = inputs[I_ATT_AMP].data;
    inZro = inputs[I_ATT_ZRO].data;

    Scheduler::scheduleSampleRate(this, true);
}

void Attenuator::connectTo(ConnectionInfo *info)
{
    MoogObject::connectTo(info);
    inSig = inputs[I_ATT_SIG].data;
    inAmp = inputs[I_ATT_AMP].data;
    inZro = inputs[I_ATT_ZRO].data;
}

void Attenuator::disconnectTo(ConnectionInfo *info)
{
    MoogObject::disconnectTo(info);
    inSig = inputs[I_ATT_SIG].data;
    inAmp = inputs[I_ATT_AMP].data;
    inZro = inputs[I_ATT_ZRO].data;
}

void Attenuator::sampleGo() 
{
    output->setData( *inSig * *inAmp + *inZro);
    MOOG_DEBUG( "attenuator %f %f %f %f",  *inSig, *inAmp, *inZro, output->data);
}
