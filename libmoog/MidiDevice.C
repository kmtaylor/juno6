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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

#include "MidiDevice.h"

#define PARITY_NONE 0
#define PARITY_EVEN 1
#define PARITY_ODD  2

MidiDevice::MidiDevice()
{
    midiFd = -1;
}

MidiDevice::~MidiDevice()
{
    closeDevice();
}

int set_comm_params(int fd, int baud_rate, int bits, int parity, int stop)
{
    struct termios tios;

    if (tcgetattr(fd, &tios) < 0)
    {
        debug(DEBUG_ERROR, "can't get termios data");
        return -1;
    }

    //cfmakeraw(&tios);
    cfsetospeed(&tios, baud_rate);
    cfsetispeed(&tios, baud_rate);

    tios.c_cflag &= ~CSIZE;

    switch(bits)
    {
    case 5:
        tios.c_cflag |= CS5;
        break;
    case 6:
        tios.c_cflag |= CS6;
        break;
    case 7:
        tios.c_cflag |= CS7;
        break;
    case 8:
        tios.c_cflag |= CS8;
        break;
    }

    if (parity)
    {
        tios.c_cflag |= PARENB;
        if (parity == PARITY_ODD)
            tios.c_cflag |= PARODD;
        else
            tios.c_cflag &= ~PARODD;
    }
    else
    {
        tios.c_cflag &= ~PARENB;
    }

    if (stop == 2)
        tios.c_cflag |= CSTOPB;
    else
        tios.c_cflag &= ~CSTOPB;
    
    if (tcsetattr(fd, TCSAFLUSH, &tios) < 0)
    {
        debug(DEBUG_ERROR, "error setting termios");
        return -1;
    }

    return 0;
}


int
MidiDevice::openDevice(char const* device, int _mode )
{
    mode = _mode;

    if ( midiFd >= 0 )
    {
	debug( DEBUG_APPERROR, "device MidiDevice already open! close it first" );
	return( -1 );
    }

    if ( ( midiFd = open( device, (mode == MIDI_READ) ? O_NONBLOCK|O_RDONLY : O_NONBLOCK|O_WRONLY, 0 ) ) < 0 ) 
    {
	debug( DEBUG_SYSERROR, "MidiDevice can't open %s", device);
	return( -1 );
    }

    if ( strcmp( device, "/dev/ttyS0" ) == 0 )
    {
	set_comm_params( midiFd, B38400, 8, PARITY_NONE, 1 );
    }

    /* the ALSA driver opens midiraw in O_NONBLOCK mode by default, in 
     * any case this shouldn't hurt the OSS driver 
     */
    int flags;

    if ((flags = fcntl(midiFd, F_GETFL)) < 0)
	debug(DEBUG_SYSERROR, "MidiDevice::openDevice fctnl 1");

    flags &= ~O_NONBLOCK;

    if (fcntl(midiFd, F_SETFL, flags) < 0)
    	debug(DEBUG_SYSERROR, "MidiDevice::openDevice fctnl 2");

    debug( DEBUG_APPMSG1, "MidiDevice: %s opened for read", device );

    return( 0 );
}

void 
MidiDevice::closeDevice()
{
    close(midiFd);
    midiFd = -1;
}
