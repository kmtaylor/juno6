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

#include "ADSR.h"
#include "Scheduler.h"
#include <util/debug.h>

// note, this is a good scale at 441000, needs to be base on sample rate
#define ADSR_SCALE .00010;

void
adsr_attack_changed( MoogObject* o, double data, long userData )
{
    ((ADSR*)o)->attackChanged( data );
} 

void
adsr_decay_changed( MoogObject* o, double data, long userData )
{
    ((ADSR*)o)->decayChanged( data );
} 

void
adsr_sustain_changed( MoogObject* o, double data, long userData )
{
    ((ADSR*)o)->sustainChanged( data );
} 

void
adsr_release_changed( MoogObject* o, double data, long userData )
{
    ((ADSR*)o)->releaseChanged( data );
} 

void
adsr_amp_changed( MoogObject* o, double data, long userData )
{
    ((ADSR*)o)->ampChanged( data );
} 

void
adsr_trigger_changed( MoogObject* o, double data, long userData )
{
    ((ADSR*)o)->triggerChanged( data );
} 

ADSR::ADSR( double _a, double _d, double _s, double _r, double _amp )
{

    addPorts( "a",    INPUT, adsr_attack_changed, 0, 1,
	      "d",    INPUT, adsr_decay_changed, 0, 1,
	      "s",    INPUT, adsr_sustain_changed, 0, 1,
	      "r",    INPUT, adsr_release_changed, 0, 1,
	      "amp",  INPUT, adsr_amp_changed, 0, 1,
	      "trig", INPUT, adsr_trigger_changed, 0, 1,
	      "sig",  OUTPUT, true,
	      NULL );

    output = &outputs[ 0 ];

    set( 0, _a );

    set( 1, _d );

    set( 2, _s );

    set( 3, _r );

    set( 4, _amp );

    state = FINISHED;
}

void ADSR::attackChanged( double _attack )
{
    if ( _attack <= 0 )
	attack = 1.0;
    else
	attack = -log( _attack *.94 ) * ADSR_SCALE;
}

void ADSR::decayChanged( double _decay )
{
    if ( _decay <= 0 )
	decay = -1.0;
    else
	decay = log( _decay * .94 ) * ADSR_SCALE;
}

void ADSR::sustainChanged( double _sustain )
{
    sustain = _sustain;
}

void ADSR::releaseChanged( double _release )
{
    if ( _release <= 0 )
	release = -1.0;
    else
	release = log( _release * .94 ) * ADSR_SCALE;
}

void ADSR::ampChanged( double _amp )
{
    amp = _amp;
}

void ADSR::triggerChanged( double trigger )
{
    if ( trigger > 0 )
	state = ATTACK;

    else
	state = RELEASE;

    if ( !isSampleScheduled() )
    {
	Scheduler::scheduleSampleRate( this, true );
    }
}

void ADSR::sampleGo()
{
    double tmpData = output->data;

    //printf("%f: %f %f %f %f %d\n", tmpData, attack, decay, sustain, release, state);

    switch( state )
    {
    case ATTACK:
	tmpData += attack;

	if ( tmpData >= amp )
	{
	    tmpData = amp;
	    state = DECAY;
	}
	break;
	
    case DECAY:  
	tmpData += decay;

	if ( tmpData <= sustain * amp )
	{
	    tmpData = sustain * amp;
	    state = SUSTAIN;
	}
	break;
	
    case RELEASE:
	tmpData += release;

	if ( tmpData <= 0.0 )
	{
	    tmpData = 0.0;
	    state = FINISHED;
	}
	break;

    case SUSTAIN:
	if (tmpData < sustain)
	{
	    tmpData -= decay;
	    if (tmpData > sustain)
		tmpData = sustain;
	}
	else if (tmpData > sustain)
	{
	    tmpData += decay;
	    if (tmpData < sustain)
		tmpData = sustain;
	}
	break;
	  

    case FINISHED:
	MOOG_DEBUG( "finished" );
	Scheduler::scheduleSampleRate( this, false );
	break;
    }

    output->setData( tmpData );
    MOOG_DEBUG( "output=%f", output->data );
}
