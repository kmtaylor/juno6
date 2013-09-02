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
#include "Chorus.h"
#include "ConnectionManager.h"
#include "WaveGen.h"
#include "Delay.h"
#include "Oscillator.h"
#include "Scheduler.h"

Chorus::Chorus()
{
    // 30 ms delay
    // FIXME, add a depth control
    dly = new Delay( (int)(Scheduler::sampleRate * .03) );
    osc = new Oscillator( wg_tri( (int)(Scheduler::sampleRate * .03) ) );

    inputs.appendElement( dly->getInput( "sig" ) );
    inputs.appendElement( osc->getInput( "frq" ) );
    inputs.appendElement( dly->getInput( "mix" ) );
    inputs.appendElement( osc->getInput( "amp" ) );
    inputs.appendElement( osc->getInput( "zro" ) );

    outputs.appendElement( dly->getOutput( "sig" ) );

//    osc->set( "amp", .05 );
//    osc->set( "zro", .8333 );

    PATCH( osc, "sig", dly, "time" );
}

void Chorus::connectTo(ConnectionInfo *info)
{
    if (info->input == &inputs[0] || info->input == &inputs[2])
	dly->connectTo(info);
    else
	osc->connectTo(info);
}

void Chorus::disconnectTo(ConnectionInfo *info)
{
    if (info->input == &inputs[0] || info->input == &inputs[2])
	dly->disconnectTo(info);
    else
	osc->disconnectTo(info);
}

Chorus::~Chorus()
{
    inputs.removeElement( 4 );
    inputs.removeElement( 3 );
    inputs.removeElement( 2 );
    inputs.removeElement( 1 );
    inputs.removeElement( 0 );
    outputs.removeElement( 0 );

    delete( dly );
    delete( osc );
}
