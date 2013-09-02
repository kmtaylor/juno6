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
 * $Revision: 1.11.4.1 $$Date: 2000/02/24 00:44:03 $
 */
#ifndef _ATTENUATOR_H
#define _ATTENUATOR_H

#include "MoogObject.h"

#define I_ATT_SIG 0
#define I_ATT_AMP 1
#define I_ATT_ZRO 2

#define O_ATT_SIG 0

class Attenuator : public MoogObject 
{
 private:
    Output* output;
    
    double *inSig;
    double *inAmp;
    double *inZro;

 public:
    Attenuator( double _amp = 0, double _zro = 0 );

    void connectTo(ConnectionInfo *info);
    void disconnectTo(ConnectionInfo *info);

    void sampleGo();
    const char *getClassName() { return "Attenuator"; }
};

#endif /* _ATTENUATOR_H */
