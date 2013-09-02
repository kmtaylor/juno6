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
#ifndef BALANCE_H
#define BALANCE_H

#include "MoogObject.h"
#include "Rms.h"

class Rms;

#define I_BAL_SIG1 0
#define I_BAL_SIG2 1
#define O_BAL_SIG  0

class Balance : public MoogObject 
{
 protected:
    Rms      rms1;
    Rms      rms2;

    Output  *output;

    double  *power1;
    double  *power2;
    double  *inSig;

 public:
    Balance();    
    ~Balance();

    //override because Rms has special handling and we've borrowed
    //its inputs
    void connectTo(ConnectionInfo *info);
    void disconnectTo(ConnectionInfo *info);

    void sampleGo();

    //override because both inputs are called 'sig'
    Input   *getInput(const char* n);      
    const char *getClassName() { return "Balance"; }
};

#endif /* BALANCE_H */
