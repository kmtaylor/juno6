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
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/soundcard.h>

#include <util/debug.h>

#include "MidiInput.h"
#include "Scheduler.h"
#include "pitch.h"

#define READ1 if ( !inCommand )\
{\
  read( midiFd, &data[ 0 ], 1 );\
}\

#define READ2 if ( inCommand )\
{\
read( midiFd, &data[ 1 ], 1 );\
}\
else\
{\
read( midiFd, &data[ 0 ], 1 );\
read( midiFd, &data[ 1 ], 1 );\
}\

void* midi_input_run( void* data )
{
    return(((MidiInput *)data)->run());
}

MidiInput::MidiInput( const char* device, int nvoices )
{
    char tmpname[ 16 ];

    running = 0;
    this->nvoices = nvoices;
    voices = new midi_voice[ nvoices ];
    pthread_mutex_init(&startStopLock, NULL);
    lastNote = -1;

    if ( openDevice( device, MIDI_READ ) < 0 )
	return;
    
    addOutput( "bend", false );

    for( int i = 0; i < nvoices; i++ )
    {
	voices[ i ].note = -1;
	sprintf(tmpname, "sig%d", i);
	voices[ i ].pitchOutput = addOutput(tmpname, false);
	sprintf(tmpname, "amp%d", i);
	voices[ i ].gateOutput = addOutput(tmpname, false);
    }

}

MidiInput::~MidiInput()
{
    if (running)
	stop();

    delete[]( voices );
}

Output* 
MidiInput::getOutput( const char* n )
{
    Output* retval = MoogObject::getOutput( n );

    if ( retval == NULL && strlen( n ) > 3 && strncmp( "ctl", n, 3 ) == 0 )
    {
	retval = addOutput( n, false );
    }

    return( retval );
}

void 
MidiInput::start()
{
    if (midiFd < 0)
    {
	debug(DEBUG_APPERROR, "MidiInput fd not opened. Refusing to start.");
	return;
    }

    pthread_mutex_lock(&startStopLock);

    if ( running == 1 )
    {
	debug( DEBUG_APPERROR, "MidiInput already started!" );
    }
    else
    {
	running = 1;
	pthread_create( &midiThread, NULL, midi_input_run, this );
    }
    pthread_mutex_unlock(&startStopLock);
}

void 
MidiInput::stop()
{
    pthread_mutex_lock(&startStopLock);
    if (running)
    {
	pthread_cancel(midiThread);
	pthread_join(midiThread, NULL);
	running = 0;
    }
    pthread_mutex_unlock(&startStopLock);
}

void*
MidiInput::run()
{
    unsigned char tmp;
    unsigned char cmd     = 0;
    unsigned char channel = 0;
    unsigned char data[2];
    int inCommand = 0;
    char ctlName[ 10 ]; //format is "ctl00-000"

    while( running )
    {
	read( midiFd, &tmp, 1 );
    
	if ( tmp == 248 ) // throw away timing messages they mess us up!
	{
	    //debug( DEBUG_STATUS, "." );
	    continue;
	}

	if ( tmp < 128 )
	{
	    data[0] = tmp;
	    inCommand = 1;
	}
	
	else
	{
	    cmd     = tmp & 0xF0;
	    channel = tmp & 0x0F;
	    inCommand = 0;
	}
	
	switch( cmd )
	{
	case MIDI_NOTEOFF: //0x80
	    READ2;
	    doNoteOff( channel, data[0], data[1] );
	    break;
	    
	case MIDI_NOTEON: //0x90
	    READ2;
	    if ( data[1] == 0 )
	    {
		doNoteOff( channel, data[0], data[1] );
	    }
	    else
	    {
		doNoteOn( channel, data[0], data[1] );
	    }
	    break;
	    
	case MIDI_KEY_PRESSURE: //0xA0
	    READ2;
	    debug( DEBUG_STATUS, "KEY PRESSURE ch=%d note=%d amount=%d\n", 
		   channel, cmd, data[ 0 ] );
	    break;
	    
	case MIDI_CTL_CHANGE: //0xB0
	    READ2;
	    debug( DEBUG_STATUS, "CTL_CHANGE %d %d %d\n", channel, data[ 0 ], data[ 1 ] );

	    sprintf( ctlName, "ctl%d-%d", channel, data[0] );
	    {
		Output* out;
		out = MoogObject::getOutput( ctlName );
		if ( out != NULL )
		    out->setData(data[ 1 ] / 127.0);
	    }

	    break;
	    
	case MIDI_PGM_CHANGE: //0xC0
	    READ1;
	    debug( DEBUG_STATUS, "PGM_CHANGE %d %d\n", channel, data[ 0 ] );
	    break;
	    
	case MIDI_CHN_PRESSURE: //0xD0
	    READ1;
	    debug( DEBUG_STATUS, "CHN_PRESSURE %d %d\n", channel, data[ 0 ] );
	    break;
	    
	case MIDI_PITCH_BEND: //0xE0
	    READ2;
	    doPitchBend( (( data[ 1 ] << 7 ) & 0xFF00) | data[ 0 ] );
	    break;
	    
	case MIDI_SYSTEM_PREFIX: //0xF0
	    switch( channel )
	    {
	    case 0:
		break;
	    }
	    break;
	    
	default:
	    debug( DEBUG_STATUS, "[%d]\n", cmd );
	}
    }

    pthread_exit(0);

    /* not reached */
    return( NULL );
}

void 
MidiInput::doNoteOn( unsigned int c, unsigned int n, unsigned int v )
{
    /* try to avoid re-using the same note as long as possible, so that
     * if it has a Release envelope, it will get as much time as possible
     */

    //debug( DEBUG_STATUS, "MidiInput::noteOn %d %d %d", c, n, v );
    int start = (lastNote + 1) % nvoices;
    int i = start;

    do
    {
	if ( voices[ i ].note == -1 )
	{
	    voices[ i ].note = n;
	    voices[ i ].pitchOutput->setData(CPS(midi_notes[n]));
	    voices[ i ].gateOutput->setData(v / 127.0);
	    lastNote = i;
	    break;
	}

	i = (i + 1) % nvoices;
    } while (i != start);
}

void 
MidiInput::doNoteOff( unsigned int c, unsigned int n, unsigned int v )
{
    //debug( DEBUG_STATUS, "MidiInput::noteOff %d %d %d", c, n, v );

    // stop all voices playing this note ( because of race condition? )
    for( int i = 0; i < nvoices; i++ )
    {
	if ( voices[ i ].note == (int)n )
	{
	    voices[ i ].note = -1;
	    // important to keep outputting the pitch signal though
	    voices[ i ].gateOutput->setData(0);
	}
    }
}


void 
MidiInput::doPitchBend( unsigned int amount )
{
    //debug( DEBUG_STATUS, "MidiInput::pitchBend %d", amount );

    double pitchBend = ( double )amount / 8192 - 1.0;

#ifdef KARL_BEND 
    for( int i = 0; i < nvoices; i++ )
    {
	if ( voices[ i ] != -1 )
	{
	    outputs[ O_MIDI_SIG(i) ].data = 
	      CPS( midi_notes[ voices[ i ] ] ) * pitchBend;
	}
    }
#else
    outputs[ 0 ].setData(pitchBend);
#endif
}

bool 
MidiInput::isOpen() { return (midiFd >= 0); }

