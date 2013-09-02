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
 *
 * $Revision: 1.34.4.1 $$Date: 2000/02/24 00:44:03 $
 */

#include <stdio.h>
#include <math.h>
#include <util/debug.h>

#include "DataBlock.h"
#include "Oscillator.h"
#include "Scheduler.h"

void
oscillator_frq_changed( MoogObject* o, double data, long userData )
{
    ((Oscillator*)o)->frqChanged( data );
} 

void
oscillator_sync_changed( MoogObject* o, double data, long userData )
{
    ((Oscillator*)o)->syncChanged( data );
} 

Oscillator::Oscillator(DataBlock *w /* = NULL */) 
{
   init( w );
}

Oscillator::Oscillator( DataBlock* w, double frq, double amp = 1, double zro = 0 )
{
   init( w );
   set( I_OSC_FRQ, frq );
   set( I_OSC_AMP, amp );
   set( I_OSC_ZRO, zro );
}

void
Oscillator::init( DataBlock* w )
{
    addPorts("frq",  INPUT,  oscillator_frq_changed, 0, 1,
	     "amp",  INPUT,  NULL,
	     "zro",  INPUT,  NULL,
	     "sync", INPUT,  oscillator_sync_changed, 0, 1,
	     "sig",  OUTPUT, true,
	     "sync", OUTPUT, false,
	     NULL);

    output = &outputs[ O_OSC_SIG ];
    inFrq = inputs[0].data;
    inAmp = inputs[1].data;
    inZro = inputs[2].data;
    inSync = inputs[3].data;
    
    lastTrigger = 0;

    if ( w != NULL )
    {
       setWaveData( w );
    }

    Scheduler::scheduleSampleRate(this, true);
}

void Oscillator::connectTo(ConnectionInfo *info)
{
    printf("Oscillator connectTo\n");
    MoogObject::connectTo(info);
    inFrq = inputs[0].data;
    inAmp = inputs[1].data;
    inZro = inputs[2].data;
    inSync = inputs[3].data;
}

void Oscillator::disconnectTo(ConnectionInfo *info)
{
    printf("Oscillator disconnectTo\n");
    MoogObject::disconnectTo(info);
    inFrq = inputs[0].data;
    inAmp = inputs[1].data;
    inZro = inputs[2].data;
    inSync = inputs[3].data;
}

void Oscillator::frqChanged( double frq ) 
{
   speed = scale * frq;

   MOOG_DEBUG( "speed %f, scale %f", speed, scale );
}

void Oscillator::syncChanged( double sync )
{
    if ( sync > 0 )
	pos = 0;

    MOOG_DEBUG( "sync" );
}

void Oscillator::sampleGo() 
{
    pos += speed;

    if ( lastTrigger )
    {
	lastTrigger = 0;
	outputs[ 1 ].setData( 0 );
    }

    if ( pos >= waveDataLen )
    {

	outputs[ 1 ].setData( 1 );
	lastTrigger = 1;
	do
	{
	    pos -= waveDataLen;
	} while( pos >= waveDataLen );
    }
   
    while( pos < 0 )
    {
	pos += waveDataLen;
    }
    
    output->setData( waveData[(int)pos] * 
		     *inAmp + 
		     *inZro );
    
    MOOG_DEBUG( "output %f", output->data );
}	

void Oscillator::setWaveData(DataBlock *w) 
{
    pos = 0;
    waveData    = w->data;
    waveDataLen = w->length;
    scale       = ( (double)waveDataLen / Scheduler::sampleRate ) * 
                  Scheduler::nyquistFreq;
}








