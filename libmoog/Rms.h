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
 * $Revision: 1.8.4.1 $$Date: 2000/02/24 00:44:04 $
 */
#ifndef RMS_H
#define RMS_H

#include "MoogObject.h"

#define I_RMS_SIG   0
#define O_RMS_POWER 0

/* use one of these two methods for calculating the rms power of a signal */
#define RMS_LOWPASS

class Rms : public MoogObject 
{
    Output  *output;

    double    c1;
    double    c2;
    double    q;

    double   *inSig;

 public:
    Rms();

    void connectTo(ConnectionInfo *info);
    void disconnectTo(ConnectionInfo *info);

    void sampleGo();

    /* RTTI */
    const char *getClassName() { return "Rms"; }
};

#endif /* RMS_H */
