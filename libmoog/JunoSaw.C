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
/**
 * Copyright (c) UltraMaster Group, LLC. All Rights Reserved.
 * $Revision: 1.7.4.1 $$Date: 2000/02/24 00:44:03 $
 */
#include <math.h>
#include "JunoSaw.h"
#include "Scheduler.h"


#define FRQ   0
#define AMP   1

void JunoSaw_frqChanged(MoogObject *o, double data, long)
{
    ((JunoSaw *)o)->frqChanged();
}

void JunoSaw_sync(MoogObject *o, double data, long)
{
    ((JunoSaw *)o)->sync();
}

JunoSaw::JunoSaw()
{
    addPorts( "frq",   INPUT,  JunoSaw_frqChanged, 0, Scheduler::sampleControlRatio, 
	      "amp",   INPUT,  NULL,
	      "sync",  INPUT,  JunoSaw_sync, 0, 1,
	      "sig",   OUTPUT, true,
	      "sync",  OUTPUT, false,
	      NULL );

    output = &outputs[ 0 ];
    inFrq = inputs[0].data;
    inAmp = inputs[1].data;
    inSync = inputs[2].data;

    frq = 0;
    pos = 1.0;
    lastTrigger = 0;
}

void JunoSaw::connectTo(ConnectionInfo *info)
{
    MoogObject::connectTo(info);
    inFrq = inputs[0].data;
    inAmp = inputs[1].data;
    inSync = inputs[2].data;
}

void JunoSaw::disconnectTo(ConnectionInfo *info)
{
    MoogObject::disconnectTo(info);
    inFrq = inputs[0].data;
    inAmp = inputs[1].data;
    inSync = inputs[2].data;
}

void JunoSaw::frqChanged()
{
    newFrq = *inFrq / 2;

    MOOG_DEBUG( "newFrq=%f", newFrq );

    if ( newFrq < 0.0 )
	newFrq *= -1.0;

    if ( newFrq == 0.0 )
	Scheduler::scheduleSampleRate( this, false );
    else if ( !isSampleScheduled() )
	Scheduler::scheduleSampleRate( this, true );
}

void JunoSaw::sync()
{
    pos = 1.0;
}

void JunoSaw::sampleGo()
{
    pos += frq;

    MOOG_DEBUG( "pos=%f, frq=%f", pos, frq );

    if ( pos >= 1.0 )
    {
	lastTrigger = 1;
	outputs[ 1 ].setData( 1 );

	pos--;

	frq = newFrq;
	output->data = -*inAmp;
	power = *inAmp * (frq * 2.0 + .0072);
	dampening = -*inAmp * (2.1 * frq * frq + .01 * frq + .000022);
    }

    else if ( lastTrigger == 1 )
    {
	lastTrigger = 0;
	outputs[ 1 ].setData( 0 );
    }

    output->setData( output->data + power );
    power += dampening;

    if (power < 0 )
    {
	power = -.0004 * *inAmp;
	dampening = 0;
    }

    MOOG_DEBUG( "output=%f", output->data );
}
