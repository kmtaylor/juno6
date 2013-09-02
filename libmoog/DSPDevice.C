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
#include <sys/types.h>
#include <sys/soundcard.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
 
#include <util/debug.h>

#include "DSPDevice.h"
#include "Scheduler.h"

#define DEF_NUM_FRAGS  3
#define DEF_FRAG_SIZE  512

DSPDevice::DSPDevice(const char *_device, 
		     int         _mode, 
		     int         _sampleRate, 
		     int         _channels, 
		     int         _numFrags, 
		     int         _fragSize)
{
    int openMode;

    mode = _mode;

    switch(mode)
    {
    case DSP_READ:
	openMode = O_RDONLY;
	break;
    case DSP_WRITE:
	openMode = O_WRONLY;
	break;
    case DSP_FULLDUPLEX:
	openMode = O_RDWR;
	break;
    default:
	debug(DEBUG_APPERROR, "DSPDevice: unsupported mode");
	goto out;
    }

    if ((soundFd = open(_device, openMode)) <0)
    {
	debug(DEBUG_SYSERROR,"DSPDevice: can't open %s: ", _device);
	goto out;
    }

    debug(DEBUG_APPMSG1, "DSPDevice: device %s opened for %s", 
	  _device, 
	  (mode == DSP_READ)?"read":(mode == DSP_WRITE)?"write":"duplex");

    if (setBits(16) < 0)
	goto out_close;

    if (setChannels(_channels) < 0)
	goto out_close;

    if (setSampleRate(_sampleRate) < 0)
	goto out_close;

    if (_numFrags == -1)
	_numFrags = DEF_NUM_FRAGS;

    if (_fragSize == -1)
	_fragSize = DEF_FRAG_SIZE;

    if (setFragments(_numFrags, _fragSize) < 0)
	goto out_close;

    if (mode & DSP_READ)
    {
	readDataPtr = readBuffBase = new short[(fragSize >> 1)];
	readBuffEnd = readBuffBase + (fragSize >> 1);
    }

    if (mode & DSP_WRITE)
    {
	writeDataPtr = writeBuffBase = new short[(fragSize >> 1)];
	writeBuffEnd = writeBuffBase + (fragSize >> 1);
    }

    return;

 out_close:
    close(soundFd);
    soundFd = -1;
 out:
    //FIXME: this is a hack so programs won't segfault if /dev/dsp open fails
    channels = _channels;
    sampleRate = _sampleRate;
    return;
}

DSPDevice::~DSPDevice()
{
    if (soundFd < 0)
	return;

    close(soundFd);
}

int DSPDevice::setBits(int _bits)
{
    int format, pformat;

    switch(_bits)
    {
    case 8:
	format = AFMT_S8;
	break;
    case 16:
	format = AFMT_S16_LE;
	break;
    default:
	debug(DEBUG_APPERROR, "unknown (unsupported) bitrate %d", bits);
	return -1;
    }
    
    pformat = format;
    
    if (ioctl(soundFd, SNDCTL_DSP_SETFMT, &pformat) == -1)  
    {
	debug(DEBUG_SYSERROR, "DSPDevice ioctl error setting audio format");  
	return -1;
    }  

    if (pformat != format)  
    {  
	debug(DEBUG_SYSERROR, "device doesn't support audio format.");
	return -1;
    }

    debug(DEBUG_STATUS, "DSPDevice new bits: %d", _bits);
    bits = _bits;
    return 0;
}

int DSPDevice::setChannels(int _channels)
{
    int pchannels = _channels;
    
    if (_channels==2)pchannels=1; //Stereo
    else pchannels=0;	//Mono
    if (ioctl(soundFd, SNDCTL_DSP_STEREO, &pchannels) == -1)
    {
	debug(DEBUG_SYSERROR, "DSPDevice: ioctl Stereo error setting num channels");
	return -1;
    }

    
    /*if (ioctl(soundFd, SNDCTL_DSP_CHANNELS, &pchannels) == -1)
    {
	debug(DEBUG_SYSERROR, "DSPDevice: ioctl error setting num channels");
	return -1;
    }*/
    
   /* if (pchannels != _channels)
    {
	debug(DEBUG_SYSERROR, "DSPDevice: num channels (%d) not supported",
	      _channels);
	return -1;
    }*/

    debug(DEBUG_APPMSG1, "DSPDevice new channels: %d", _channels);
    channels = _channels;
    return 0;
}

int DSPDevice::setSampleRate(int _sampleRate)
{
    int psampleRate = _sampleRate;

    if (ioctl(soundFd, SNDCTL_DSP_SPEED, &psampleRate) == -1)
    {
	debug(DEBUG_SYSERROR, "DSPDevice: ioctl error setting sampleRate");
	return -1;
    }

    /* this is not considered an error, because in reality it will often
     * vary a bit from the frequency we requested, but not by much
     */
    if (psampleRate != _sampleRate)
	debug(DEBUG_STATUS, 
	      "DSPDevice: Warning: sampleRate: requested %d got %d", 
	      _sampleRate, 
	      psampleRate);
    
    debug(DEBUG_APPMSG1, "DSPDevice new sampleRate: %d", psampleRate);
    sampleRate = psampleRate;
    return 0;
}

int DSPDevice::setFragments(int _numFrags, int _fragSize)
{
    int fragPower;

    /* _fragSize must be a power of 2, verify and find the power */
    for (fragPower = 0; fragPower < 32; fragPower++)
    {
	if (_fragSize & (1 << fragPower))
	    break;
    }

    if (_fragSize != (1 << fragPower))
    {
	debug(DEBUG_APPERROR, "fragSize (%d) is not power of 2", fragSize);
	return -1;
    }

    /* high 16 bits of fragParm is number of buffers
     * low  16 bits of fragParm is (size of buffer) given as (1 << x)
     */

    int fragParm = (_numFrags << 16) | (fragPower);


    debug(DEBUG_STATUS, "DSPDevice: setting frag: fragParm %08x", fragParm);

    if (ioctl(soundFd, SNDCTL_DSP_SETFRAGMENT, &fragParm) < 0)
    {
	debug(DEBUG_SYSERROR, "DSPDevice: ioctl error setting fragments");
	return -1;
    }
    
    audio_buf_info info;
    
    if (ioctl(soundFd, 
	      (mode == DSP_READ) ? SNDCTL_DSP_GETISPACE :SNDCTL_DSP_GETOSPACE, 
	      &info) < 0)
    {
	debug(DEBUG_SYSERROR, "DSPDevice: ioctl error getting frag info");
	return -1;
    }

    debug(DEBUG_APPMSG1, "DSPDevice: fragments set %d of %d size",
	  info.fragstotal,
	  info.fragsize);
    numFrags = info.fragstotal;
    fragSize = info.fragsize;
    return 0;
}
