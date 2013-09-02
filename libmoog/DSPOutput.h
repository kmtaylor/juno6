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
 * $Revision: 1.9.4.1 $$Date: 2000/02/24 00:44:03 $
 */
#ifndef _DSPOutput_H
#define _DSPOutput_H

#include "MoogObject.h"

#define I_DSP_SIG(n) ((n) * 2)
#define I_DSP_AMP(n) ((n) * 2 + 1)

#define SAMPLE_RATE_44k 44100
#define SAMPLE_RATE_22k 22050
#define SAMPLE_RATE_8k   8000

#include "DSPDevice.h"

class DSPOutput : public MoogObject
{
    DSPDevice *dsp;
    int        myDsp;

    double    **inSig;
    double    **inAmp;

    void     (*dataWrittenCallback)(void);
    void       setup();

 public:
    DSPOutput(DSPDevice *dsp);
    DSPOutput(const char *device = "/dev/dsp",  
	      int rate = SAMPLE_RATE_44k, 
	      int channels = 1,
	      int numFrags = -1,
	      int fragSize = -1);

    ~DSPOutput();

    void   connectTo(ConnectionInfo *info);
    void   disconnectTo(ConnectionInfo *info);

    void   setDataWrittenCallback(void (*)(void));
    void   sampleGo();

    bool   isOpen() { return dsp->isOpen(); }
    
    /* RTTI */
    const char *getClassName() { return "DSPOutput"; }
};

#endif /* _DSPOutput_H */
