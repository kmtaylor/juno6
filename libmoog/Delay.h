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
 * $Revision: 1.13.4.1 $$Date: 2000/02/24 00:44:03 $
 */
#ifndef _DELAY_H
#define _DELAY_H

#include "MoogObject.h"

#define I_DLY_SIG      0
#define I_DLY_TIME     1
#define I_DLY_FEEDBACK 2
#define I_DLY_MIX      3
#define O_DLY_SIG      0

class Delay : public MoogObject
{
 private:
    friend void delay_mix_changed( MoogObject*, double, long );
    friend void delay_feedback_changed( MoogObject*, double, long );

    void mixChanged( double );
    void feedbackChanged( double );

    int        dsize;
    double    *dline;
    Output*    output;
    int        wpos;

    double    *inSig;
    double    *inTime;
    double    *inFeedback;
    double    *inMix;

    double     feedback;
    double     mix;

 public:
    Delay(int buffSize = 44000);

    void connectTo(ConnectionInfo *info);
    void disconnectTo(ConnectionInfo *info);

    double getTap( double dtime );
    void sampleGo();
    const char *getClassName() { return "Delay"; }
};


#endif /* _DELAY_H */
