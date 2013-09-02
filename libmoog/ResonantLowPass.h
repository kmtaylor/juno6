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
#ifndef _LIBMOOG_LOWPASS_H
#define _LIBMOOG_LOWPASS_H

#include "MoogObject.h"

struct BiQuad 
{
    double a0, a1, a2;
    double b0, b1, b2;
};

class FilterPrototype
{
    friend class ResonantLowPass;

    int numSections;
    BiQuad *coef;

 public:
    FilterPrototype(int);
    ~FilterPrototype();

    void setSectionCoef(int, double, double, double, double, double, double);
};

class ResonantLowPass : public MoogObject
{
    friend void ResonantLowPass_gainChanged(MoogObject *, double, long);
    friend void ResonantLowPass_cutoffChanged(MoogObject *, double, long);
    friend void ResonantLowPass_resonanceChanged(MoogObject *, double, long);

    int len;
    double *hist;
    double *coef;
    FilterPrototype *proto;
    
    double gain;
    double resonance;
    double cutoff;

    Output *output;
    double *inSig;
    double *inGain;
    double *inCutoff;
    double *inResonance;

    double fixedGain;

    void init();

    void gainChanged(double);
    void cutoffChanged(double);
    void resonanceChanged(double);

 public:

    ResonantLowPass();
    ResonantLowPass(FilterPrototype *);
    ~ResonantLowPass();
    
    void connectTo(ConnectionInfo *info);
    void disconnectTo(ConnectionInfo *info);

    void recalcFilter();
    void sampleGo();

    const char *getClassName() { return "ResonantLowPass"; }
};



#endif /* _LIBMOOG_LOWPASS_H */
