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
 * $Revision: 1.11.4.1 $$Date: 2000/02/24 00:44:03 $
 */
#include "JunoPulse.h"
#include "Scheduler.h"

#define FRQ   0
#define AMP   1
#define WIDTH 2

void JunoPulse_horizBoundsChanged(MoogObject *o, double data, long)
{
    ((JunoPulse *)o)->horizBoundsChanged();
}

void JunoPulse_sync(MoogObject *o, double data, long)
{
    ((JunoPulse *)o)->sync();
}

JunoPulse::JunoPulse()
{
    addPorts( "frq",   INPUT,  JunoPulse_horizBoundsChanged, 0, Scheduler::sampleControlRatio, 
	      "amp",   INPUT,  NULL,
	      "width", INPUT,  NULL,
	      "sync",  INPUT,  JunoPulse_sync, 0, 1,
	      "sig",   OUTPUT, true,
	      "sync",  OUTPUT, false,
	      NULL );

    sigOutput = &outputs[ 0 ];
    syncOutput = &outputs[ 1 ];

    pos = 1.0;
    sign = 1.0;

    lastTrigger = 0;

    inFrq = inputs[0].data;
    inAmp = inputs[1].data;
    inWidth = inputs[2].data;
    inSync = inputs[3].data;
}

void JunoPulse::connectTo(ConnectionInfo *info)
{
    MoogObject::connectTo(info);
    inFrq = inputs[0].data;
    inAmp = inputs[1].data;
    inWidth = inputs[2].data;
    inSync = inputs[3].data;
}

void JunoPulse::disconnectTo(ConnectionInfo *info)
{
    MoogObject::disconnectTo(info);
    inFrq = inputs[0].data;
    inAmp = inputs[1].data;
    inWidth = inputs[2].data;
    inSync = inputs[3].data;
}

void JunoPulse::horizBoundsChanged()
{
    frq = *inFrq;

    MOOG_DEBUG( "freq=%f", frq );

    if ( frq < 0.0 )
	frq *= -1.0;

    if ( frq == 0.0 )
	Scheduler::scheduleSampleRate( this, false );
    else if ( !isSampleScheduled() )
	Scheduler::scheduleSampleRate( this, true );
}

void JunoPulse::sync()
{
    pos = 1.0;
    sign = 1.0;
}

void JunoPulse::sampleGo()
{
    pos += frq;

    if ( pos >= .45 && sign < 0 )
    {
	if ( lastTrigger == 0 )
	{
	    lastTrigger = 1;
	    syncOutput->setData( 1 );
	}

	else if ( syncOutput->data == 1 )
	{
	    syncOutput->setData( 0 );
	}
    }

    if ( pos >= 1.0 )
    {
	lastTrigger = 0;

	// Juno Pulse width dever goes all the way to zero ( or one )
	pos = pos - 1.0 + (sign * *inWidth * .94);

	if ( sign == 1 )
	{
	    sigOutput->data = sign * *inAmp;
	    power = -.008 * sign * *inAmp;
	}

	else
	{
	    sigOutput->data -= *inAmp;
	    power = .008 * sign * sigOutput->data;
	}

	dampening = -power / 320.0;
	sign = -sign;
    }

    sigOutput->setData( sigOutput->data + power );
    power += dampening;

    if ( (sign < 0 && power > 0) || (sign > 0 && power < 0 ))
    {
	power = sign * -.0004 * *inAmp;
	dampening = 0;
    }
}
