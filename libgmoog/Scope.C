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
 * $Revision: 1.6.4.2 $$Date: 2000/02/25 20:40:43 $
 */
#include "Scope.h"
#include <moog/Scheduler.h>
#include <gtk/gtk.h>

gint delete_event_handler( GtkWidget *widget,
			   GdkEvent  *event,
			   gpointer   data )
{
    ((Scope*)data)->popdown();
    return(TRUE);
}


Scope::Scope()
{
    addInput( "sync", NULL ,0,0); 
    addInput( "sig", NULL,0,0 ); 

    showing = 0;
    mainWindow = NULL;

    widget = GTK_SCOPE( gtk_scope_new() );
    gtk_widget_show( GTK_WIDGET( widget ) );
}

void Scope::sampleGo()
{
    /* avoid deadlock: implicitly, sampleGo and controlGo have Scheduler lock
     * if we block on getting GTK lock, we could deadlock with a gtk signal 
     * handler, which has GTK lock and may need Scheduler lock, so we just try
     * to update if possible:
     */

    int draw = gtk_scope_plot( widget, 
			       *inputs[0].data > 0.0, *inputs[1].data );

    if (draw)
    {
	if(g_mutex_trylock(gdk_threads_mutex))
	{
	    gtk_scope_draw( widget );
	    g_mutex_unlock(gdk_threads_mutex);
	}
	else
	{
	    widget->pos = 0;
	    debug(DEBUG_STATUS, "gdk lock busy, skipping scope update");
	}
    }
}

void Scope::toggle()
{
    if ( showing )
	popdown();
    else
	popup();
}

void Scope::popup()
{
    if ( !mainWindow )
    {
	mainWindow = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	gtk_widget_show( mainWindow );
	gtk_container_add( GTK_CONTAINER( mainWindow ), GTK_WIDGET( widget ) );
	gtk_signal_connect( GTK_OBJECT( mainWindow ), 
			    "delete_event",
			    GTK_SIGNAL_FUNC( delete_event_handler ),
			    this );
			    
    }
    
    gtk_widget_show( mainWindow );
    showing=1;
    Scheduler::scheduleSampleRate( this, true );
}

void Scope::popdown()
{
    gtk_widget_hide( mainWindow );
    showing=0;
    Scheduler::scheduleSampleRate( this, false );
}
