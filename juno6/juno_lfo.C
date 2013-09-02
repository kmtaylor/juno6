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
#include "juno_lfo.h"
#include "juno_control.h"

void JunoLfo_lforateChanged(MoogObject *o, double data, long userData)
{
    ((JunoLfo*)o)->lforateChanged(data);
}

void JunoLfo_lfodelayChanged(MoogObject *o, double data, long userData)
{
    ((JunoLfo*)o)->lfodelayChanged(data);
}

void JunoLfo_lfomodeChanged(MoogObject *o, double data, long userData)
{
    ((JunoLfo*)o)->lfomodeChanged(data);
}

void JunoLfo_lfotriggerChanged(MoogObject *o, double data, long userData)
{
    ((JunoLfo*)o)->lfotriggerChanged(data);
}

void JunoLfo_voicegateChanged(MoogObject *o, double data, long userData)
{
    ((JunoLfo*)o)->voicegateChanged(userData, data);
}

JunoLfo::JunoLfo(JunoControl *jc,int voices)
{
    osc.setWaveData(wg_tri(4096));
  
    addInput("lfo_rate", JunoLfo_lforateChanged, 0, 1);
    addInput("lfo_delay", JunoLfo_lfodelayChanged, 0, 1);
    addInput("lfo_mode", JunoLfo_lfomodeChanged, 0, 1);
    addInput("lfo_trigger", JunoLfo_lfotriggerChanged, 0, 1);
  
    for (int i=0;i<voices;i++)
    {
    char *tmpgate = new char[15];
    sprintf(tmpgate,"voice%d_gate",i);
    addInput(tmpgate, JunoLfo_voicegateChanged, i, 1);
    }
/*
    addInput("voice1_gate", JunoLfo_voicegateChanged, 1, 1);
    addInput("voice2_gate", JunoLfo_voicegateChanged, 2, 1);
    addInput("voice3_gate", JunoLfo_voicegateChanged, 3, 1);
    addInput("voice4_gate", JunoLfo_voicegateChanged, 4, 1);
    addInput("voice5_gate", JunoLfo_voicegateChanged, 5, 1);
*/
    PATCH(jc, "lfo_rate", this, "lfo_rate");
    PATCH(jc, "lfo_delay", this, "lfo_delay");
    PATCH(jc, "lfo_mode", this, "lfo_mode");
    PATCH(jc, "lfo_trigger", this, "lfo_trigger");
for (int i=0;i<voices;i++)
    {
    char *tmpgate = new char[15];
    sprintf(tmpgate,"voice%d_gate",i);
    PATCH(jc, tmpgate, this, tmpgate);
    }
   /* PATCH(jc, "voice0_gate", this, "voice0_gate");
    PATCH(jc, "voice1_gate", this, "voice1_gate");
    PATCH(jc, "voice2_gate", this, "voice2_gate");
    PATCH(jc, "voice3_gate", this, "voice3_gate");
    PATCH(jc, "voice4_gate", this, "voice4_gate");
    PATCH(jc, "voice5_gate", this, "voice5_gate");*/
    outputs.appendElement(osc.getOutput(0));
    outputs.appendElement(osc.getOutput(1));
}

JunoLfo::~JunoLfo()
{
    outputs.removeElement(1);
    outputs.removeElement(0);
}

void JunoLfo::lforateChanged(double data)
{
    //printf("lfo_rate is %f\n", data);
    lforate = pow( data, 2.5 );

    /* range is .125 HZ to 20.125 HZ */
    lforate *= CPS(20);
    lforate += CPS(.0625);

    osc.set("frq", lforate);
}

void JunoLfo::lfodelayChanged(double data)
{
    if (data == 0)
	lfodelay = 1;
    else
	lfodelay = -log(data * .90) * .001;

    //printf("lfodelay is %f (%f)\n", lfodelay, data);
}

void JunoLfo::lfomodeChanged(double data)
{
    //printf("lfo_mode is %f\n", data);
    lfomode = (data != 0);
    trigger = !lfotrigger;

    checkTrigger();
}

void JunoLfo::lfotriggerChanged(double data)
{
    //printf("lfo_trigger is %f\n", data);
    lfotrigger = (data != 0);
    checkTrigger();
}

void JunoLfo::voicegateChanged(int which, double data)
{
    if (data)
	voicegate |= 1 << which;
    else
	voicegate &= ~(1 << which);

    checkTrigger();
}

void JunoLfo::checkTrigger()
{
    int newTrigger = (lfomode == 0) ? (voicegate != 0) : lfotrigger;

    if (newTrigger != trigger)
    {
	trigger = newTrigger;

	if (!trigger)
	{
	    /* note the slight oddity: the juno doesn't turn off the 
	     * lfo when the last key is released and the trigger mode
	     * is 'auto' but it does reset it when the trigger is 
	     * re-asserted (below).  when trigger mode is manual, 
	     * however, it is shut off 
	     */
	    
	    if (isControlScheduled())
		Scheduler::scheduleControlRate(this, false);

	    if (lfomode == 1)
		osc.set("amp", 0);
	}
	else
	{
	    osc.set("amp", 0);
	    lfoamp = 0;

	    if (!isControlScheduled())
		Scheduler::scheduleControlRate(this, true);
	}
    }
}

void JunoLfo::controlGo()
{
    //printf("JunoLfo::controlGo %f %f\n", lfoamp, lfodelay);

    lfoamp += lfodelay;

    if (lfoamp >= 1.0)
    {
	osc.set("amp", 1.0);
	Scheduler::scheduleControlRate(this, false);
    }
    else
    {
	osc.set("amp", lfoamp);
    }
}
