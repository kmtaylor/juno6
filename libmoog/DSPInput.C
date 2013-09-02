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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <util/debug.h>
#include <netinet/in.h>

#include "DSPInput.h"
#include "DSPDevice.h"
#include "Scheduler.h"

DSPInput::DSPInput(DSPDevice *_dsp)
{
    myDsp = 0;
    dsp = _dsp;

    setup();
}

DSPInput::DSPInput(char *device, int rate, int channels)
{
    myDsp = 1;
    dsp = new DSPDevice(device, DSP_READ, rate, channels);

    setup();
}

void DSPInput::setup()
{
    char tmpname[16];
    for(int i = 0; i < dsp->channels; i++)
    {
	sprintf(tmpname, "sig%d", i);
	addOutput(tmpname, true);
    }

    Scheduler::setSampleRate(dsp->getSampleRate());
    Scheduler::scheduleSampleRate(this, true);
}

DSPInput::~DSPInput()
{
    if (myDsp)
	delete dsp;
}

void DSPInput::sampleGo()
{
    for (int i = 0; i < dsp->channels; i++)
	outputs[i].data = ((double)*dsp->readDataPtr++) / 32768;

    dsp->checkFlushBuffers();
}

