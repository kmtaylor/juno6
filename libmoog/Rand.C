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
#include <stdlib.h>
#include <math.h>

#include "Rand.h"
#include "Scheduler.h"

void Rand_freqChanged(MoogObject *o, double data, long userData)
{
    ((Rand*)o)->freqChanged(data);
}

Rand::Rand( int _type, double amp ) 
{
    addPorts( "frq", INPUT,  Rand_freqChanged, 1, 0, 
	      "amp", INPUT,  NULL,
	      "zro", INPUT,  NULL,
	      "sig", OUTPUT, true, 
	      NULL );

    output = &outputs[ O_RND_SIG ];
    inFrq = inputs[0].data;
    inAmp = inputs[1].data;
    inZro = inputs[2].data;

    type    = _type;
    last    = ( 2.0 * rand() / RAND_MAX ) - 1.0;
    current = ( 2.0 * rand() / RAND_MAX ) - 1.0;
    pos     = 0;

    set( I_RND_AMP, amp );

    Scheduler::scheduleSampleRate( this, true );
}

void Rand::connectTo(ConnectionInfo *info)
{
    MoogObject::connectTo(info);
    inFrq= inputs[0].data;
    inAmp = inputs[1].data;
    inZro = inputs[2].data;
}

void Rand::disconnectTo(ConnectionInfo *info)
{
    MoogObject::disconnectTo(info);
    inFrq = inputs[0].data;
    inAmp = inputs[1].data;
    inZro = inputs[2].data;
}


void
Rand::freqChanged(double data)
{
    step = fabs(data);
}

void
Rand::sampleGo()
{ 
   if ( type == RND_NORMAL )
   {
      output->setData(*inZro + *inAmp * ( ( 2.0 * rand() / RAND_MAX ) - 1.0 ));
   }

   else if ( type == RND_HOLD )
   {
      if ( floor( pos + step ) > floor( pos ) )
      {
	 pos = 0;
	 current = ( 2.0 * rand() / RAND_MAX ) - 1.0;
      }

      pos += step;

      output->setData(*inZro + *inAmp * current);
   }

   else if ( type == RND_INT )
   {
      if ( floor( pos + step ) > floor( pos ) )
      {
	 pos     = 0;
	 last    = current;
	 current = ( 2.0 * rand() / RAND_MAX ) - 1.0;
      }

      pos += step;

      output->setData(*inZro + *inAmp * (last + ((current - last) * pos )));
   }
}



