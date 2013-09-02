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
#include "ResonantLowPass.h"
#include "ConnectionInfo.h"
#include "Scheduler.h"
#include "bilinear.h"

FilterPrototype::FilterPrototype(int ns)
{
    numSections = ns;
    coef = new BiQuad[ns];
}

FilterPrototype::~FilterPrototype()
{
    delete coef;
}

void FilterPrototype::setSectionCoef(int section,
				     double a0,
				     double a1,
				     double a2,
				     double b0,
				     double b1,
				     double b2)
{
    if (section < 0 || section >= numSections)
    {
	debug(DEBUG_APPERROR, "FilterPrototype::setSectionCoef section of of range");
	return;
    }

    coef[section].a0 = a0;
    coef[section].a1 = a1;
    coef[section].a2 = a2;
    coef[section].b0 = b0;
    coef[section].b1 = b1;
    coef[section].b2 = b2;
}

/* ************************ */

static FilterPrototype basicLowPassFilter(2);
static int basicLowPassFilterInitialized = 0;

void ResonantLowPass_gainChanged(MoogObject *o, double data, long userData)
{
    ((ResonantLowPass*)o)->gainChanged(data);
}

void ResonantLowPass_cutoffChanged(MoogObject *o, double data, long userData)
{
    ((ResonantLowPass*)o)->cutoffChanged(data);
}

void ResonantLowPass_resonanceChanged(MoogObject *o,double data, long userData)
{
    ((ResonantLowPass*)o)->resonanceChanged(data);
}


ResonantLowPass::ResonantLowPass()
{
    if (!basicLowPassFilterInitialized)
    {
	basicLowPassFilter.setSectionCoef(0, 1.0, 0, 0, 1.0, 0.765367, 1.0);
	basicLowPassFilter.setSectionCoef(1, 1.0, 0, 0, 1.0, 1.847759, 1.0);
    }

    proto = &basicLowPassFilter;
    init();
}

ResonantLowPass::ResonantLowPass(FilterPrototype *fp)
{
    proto = fp;
    init();
}

void ResonantLowPass::init()
{
    len = proto->numSections;

    hist = new double[2 * len];
    coef = new double[4 * len];

    memset(hist, 0, sizeof(double) * 2 * len);
    memset(coef, 0, sizeof(double) * 4 * len);

    addPorts( "sig",       INPUT,  NULL,
	      "gain",      INPUT, ResonantLowPass_gainChanged, 0, 1,
	      "cutoff",    INPUT, ResonantLowPass_cutoffChanged, 0, 1,
	      "resonance", INPUT, ResonantLowPass_resonanceChanged, 0, 1,
	      "sig",       OUTPUT, true, 
	      NULL);

    output = &outputs[0];
    inSig = inputs[0].data;
    inGain = inputs[1].data;
    inCutoff = inputs[2].data;
    inResonance = inputs[3].data;

    Scheduler::scheduleSampleRate(this, true);
}

ResonantLowPass::~ResonantLowPass()
{
    delete[] hist;
    delete[] coef;
}

void ResonantLowPass::connectTo(ConnectionInfo *info)
{
    MoogObject::connectTo(info);
    inSig = inputs[0].data;
    inGain = inputs[1].data;
    inCutoff = inputs[2].data;
    inResonance = inputs[3].data;
}

void ResonantLowPass::disconnectTo(ConnectionInfo *info)
{
    MoogObject::disconnectTo(info);
    inSig = inputs[0].data;
    inGain = inputs[1].data;
    inCutoff = inputs[2].data;
    inResonance = inputs[3].data;
}

void ResonantLowPass::gainChanged(double data)
{
    gain = data;
    recalcFilter();
}

void ResonantLowPass::cutoffChanged(double data)
{
    cutoff = data;
    recalcFilter();
}

void ResonantLowPass::resonanceChanged(double data)
{
    resonance = data;
    recalcFilter();
}

void ResonantLowPass::recalcFilter()
{
    double *coefptr = coef;
    fixedGain = gain;
    
    for (int i = 0; i < len; i++)
    {
	szxform(proto->coef[i].a0,
		proto->coef[i].a1,
		proto->coef[i].a2,
		proto->coef[i].b0,
		// the resonance of the filter, the Q, is actually the Q
		// of each section of the filter multiplied together.
		// but the actual Q should go from 1 to 1000.  Since this
		// is a two section filter, and we use the same Q for each
		// section, the max Q should be sqrt(1000) or 30.6227
		// FIXME: resonance should probably not be linear
		proto->coef[i].b1 / (resonance * 30.6227 + 1),
		proto->coef[i].b2,
		cutoff * Scheduler::nyquistFreq, 
		Scheduler::sampleRate, 
		&fixedGain,
		coefptr);

 	coefptr += 4;
    }
}

void ResonantLowPass::sampleGo()
{
    double *coefptr = coef;
    double *hist1ptr, *hist2ptr;
    double newhist, hist1, hist2;
    double tmpOutput;

    hist1ptr = hist;
    hist2ptr = hist + 1;

    tmpOutput = *inSig * fixedGain;

    for (int i = 0; i < len; i++)
    {
	hist1 = *hist1ptr;
	hist2 = *hist2ptr;

	tmpOutput = tmpOutput - hist1 * (*coefptr++);

	newhist = tmpOutput - hist2 * (*coefptr++);

	tmpOutput = newhist + hist1 * (*coefptr++);

	tmpOutput = tmpOutput + hist2 * (*coefptr++);

	*hist2ptr++ = *hist1ptr;
	*hist1ptr++ = newhist;
	hist1ptr++;
	hist2ptr++;
    }

   output->setData(tmpOutput);
}
