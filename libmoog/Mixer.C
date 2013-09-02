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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/debug.h>

/* $Id: Mixer.C,v 1.24.4.1 2000/02/24 00:44:03 larocca Exp $ */

#include "Mixer.h"
#include "Scheduler.h"
#include "ConnectionManager.h"

Mixer::Mixer(int n, int objs, ... )
{
    init( n );

    va_list ap;
    va_start( ap, objs );
    for( int i = 0; i < objs; i++ )
    {
	char tmpname[20];
	sprintf( tmpname, "sig%d", i );

	ConnectionManager::connect( (MoogObject*)va_arg( ap, MoogObject* ), 
				    "sig",
				    this, 
				    tmpname );
    }
    va_end( ap );
}

void
Mixer::init( int n )
{
    numChannels = n;

    char tmpname[20];
    int i;
    
    double mix = 2.0 / n;

    for (i = 0; i < numChannels; i++) 
    {
	sprintf(tmpname, "amp%d", i);
	addInput(tmpname);
	sprintf(tmpname, "sig%d", i);
	addInput(tmpname);
	set( I_MIX_AMP( i ), mix );
    }
    
    output = addOutput("sig", true);
    inputData = new double *[numChannels * 2];
    
    for (int i = 0; i < numChannels * 2; i++)
	inputData[i] = inputs[i].data;

    Scheduler::scheduleSampleRate(this, true);
}

Mixer::~Mixer()
{
    delete[] inputData;
}

void Mixer::connectTo(ConnectionInfo *info)
{
    MoogObject::connectTo(info);

    for (int i = 0; i < numChannels * 2; i++)
	inputData[i] = inputs[i].data;
}

void Mixer::disconnectTo(ConnectionInfo *info)
{
    MoogObject::disconnectTo(info);

    for (int i = 0; i < numChannels * 2; i++)
	inputData[i] = inputs[i].data;
}

void Mixer::sampleGo()
{
    register int i = numChannels * 2;
    double tmpdata = 0;
    
    while(i > 0)
    {
	i--;
	tmpdata += *inputData[i] * *inputData[i - 1];
	i--;
    }
	if (tmpdata>1.0)tmpdata=1;
	if (tmpdata<-1.0)tmpdata=-1;

    output->setData(tmpdata);

    MOOG_DEBUG("%f", tmpdata);
}

