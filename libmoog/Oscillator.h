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
 * $Revision: 1.23.4.1 $$Date: 2000/02/24 00:44:03 $
 */
#ifndef _OSCILLATOR_H
#define _OSCILLATOR_H

#include "MoogObject.h"

class DataBlock;

#define I_OSC_FRQ 0
#define I_OSC_AMP 1
#define I_OSC_ZRO 2
#define O_OSC_SIG 0

class Oscillator : public MoogObject 
{
 protected:
    friend void oscillator_frq_changed( MoogObject*, double, long );
    friend void oscillator_sync_changed( MoogObject*, double, long );

    void        frqChanged( double frq );
    void        syncChanged( double frq );

    int         lastTrigger;

    Output     *output;
    double     *inFrq;
    double     *inAmp;
    double     *inZro;
    double     *inSync;

    double     *waveData;
    int         waveDataLen;
    double      pos;
    double      scale;
    double      speed;

    void        init( DataBlock* w );

 public:
    Oscillator( DataBlock* = NULL );
    Oscillator( DataBlock* w, double frq, double amp, double zro);

    /* virtual overrides */
    void        connectTo(ConnectionInfo *info);
    void        disconnectTo(ConnectionInfo *info);

    void        setWaveData(DataBlock *);
    void        sampleGo();
    const char *getClassName() { return "Oscillator"; }
};

#endif /* _OSCILLATOR_H */







