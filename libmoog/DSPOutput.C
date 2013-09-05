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
#include <util/sio.h>

#include "DSPOutput.h"
#include "Scheduler.h"

/* $Id: DSPOutput.C,v 1.13.4.1 2000/02/24 00:44:03 larocca Exp $ */

DSPOutput::DSPOutput(DSPDevice *_dsp)
{
    dsp = _dsp;
    myDsp = 0;

    setup();
}

DSPOutput::DSPOutput(const char *device   /* = "/dev/dsp" */,
		     int         rate     /* = SAMPLE_RATE_44k */ ,
		     int         channels /* = 1 */,
		     int         numFrags /* = -1 */,
		     int         fragSize /* = -1 */)
{
    dsp = new DSPDevice(device, DSP_WRITE, rate, channels, numFrags, fragSize);
    myDsp = 1;

    setup();
}

void DSPOutput::setup()
{
    char tmpname[16];

    inSig = new double *[dsp->channels];
    inAmp = new double *[dsp->channels];

    for (int i = 0; i < dsp->channels; i++)
    {
	sprintf(tmpname, "sig%d", i);
	inSig[i] = addInput(tmpname )->data;
	sprintf(tmpname, "amp%d", i);
	inAmp[i] = addInput(tmpname )->data;
    }
    
    dataWrittenCallback = NULL;
    
    Scheduler::setSampleRate(dsp->getSampleRate());

    if (dsp->isOpen())
	Scheduler::scheduleSampleRate(this, true);
}

DSPOutput::~DSPOutput()
{
    if (myDsp)
	delete dsp;

    delete[] inSig;
    delete[] inAmp;
}

void DSPOutput::connectTo(ConnectionInfo *info)
{
    MoogObject::connectTo(info);

    for (int i = 0; i < dsp->channels; i++)
    {
	inSig[i] = inputs[2 * i].data;
	inAmp[i] = inputs[2 * i + 1].data;
    }
}

void DSPOutput::disconnectTo(ConnectionInfo *info)
{
    MoogObject::disconnectTo(info);

    for (int i = 0; i < dsp->channels; i++)
    {
	inSig[i] = inputs[2 * i].data;
	inAmp[i] = inputs[2 * i + 1].data;
    }
}

void DSPOutput::sampleGo()
{
    int moved;

    for (int i = 0; i < dsp->channels; i++)
    {
#if 1 /* Clip instead of wrap */
	int b = (int)((*inSig[i] * *inAmp[i]) * 32768);
	if (b > 32767) b = 32767;
	if (b < -32767) b = -32767;
	*dsp->writeDataPtr++ = b;
#else
	*dsp->writeDataPtr++ = (short)((*inSig[i] * *inAmp[i]) * 32768);
#endif
    }

    moved = dsp->checkFlushBuffers();

    if ((moved & DSP_WRITE) && dataWrittenCallback)
	dataWrittenCallback();
}

void DSPOutput::setDataWrittenCallback(void (*dwcb)(void))
{
    dataWrittenCallback = dwcb;
}
