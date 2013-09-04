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
 * $Revision: 1.31.4.1 $$Date: 2000/02/24 00:43:48 $
 */
#include <stdlib.h>
#include "juno_synth.h"
#include "juno_voice.h"
#include "juno_lfo.h"
#include "juno_arpeggio.h"
#include "juno_chorus.h"
#include <gmoog/Scope.h>
#include "juno_patch.h"

//FIXME: this is grosser than gross
extern JunoControl *junoControl;

DSPOutput    *dsp;
int numV;
JunoVoice *voice[64];
/*
JunoVoice    *voice1;
JunoVoice    *voice2;
JunoVoice    *voice3;
JunoVoice    *voice4;
JunoVoice    *voice5;
*/
Mixer        *voiceMix;
Rand         *noise;
JunoLfo      *lfo;
JunoArpeggio *arpeggio;
JunoChorus   *chorus;
JunoChorus   *chorus2;
Attenuator   *pwmLfo;
HPF          *hpf;

extern Settings *settings;
extern String patchFileName;

Scope* scope = NULL;

bool stereo;

void
setScope( int v )
{
    if ( scope == NULL || scope->showing == 0 )
	return;

    PATCH( &voice[v]->sub, "sync", scope, "sync" );

  /*  switch( voice )
    {
    case 0:
	PATCH( &voice0->sub, "sync", scope, "sync" );
	break;

    case 1:
	PATCH( &voice1->sub, "sync", scope, "sync" );
	break;

    case 2:
	PATCH( &voice2->sub, "sync", scope, "sync" );
	break;

    case 3:
	PATCH( &voice3->sub, "sync", scope, "sync" );
	break;

    case 4:
	PATCH( &voice4->sub, "sync", scope, "sync" );
	break;

    case 5:
	PATCH( &voice5->sub, "sync", scope, "sync" );
	break;
    }*/
}

void
initSynth( JunoControl* _junoControl, 
	   Settings* settings, 
	   MidiInput* midiInput,
	   int numVoices )
{
    stereo = strcmp( "yes", 
		     settings->getString("devices", "stereo_output") ) == 0;

    int numFrags = -1;
    int fragSize = -1;
    const char *p;

    p = settings->getString("devices", "NumFrags");
    if (strlen(p))
	numFrags = atoi(p);

    p = settings->getString("devices", "FragSize");
    if (strlen(p))
	fragSize = atoi(p);
    dsp = new DSPOutput( settings->getString( "devices", "dsp-output" ),
			 SAMPLE_RATE_44k,
			 ( stereo ) ? 2 : 1,
			 numFrags,
			 fragSize);
    //FIXME: we have to hold on to the junoControl pointer... sloppy
    junoControl = _junoControl;

    lfo = new JunoLfo(junoControl,numVoices);
    
    arpeggio = new JunoArpeggio(junoControl,numVoices);
    
    pwmLfo = new Attenuator;
   
    noise = new Rand;
    
    voiceMix = new Mixer(numVoices);
    
    hpf = new HPF;
   
    PATCH(lfo, "sig", pwmLfo, "sig");
    
    pwmLfo->set("amp", .5);
    
    pwmLfo->set("zro", .5);

    //FIXME: an array of voices would be nice, huh?
    debug(DEBUG_APPMSG1, "Creating %d voices...", numVoices);

    /* fall thru each case */
    if (numVoices>64)
    {
    debug(DEBUG_APPMSG1, "more than 64 voices are not allowed, truncated!");
    numVoices=64;
    }
    numV = numVoices;
    for (int i=0;i<numVoices;i++)
    {
    //debug(DEBUG_APPMSG1, "CREATING VOICE %d",i);
    voice[i] = new JunoVoice(junoControl,i);
    char *tmp = new char[6];
    sprintf(tmp,"sig%d",i);
    //debug(DEBUG_APPMSG1, "patching voice",i);
    PATCH(voice[i],"sig",voiceMix,tmp);
    }
    
    /*switch(numVoices)
    {
    case 6:
	voice5 = new JunoVoice(junoControl, 5);
	PATCH(voice5, "sig", voiceMix, "sig5");
	
    case 5:
	voice4 = new JunoVoice(junoControl, 4);
	PATCH(voice4, "sig", voiceMix, "sig4");
	
    case 4:
	voice3 = new JunoVoice(junoControl, 3);
	PATCH(voice3, "sig", voiceMix, "sig3");
	
    case 3:
	voice2 = new JunoVoice(junoControl, 2);
	PATCH(voice2, "sig", voiceMix, "sig2");
	
    case 2:
	voice1 = new JunoVoice(junoControl, 1);
	PATCH(voice1, "sig", voiceMix, "sig1");
	
    case 1:
	voice0 = new JunoVoice(junoControl, 0);
	PATCH(voice0, "sig", voiceMix, "sig0");
    }*/

  
    PATCH(voiceMix, "sig", hpf, "sig");
    
    PATCH(junoControl, "hpf_frq", hpf, "frq");
    //FIXME: tune the Q of the hpf
    
    hpf->set("Q", 0.0);
    
    chorus = new JunoChorus( hpf, "sig", 0 );
    
    if ( stereo )
    {
        
	chorus2 = new JunoChorus( hpf, "sig", 1 );
	PATCH( junoControl, "chorus_off_switch", chorus2, "off" );
	PATCH( junoControl, "chorus_I_switch", chorus2, "I" );
	PATCH( junoControl, "chorus_II_switch", chorus2, "II" );
	PATCH( chorus2, "sig", dsp, "sig1" );
	PATCH( junoControl, "volume", dsp, "amp1");
    }

    PATCH( junoControl, "chorus_off_switch", chorus, "off" );
    PATCH( junoControl, "chorus_I_switch", chorus, "I" );
    PATCH( junoControl, "chorus_II_switch", chorus, "II" );
    PATCH( chorus, "sig", dsp, "sig0" );

    PATCH( junoControl, "volume", dsp, "amp0");

    // all cool dave, scope does nothing until you pop it up.
    scope = new Scope();
    PATCH( chorus, "sig", scope, "sig" );

    if ( stereo )
    {
	PATCH( chorus2, "sig", scope, "sig2" );
    }

    /* the Balance in the voices needs signal to calibrate, so start
     * the oscillators with an arbitrary frequency - they run forever
     */
    for (int i=0;i<numV;i++)
    {
    junoControl->getOutput((String)"voice"+i+(String)"_pitch")->setData(CPS(666));
    }
    /*junoControl->getOutput("voice1_pitch")->setData(CPS(666));
    junoControl->getOutput("voice2_pitch")->setData(CPS(666));
    junoControl->getOutput("voice3_pitch")->setData(CPS(666));
    junoControl->getOutput("voice4_pitch")->setData(CPS(666));
    junoControl->getOutput("voice5_pitch")->setData(CPS(666));*/

    PATCH( junoControl, "volume", dsp, "amp0");
    
    Scheduler::start( dsp );
}

void turnOnArpeggio(bool on)
{
    MoogObject *target = (MoogObject*)((on) ? (MoogObject*)arpeggio : (MoogObject*)junoControl);
    for (int i=0;i<numV;i++)
	{

	if (voice[i])
	   voice[i]->attachVoice(target);
	}
    /*if (voice0)
	voice0->attachVoice(target);
    if (voice1)
	voice1->attachVoice(target);
    if (voice2)
	voice2->attachVoice(target);
    if (voice3)
	voice3->attachVoice(target);
    if (voice4)
	voice4->attachVoice(target);
    if (voice5)
	voice5->attachVoice(target);*/
}






