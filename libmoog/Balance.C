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
#include "Balance.h"
#include "Rms.h"
#include "Scheduler.h"
#include "ConnectionInfo.h"

Balance::Balance()
{
    addPorts( "sig",  OUTPUT, true,
	      NULL);

    inputs.appendElement( rms1.getInput( I_RMS_SIG ) );
    inputs.appendElement( rms2.getInput( I_RMS_SIG ) );

    power1 = &rms1.getOutput( O_RMS_POWER )->data;
    power2 = &rms2.getOutput( O_RMS_POWER )->data;

    output = &outputs[ O_BAL_SIG ];  // just for optimization
    inSig = inputs[1].data;

    Scheduler::scheduleSampleRate( this, true );
}

Balance::~Balance()
{ 
    /* unborrow the inputs rms so they won't get double deleted 
     * from the back is more efficient for the SimpleArray
     */

    inputs.removeElement( I_BAL_SIG2 );
    inputs.removeElement( I_BAL_SIG1 );
}

void Balance::connectTo(ConnectionInfo *info)
{
    if (info->input == rms1.getInput(0))
	rms1.connectTo(info);
    else
	rms2.connectTo(info);

    inSig = inputs[1].data;
}

void Balance::disconnectTo(ConnectionInfo *info)
{
    if (info->input == rms1.getInput(0))
	rms1.disconnectTo(info);
    else
	rms2.disconnectTo(info);

    inSig = inputs[1].data;
}

Input *Balance::getInput(const char *name)
{
    if (strcmp(name, "pow") == 0)
	return rms1.getInput("sig");
    else if (strcmp(name, "sig") == 0)
	return rms2.getInput("sig");

    return NULL;
}

void Balance::sampleGo()
{
    if ( *power2 == 0 )
	output->setData( 0 );
    else
	output->setData( *inSig * ( *power1 / *power2 ) );
}
