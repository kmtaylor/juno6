/*
Win32 MIDI Implementation for Juno 666 project. (c) 2003 - 2004 Sebastian Gottschall

*/

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <util/debug.h>
#include <juno6/juno_gui.h>
#include "MidiInputWin32.h"
#include "Scheduler.h"
#include "pitch.h"
#include <windows.h>
#include <mmsystem.h>


/*
 * The 4 most significant bits of byte 0 specify the class of
 * the event:
 *
 *	0x8X = system level events,
 *	0x9X = device/port specific events, event[1] = device/port,
 *		The last 4 bits give the subtype:
 *			0x02	= Channel event (event[3] = chn).
 *			0x01	= note event (event[4] = note).
 *			(0x01 is not used alone but always with bit 0x02).
 *	       event[2] = MIDI message code (0x80=note off etc.)
 *
 */

#define EV_SEQ_LOCAL		0x80
#define EV_TIMING		0x81
#define EV_CHN_COMMON		0x92
#define EV_CHN_VOICE		0x93
#define EV_SYSEX		0x94
/*
 * Event types 200 to 220 are reserved for application use.
 * These numbers will not be used by the driver.
 */

/*
 * Events for event type EV_CHN_VOICE
 */

#define MIDI_NOTEOFF		0x80
#define MIDI_NOTEON		0x90
#define MIDI_KEY_PRESSURE	0xA0

/*
 * Events for event type EV_CHN_COMMON
 */

#define MIDI_CTL_CHANGE		0xB0
#define MIDI_PGM_CHANGE		0xC0
#define MIDI_CHN_PRESSURE	0xD0
#define MIDI_PITCH_BEND		0xE0

#define MIDI_SYSTEM_PREFIX	0xF0 

HMIDIIN handle;
MidiInput *input;
void CALLBACK MidiInProc(HMIDIIN hMidiIn,UINT wMsg,DWORD dwInstance,DWORD dwParam1,DWORD dwParam2);	

MidiInput::MidiInput( const char* device, int nvoices )
{
UINT devs=midiInGetNumDevs(); 


for (UINT i=0;i<devs;i++)
	{
	MIDIINCAPS caps;
	MMRESULT result = midiInGetDevCaps(i,&caps,sizeof(MIDIINCAPS));
	if (result!=MMSYSERR_NOERROR)
	 	{
		printf("Error opening Device :%d\n",result);
		continue;
		}
		
	printf("opening %d:%s \n",i,caps.szPname);	
	result = midiInOpen(&handle,i,(DWORD)MidiInProc,0,CALLBACK_FUNCTION);
	if (result!=MMSYSERR_NOERROR)
	 	{
		printf("Error init Device :%d\n",result);
		continue;
		}
	break;

	}

    char tmpname[ 16 ];
    running = 0;
    this->nvoices = nvoices;
    voices = new midi_voice[ nvoices ];
    lastNote = -1;
    
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
input = this;

    if ( running == 1 )
    {
	debug( DEBUG_APPERROR, "MidiInput already started!" );
    }
    else
    {
	running = 1;
	MMRESULT result = midiInStart(handle);
	if (result!=MMSYSERR_NOERROR)
	 	{
		printf("Error starting Device :%d\n",result);
		}
    }

}

void 
MidiInput::stop()
{
puts("stop midi device");
midiInStop(handle);
running = 0;
}


void CALLBACK MidiInProc(HMIDIIN hMidiIn,UINT wMsg,DWORD dwInstance,DWORD dwParam1,DWORD dwParam2)	
{
 unsigned char cmd     = 0;
 unsigned char channel = 0;
 unsigned char data[2];
 char ctlName[ 10 ]; //format is "ctl00-000"
if (wMsg==MM_MIM_DATA) //accept only complete events
{

 cmd = dwParam1&0xf0;
 channel = dwParam1&0x0f;
 data[0] = (dwParam1>>8) &0xff;
 data[1] = (dwParam1>>16)&0xff;
 
//printf("%d %X %d %d\n",channel,cmd,data[0],data[1]);

if (input==NULL)
	{
	return;
	}
switch( cmd )
	{
	case MIDI_NOTEOFF: //0x80
	   
	    input->doNoteOff( channel, data[0], data[1] );
	    break;
	    
	case MIDI_NOTEON: //0x90
	    
	    if ( data[1] == 0 ) // some midi keyboards or synths are returning Velocity 0 if the key is released
	    {
	    input->doNoteOff( channel, data[0], data[1] );
	    }
	    else
	    {
	    input->doNoteOn( channel, data[0], data[1] );
	    }
	    break;
	    
	case MIDI_KEY_PRESSURE: //0xA0
	   
	    debug( DEBUG_STATUS, "KEY PRESSURE ch=%d note=%d amount=%d\n", 
		   channel, cmd, data[ 0 ] );
	    break;
	    
	case MIDI_CTL_CHANGE: //0xB0
	   
	    debug( DEBUG_STATUS, "CTL_CHANGE %d %d %d\n", channel, data[ 0 ], data[ 1 ] );

	    sprintf( ctlName, "ctl%d-%d", channel, data[0] );
	    {
		Output* out;
		out = input->MoogObject::getOutput( ctlName );
		if ( out != NULL )
		    out->setData(data[ 1 ] / 127.0);
	    }

	    break;
	    
	case MIDI_PGM_CHANGE: //0xC0
	    
	    debug( DEBUG_STATUS, "PGM_CHANGE %d %d\n", channel, data[ 0 ] );
	    //printf("change program to %d\n",data[0]);
            sprintf( ctlName, "patch_%d", data[0]%10);
            Output* out;
	    out = input->MoogObject::getOutput( ctlName );
            if ( out != NULL )
		    out->setData(1);

	   // if (data[0]>NUM_PATCHES)
	   // {
	   // printf("can't change. highes patch number is\n",NUM_PATCHES);
           // return;
	   // }
	   // loadPatch(&patches[data[0]]); 
	    break;
	    
	case MIDI_CHN_PRESSURE: //0xD0
	   
	    debug( DEBUG_STATUS, "CHN_PRESSURE %d %d\n", channel, data[ 0 ] );
	    break;
	    
	case MIDI_PITCH_BEND: //0xE0
	    
	    input->doPitchBend( ( data[ 1 ] << 7 ) & 0xFF00 | data[ 0 ] );
	    break;
	    
	case MIDI_SYSTEM_PREFIX: //0xF0 SysEx? i have to study the format first. currently not supported
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

}



void 
MidiInput::doNoteOn( unsigned int c, unsigned int n, unsigned int v )
{
    /* try to avoid re-using the same note as long as possible, so that
     * if it has a Release envelope, it will get as much time as possible
     */
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

bool MidiInput::isOpen() { 
return (handle!=NULL); 
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
