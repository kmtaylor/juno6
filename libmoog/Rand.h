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
 * $Revision: 1.13.4.1 $$Date: 2000/02/24 00:44:04 $
 */
#ifndef RAND_H
#define RAND_H

#include "MoogObject.h"

#define I_RND_FRQ 0
#define I_RND_AMP 1
#define I_RND_ZRO 2
#define O_RND_SIG 0

#define RND_NORMAL 0
#define RND_HOLD   1 
#define RND_INT    2 // linear interpolation
#define RND_INTX   3 // exponential intepolation

class Rand : public MoogObject 
{
    friend void Rand_freqChanged(MoogObject *, double , long);

    void freqChanged(double);
    
 protected:
    Output   *output;
    double   *inFrq;
    double   *inAmp;
    double   *inZro;

    int        type;
    
    double     last;
    double     current;
    
    double     pos;
    double     step;
    
 public:
    Rand( int _type = RND_NORMAL, double amp = 1.0 );

    void       connectTo(ConnectionInfo *info);
    void       disconnectTo(ConnectionInfo *info);

    void       sampleGo();
    const char *getClassName() { return "Rand"; }
};

#endif /* RAND_H */
