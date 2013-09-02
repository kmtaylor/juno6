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
 * $Revision: 1.17.4.2 $$Date: 2000/02/24 00:43:48 $
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <moog/moog.h>
#include <util/String.h>
#include <sys/types.h>
#include <util/debug.h>
//#include <util/cpudetect.h>
#include "juno_patch.h"
#include "juno_gui.h"
#include "juno_synth.h"

String patchFileName;
juno_patch* patches;

#ifdef DOMAIN
int main(int argc,char** argv)
#else
int Juno666( int argc, char** argv )
#endif
{
    debuglvl = DEBUG_SYSERROR | DEBUG_APPERROR | DEBUG_APPMSG1;

    g_thread_init(NULL);
    gtk_init( &argc, &argv );

    if (argc > 1)
    {
	patchFileName = argv[1];
    }
    else
    {
	//FIXME: maybe this should come from settings???
	
	patchFileName = "juno6.patches";
    }

    MidiInput* midiInput = NULL;

    Settings settings;
    const char *numVoicesStr = settings.getString("juno6", "num-voices");
    int numVoices = 6;

    if (strcmp( numVoicesStr, "" ) != 0)
	numVoices = atoi(numVoicesStr);

    if ( settings.getInt( "devices", "use-midi" ) )
    {
	midiInput = new MidiInput( settings.getString( "devices", 
						       "midi-input" ),
				   numVoices);

	if (midiInput->isOpen())
	{
	    midiInput->start();
	}
	else
	{
	    delete midiInput;
	    midiInput = NULL;
	}
    }
    Scheduler::Init();
    JunoControl *junoControl = new JunoControl(numVoices);
//puts("init synth");
    initSynth( junoControl, &settings, midiInput, numVoices );
//puts("init gui");
    initGui( junoControl, &settings, midiInput, numVoices );
//puts("create patches");
    patches = juno_patchset_new();
//puts("load patches");
    load_patches( patchFileName, patches );
//puts("loadPatch");
    loadPatch( &patches[0] );
//puts("gdk_enter");
    gdk_threads_enter();
//puts("gdk_main");
    gtk_main();
    gdk_threads_leave();
    Scheduler::DeInit();
    exit(1);
    return( 0 );
}

#ifndef DOMAIN
extern "C" int Juno666main(int argc,char *argv[])
{
return Juno666(argc,argv);
}
#endif
