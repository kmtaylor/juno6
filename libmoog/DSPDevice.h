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
 * $Revision: 1.17.4.1 $$Date: 2000/02/24 00:44:03 $
 */
#ifndef _DSPDEVICE_H
#define _DSPDEVICE_H

#include <unistd.h>
#include <util/fastmemcpy.h>
#define DSP_READ       1
#define DSP_WRITE      2
#define DSP_FULLDUPLEX 3

class DSPDevice
{
#ifdef POSIX
    int    setBits(int);
    int    setChannels(int);
    int    setSampleRate(int);
    int    setFragments(int, int);
#else
    int Play(void *data,int len);
#endif 
    int    mode;
    int    soundFd;

    int    bits;
 public:   //make channels public for a speed optimization

    int    channels;
 private:
    int    sampleRate;
    int    numFrags;
    int    fragSize;
    int    fragsAvail;
    
    short *readBuffBase;
    short *readBuffEnd;

    short *writeBuffBase;
    short *writeBuffEnd;

    void   initVars();

 public:

    /* this is hardcoding 16 bit sample resoution ... */
    short *readDataPtr;
    short *writeDataPtr;

    int checkFlushBuffers()
    {
	int retval = 0;

#ifdef POSIX
	if ((mode & DSP_READ) && readDataPtr >= readBuffEnd)
	{
	    read(soundFd, readBuffBase, fragSize);
	    readDataPtr = readBuffBase;
	    retval |= DSP_READ;
	}
	if ((mode & DSP_WRITE) && writeDataPtr >= writeBuffEnd)
	{
	    write(soundFd, writeBuffBase, fragSize);
	    writeDataPtr = writeBuffBase;
	    retval |= DSP_WRITE;
	}
#else
	if (writeDataPtr>=writeBuffEnd)
	{
	    int tmpFragSize = fragSize;
	    int t;
	
	    int pos=0;
	    //first type
	    while((t = Play(writeBuffBase+pos,tmpFragSize))<tmpFragSize)
	    {
		tmpFragSize = tmpFragSize - t;
		pos+=t; 
	    }
	    //printf("%d written\n",t);
	    writeDataPtr = writeBuffBase;
	
	
	    /*t = Play(writeBuffBase,fragSize);
	    if (t<fragSize)
	    {
		memcpy(writeBuffBase,writeBuffBase+t,fragSize-t);
		writeDataPtr=writeBuffBase+t;
	    } else
	    {
		writeDataPtr=writeBuffBase;	
	    }
	    */ 	
	    retval|=DSP_WRITE;	
	}
#endif

	return retval;
    }

    DSPDevice(const char *dev = "/dev/dsp", 
	      int mode = DSP_WRITE, 
	      int sampleRate = 44100, 
	      int channels = 2, 
	      int numFrags = -1, 
	      int fragSize = -1);

    ~DSPDevice();

    bool   isOpen() { return (soundFd >= 0); }
    int    getSampleRate() { return sampleRate; }

    /* channels variable has been made public as a speed optimization */
    //int    getChannels() { return channels; }
};

#endif /* _DSPDEVICE_H */
