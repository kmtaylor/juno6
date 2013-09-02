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
#ifndef _JUNO6_JUNO6_LFO_H
#define _JUNO6_JUNO6_LFO_H

#include <moog/MoogObject.h>
#include <moog/Oscillator.h>

class JunoControl;

class JunoLfo : public MoogObject
{
    friend void JunoLfo_lforateChanged(MoogObject *, double, long);
    friend void JunoLfo_lfodelayChanged(MoogObject *, double, long);
    friend void JunoLfo_lfomodeChanged(MoogObject *, double, long);
    friend void JunoLfo_lfotriggerChanged(MoogObject *, double, long);
    friend void JunoLfo_voicegateChanged(MoogObject *, double, long);

    Oscillator osc;

    double lforate;
    double lfodelay;
    int    lfomode;
    int    lfotrigger;
    int    voicegate;

    int    trigger;
    double lfoamp;

    void lforateChanged(double);
    void lfodelayChanged(double);
    void lfomodeChanged(double);
    void lfotriggerChanged(double);
    void voicegateChanged(int, double);

    void checkTrigger();

 public:
    JunoLfo(JunoControl *,int);
    ~JunoLfo();

    void controlGo();

    const char *getClassName() { return "JunoLfo"; }
};

#endif /* _JUNO6_JUNO6_LFO_H */
