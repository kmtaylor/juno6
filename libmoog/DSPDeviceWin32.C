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
 #include <util/debug.h>

#include "DSPDevice.h"
#include "Scheduler.h"
#include "afmt.h"

#define DEF_NUM_FRAGS  3
#define DEF_FRAG_SIZE  512

extern "C"
{
extern int init(int rate,int channels,int format,int flags);
extern void uninit();
extern void reset();
extern void audio_pause();
extern void audio_resume();
extern int get_space();
extern int play(void *data,int len,int flags);
extern float get_delay();


}




DSPDevice::DSPDevice(const char *_device, 
		     int         _mode, 
		     int         _sampleRate, 
		     int         _channels, 
		     int         _numFrags, 
		     int         _fragSize)
{
    if (_numFrags == -1)
	_numFrags = DEF_NUM_FRAGS;

    if (_fragSize == -1)
	_fragSize = DEF_FRAG_SIZE;
	fragSize = _fragSize;
        numFrags = _numFrags;
if (init(_sampleRate,_channels,AFMT_S16_LE,0)==0)
	{
	soundFd=-1;
	channels=_channels;
	sampleRate=_sampleRate;
	puts("Error opening sound device");
	return;
	}else
soundFd=1;
puts("dsp output open");

	channels=_channels;
	sampleRate=_sampleRate;
        mode=_mode;
	bits = 16;
    if (mode & DSP_WRITE)
    {
	writeDataPtr = writeBuffBase = new short[(fragSize >> 1)];
	writeBuffEnd = writeBuffBase + (fragSize >> 1);
    }

    return;
}
int DSPDevice::Play(void *data,int len)
{
//printf("try to write %d\n",len);
int r = play(data,len,0);
//printf("%d written\n",r);
return r;
}

DSPDevice::~DSPDevice()
{
    if (soundFd < 0)
	return;

    uninit();
}
