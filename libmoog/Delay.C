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
#include <util/debug.h>

#include "Delay.h"
#include "Scheduler.h"

//#define DELAY_INTERPOLATE 

void
delay_mix_changed( MoogObject* o, double data, long userData )
{
    ((Delay*)o)->mixChanged( data );
} 

void
delay_feedback_changed( MoogObject* o, double data, long userData )
{
    ((Delay*)o)->feedbackChanged( data );
} 

Delay::Delay( int _dsize )
{
    dsize = _dsize;
    dline = new double[ dsize ];
    wpos = 0;

    mix      = 0;
    feedback = 0;

    addPorts( "sig",      INPUT, NULL,
	      "time",     INPUT, NULL,
	      "feedback", INPUT, delay_feedback_changed, 0, 1,
	      "mix",      INPUT, delay_mix_changed, 0, 1,
	      "sig",      OUTPUT, true,
	      NULL );

    memset( dline, 0, sizeof(double) * dsize );

    output = &outputs[ O_DLY_SIG ];
    inSig = inputs[0].data;
    inTime = inputs[1].data;
    inFeedback = inputs[2].data;
    inMix = inputs[3].data;

    Scheduler::scheduleSampleRate( this, true );
}

void Delay::connectTo(ConnectionInfo *info)
{
    MoogObject::connectTo(info);
    inSig = inputs[0].data;
    inTime = inputs[1].data;
    inFeedback = inputs[2].data;
    inMix = inputs[3].data;
}

void Delay::disconnectTo(ConnectionInfo *info)
{
    MoogObject::disconnectTo(info);
    inSig = inputs[0].data;
    inTime = inputs[1].data;
    inFeedback = inputs[2].data;
    inMix = inputs[3].data;
}


void Delay::mixChanged( double _mix )
{
    mix = _mix;
}

void Delay::feedbackChanged( double _feedback )
{
    feedback = _feedback;
}

double Delay::getTap( double dtime )
{
    double rpos = (double)wpos - ( dtime * dsize );
    if ( rpos < 0 )
	rpos += dsize;
    
#ifdef DELAY_INTERPOLATE
    int s1 = (int)rpos;
    int s2 = s1 + 1;
    if ( s2 == dsize )
	s2 = 0;
    // s1 plus a fraction of the difference between s2 and s1
    return( dline[s1] + (rpos - s1) * (dline[s2]-dline[s1]) ); 
#else
    return( dline[ (int)rpos ] );
#endif
}

void Delay::sampleGo()
{
    double in  = *inSig;
    double out = getTap( *inTime );

    output->data = in * ( 1.0 - mix ) + out * mix;

    dline[ wpos++ ] = in + out * feedback;
    if ( wpos == dsize )
	wpos = 0;
}





