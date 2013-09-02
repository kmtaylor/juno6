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
 * $Revision: 1.7.4.1 $$Date: 2000/02/24 00:43:59 $
 */
#include <math.h>
#include <stdio.h>
#include <gtk/gtkmain.h>
#include <gtk/gtksignal.h>
#include <gdk/gdk.h>

#include "juno_button.h"

static GdkColor colors[] =
{
    {0,0xfdfd,0xf7f7,0xcbcb},
    {0,0xdcdc,0xdcdc,0xb2b2},
    {0,0xb2b2,0xb3b3,0x9090},

    {0,0xfdfd,0xf7f7,0xcbcb},
    {0,0xe4e4,0xe5e5,0x5858},
    {0,0xb4b4,0xb3b3,0x4545},

    {0,0xfdfd,0xf7f7,0xcbcb},
    {0,0xf0f0,0xadad,0x3838},
    {0,0xb4b4,0x7e7e,0x2929}
};

static GtkWidgetClass *parent_class = NULL;

static void
gtk_juno_button_init( GtkJunoButton *juno_button );

static void
gtk_juno_button_class_init( GtkJunoButtonClass *class );

static void
gtk_juno_button_destroy( GtkObject *object );

static void
gtk_juno_button_realize( GtkWidget *widget );

static gint
gtk_juno_button_expose( GtkWidget      *widget,
			GdkEventExpose *event );

static gint
gtk_juno_button_button_press( GtkWidget      *widget,
			      GdkEventButton *event );

static void 
gtk_juno_button_size_request( GtkWidget      *widget,
			      GtkRequisition *requisition );

static void 
gtk_juno_button_size_allocate( GtkWidget      *widget,
			       GtkAllocation *allocation );

void
gtk_juno_button_set_update_policy( GtkJunoButton *juno_button,
				   GtkUpdateType  policy );

static gint
gtk_juno_button_button_release( GtkWidget      *widget,
				GdkEventButton *event );

static gint
gtk_juno_button_motion_notify( GtkWidget      *widget,
			       GdkEventMotion *event );

enum {
  PRESSED,
  RELEASED,
  CLICKED,
  ENTER,
  LEAVE,
  LAST_SIGNAL
};

static guint button_signals[LAST_SIGNAL] = { 0 };

GtkWidget*
gtk_juno_button_new( GtkJunoButtonType type )
{
    GtkJunoButton *juno_button = gtk_type_new( gtk_juno_button_get_type() );
    juno_button->type = type;
    return GTK_WIDGET( juno_button );
}

static void
gtk_juno_button_destroy( GtkObject *object )
{
    //GtkJunoButton *juno_button;
    
    g_return_if_fail( object != NULL );
    g_return_if_fail( GTK_IS_JUNO_BUTTON( object ) );
    
    //juno_button = GTK_JUNO_BUTTON( object );
    
    if ( GTK_OBJECT_CLASS (parent_class)->destroy )
	(* GTK_OBJECT_CLASS (parent_class)->destroy) (object);
}

guint
gtk_juno_button_get_type()
{
    static guint juno_button_type = 0;
    
    if (!juno_button_type)
    {
	GtkTypeInfo juno_button_info =
	{
	    "GtkJuno_Button",
	    sizeof(GtkJunoButton),
	    sizeof(GtkJunoButtonClass),
	    (GtkClassInitFunc)gtk_juno_button_class_init,
	    (GtkObjectInitFunc)gtk_juno_button_init,
	    (GtkArgSetFunc)NULL,
	    (GtkArgGetFunc)NULL,
	};
	
	juno_button_type = gtk_type_unique( gtk_widget_get_type(), 
					    &juno_button_info );
    }
    
    return(juno_button_type);
}

static void
gtk_juno_button_class_init( GtkJunoButtonClass *class )
{
    GtkObjectClass *object_class;
    GtkWidgetClass *widget_class;
    
    object_class = (GtkObjectClass*) class;
    widget_class = (GtkWidgetClass*) class;
    
    parent_class = gtk_type_class (gtk_widget_get_type ());
    
    object_class->destroy = gtk_juno_button_destroy;
    
    widget_class->realize = gtk_juno_button_realize;
    widget_class->expose_event = gtk_juno_button_expose;
    widget_class->size_request = gtk_juno_button_size_request;
    widget_class->size_allocate = gtk_juno_button_size_allocate;
    widget_class->button_press_event = gtk_juno_button_button_press;
    widget_class->button_release_event = gtk_juno_button_button_release;
    widget_class->motion_notify_event = gtk_juno_button_motion_notify;

    button_signals[PRESSED] =
    gtk_signal_new ("pressed",
                    GTK_RUN_FIRST,
                    object_class->type,
                    GTK_SIGNAL_OFFSET (GtkJunoButtonClass, pressed),
                    gtk_marshal_NONE__NONE,
                    GTK_TYPE_NONE, 0);
    button_signals[RELEASED] =
    gtk_signal_new ("released",
                    GTK_RUN_FIRST,
                    object_class->type,
                    GTK_SIGNAL_OFFSET (GtkJunoButtonClass, released),
                    gtk_marshal_NONE__NONE,
                    GTK_TYPE_NONE, 0);
    button_signals[CLICKED] =
    gtk_signal_new ("clicked",
                    GTK_RUN_FIRST | GTK_RUN_ACTION,
                    object_class->type,
                    GTK_SIGNAL_OFFSET (GtkJunoButtonClass, clicked),
                    gtk_marshal_NONE__NONE,
                    GTK_TYPE_NONE, 0);
    button_signals[ENTER] =
    gtk_signal_new ("enter",
                    GTK_RUN_FIRST,
                    object_class->type,
                    GTK_SIGNAL_OFFSET (GtkJunoButtonClass, enter),
                    gtk_marshal_NONE__NONE,
                    GTK_TYPE_NONE, 0);
    button_signals[LEAVE] =
    gtk_signal_new ("leave",
                    GTK_RUN_FIRST,
                    object_class->type,
                    GTK_SIGNAL_OFFSET (GtkJunoButtonClass, leave),
                    gtk_marshal_NONE__NONE,
                    GTK_TYPE_NONE, 0);
    
  gtk_object_class_add_signals (object_class, button_signals, LAST_SIGNAL);

}

static void
gtk_juno_button_init( GtkJunoButton *juno_button )
{
    juno_button->button = 0;
//    juno_button->policy = GTK_UPDATE_CONTINUOUS;
}

void
gtk_juno_button_set_update_policy(GtkJunoButton *juno_button,
				  GtkUpdateType  policy)
{
    g_return_if_fail (juno_button != NULL);
    g_return_if_fail (GTK_IS_JUNO_BUTTON (juno_button));
    
//    juno_button->policy = policy;
}

static void
gtk_juno_button_realize (GtkWidget *widget)
{
    //GtkJunoButton *juno_button;
    GdkWindowAttr attributes;
    gint attributes_mask;
    
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_JUNO_BUTTON (widget));
    
    GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);
    //juno_button = GTK_JUNO_BUTTON (widget);
    
    attributes.x = widget->allocation.x;
    attributes.y = widget->allocation.y;
    attributes.width = widget->allocation.width;
    attributes.height = widget->allocation.height;
    attributes.wclass = GDK_INPUT_OUTPUT;
    attributes.window_type = GDK_WINDOW_CHILD;
    attributes.event_mask = gtk_widget_get_events (widget) | 
    GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK | 
    GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK |
    GDK_POINTER_MOTION_HINT_MASK;
    attributes.visual = gtk_widget_get_visual (widget);
    attributes.colormap = gtk_widget_get_colormap (widget);
    
    attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
    widget->window = gdk_window_new (widget->parent->window, &attributes, attributes_mask);
    
    widget->style = gtk_style_attach (widget->style, widget->window);
    
    gdk_window_set_user_data (widget->window, widget);
    
    gtk_style_set_background (widget->style, widget->window, GTK_STATE_ACTIVE);
}

static void 
gtk_juno_button_size_request( GtkWidget      *widget,
			      GtkRequisition *requisition )
{
    requisition->width  = JUNO_BUTTON_DEFAULT_WIDTH;
    requisition->height = JUNO_BUTTON_DEFAULT_HEIGHT;
}

static void
gtk_juno_button_size_allocate( GtkWidget     *widget,
			       GtkAllocation *allocation )
{
    //GtkJunoButton *juno_button;
    
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_JUNO_BUTTON (widget));
    g_return_if_fail (allocation != NULL);
    
    widget->allocation = *allocation;
    if (GTK_WIDGET_REALIZED (widget))
    {
	//juno_button = GTK_JUNO_BUTTON (widget);
	
	gdk_window_move_resize (widget->window,
				allocation->x, allocation->y,
				allocation->width, allocation->height);
    }
}
 
static gint
gtk_juno_button_expose( GtkWidget      *widget,
			GdkEventExpose *event )
{
    GtkJunoButton * juno_button = GTK_JUNO_BUTTON (widget);
    GdkColormap * map;
    GdkGC * gc;

    GdkColor shadow  = colors[ 3 * juno_button->type + JUNO_BUTTON_SHADOW ];
    GdkColor normal  = colors[ 3 * juno_button->type + JUNO_BUTTON_NORMAL ];
    GdkColor hilight = colors[ 3 * juno_button->type + JUNO_BUTTON_HILIGHT ];

    if (widget == NULL || widget->window == NULL)
	return FALSE;

    map = gdk_colormap_new( gtk_widget_get_visual( widget ), TRUE );
    gc = gdk_gc_new( widget->window );

    gdk_colormap_alloc_color( map, &shadow, TRUE, TRUE );
    gdk_colormap_alloc_color( map, &normal, TRUE, TRUE );
    gdk_colormap_alloc_color( map, &hilight, TRUE, TRUE );

    // background
    gdk_draw_rectangle( widget->window, 
			widget->style->base_gc[ GTK_STATE_NORMAL ], TRUE, 0, 0, 
			widget->allocation.width, widget->allocation.height );


    // outline
    gdk_gc_set_foreground( gc, &normal );
    gdk_draw_rectangle( widget->window, gc, 
			TRUE, 2, 2, 
			JUNO_BUTTON_DEFAULT_WIDTH - 4, 
			JUNO_BUTTON_DEFAULT_HEIGHT - 4 );
    
    // hilight
    if ( juno_button->button == 1 )
	gdk_gc_set_foreground( gc, &shadow );
    else
	gdk_gc_set_foreground( gc, &hilight );

    gdk_draw_line( widget->window, gc,
		   1, 1, JUNO_BUTTON_DEFAULT_WIDTH - 2, 1 );
    
    gdk_draw_line( widget->window, gc,
		   1, 1, 1, JUNO_BUTTON_DEFAULT_HEIGHT - 2 );

    if ( juno_button->button == 1 )
	gdk_gc_set_foreground( gc, &hilight );
    else
	gdk_gc_set_foreground( gc, &shadow );
 
    gdk_draw_line( widget->window, gc,
		   JUNO_BUTTON_DEFAULT_WIDTH - 2, 2, 
		   JUNO_BUTTON_DEFAULT_WIDTH -2, 
		   JUNO_BUTTON_DEFAULT_HEIGHT - 2 );
    
    gdk_draw_line( widget->window, gc,
		   2, 
		   JUNO_BUTTON_DEFAULT_HEIGHT - 2, 
		   JUNO_BUTTON_DEFAULT_WIDTH -2, 
		   JUNO_BUTTON_DEFAULT_HEIGHT - 2 );

    //outline
    gdk_draw_rectangle( widget->window, 
			widget->style->black_gc, FALSE, 0, 0, 
			JUNO_BUTTON_DEFAULT_WIDTH - 1, 
			JUNO_BUTTON_DEFAULT_HEIGHT - 1 );

    gdk_draw_rectangle( widget->window, 
			widget->style->black_gc, FALSE, 0, 0, 
			JUNO_BUTTON_DEFAULT_WIDTH - 1, 
			JUNO_BUTTON_DEFAULT_HEIGHT - 1 );

    gdk_gc_destroy( gc );

    gdk_flush();
    return FALSE;  
}

static gint
gtk_juno_button_button_press( GtkWidget      *widget,
			      GdkEventButton *event )
{
    GtkJunoButton *juno_button; 
    
    g_return_val_if_fail( widget != NULL, FALSE );
    g_return_val_if_fail( GTK_IS_JUNO_BUTTON (widget), FALSE );
    g_return_val_if_fail( event != NULL, FALSE );
    
    juno_button = GTK_JUNO_BUTTON( widget );

    juno_button->button = event->button;

    if (event->type == GDK_BUTTON_PRESS)
    {
	if (!GTK_WIDGET_HAS_FOCUS (widget))
	    gtk_widget_grab_focus (widget);

	if( event->button == 1 )
	{
	    gtk_grab_add( widget );
	    gtk_juno_button_do_press( juno_button );
	}
    }

    return TRUE;
}

static gint
gtk_juno_button_button_release (GtkWidget      *widget,
			 GdkEventButton *event)
{
    GtkJunoButton *juno_button;
    
    g_return_val_if_fail (widget != NULL, FALSE);
    g_return_val_if_fail (GTK_IS_JUNO_BUTTON (widget), FALSE);
    g_return_val_if_fail (event != NULL, FALSE);
    
    juno_button = GTK_JUNO_BUTTON (widget);
    
    if( event->button == 1 )
    {
	gtk_grab_remove( widget ); // what does this do?
	gtk_juno_button_do_release( juno_button );
    }
    
    return TRUE;
}

static gint
gtk_juno_button_motion_notify (GtkWidget      *widget,
			GdkEventMotion *event)
{
    GtkJunoButton *juno_button;
    GdkModifierType mods;
    gint x, y; // mask;
    
    g_return_val_if_fail (widget != NULL, FALSE);
    g_return_val_if_fail (GTK_IS_JUNO_BUTTON (widget), FALSE);
    g_return_val_if_fail (event != NULL, FALSE);
    
    juno_button = GTK_JUNO_BUTTON( widget );
   
    if (juno_button->button != 0)
    {
	x = event->x;
	y = event->y;
	
	if (event->is_hint || (event->window != widget->window))
	    gdk_window_get_pointer (widget->window, &x, &y, &mods);
	
	switch (juno_button->button)
	{
	case 1:
	    //mask = GDK_BUTTON1_MASK;
	    break;
	case 2:
	    //mask = GDK_BUTTON2_MASK;
	    break;
	case 3:
	    //mask = GDK_BUTTON3_MASK;
	    break;
	default:
	    //mask = 0;
	    break;
	}
	
//	if (mods & mask)
//	    gtk_juno_button_update_mouse (juno_button, x,y);

	gtk_juno_button_expose( widget, NULL );
    }
    
    return FALSE;
}

void gtk_juno_button_do_press( GtkJunoButton* b )
{
    b->button = 1;	
    gtk_juno_button_expose( GTK_WIDGET(b), NULL );
    gtk_signal_emit( GTK_OBJECT( b ), button_signals[PRESSED] );
}

void gtk_juno_button_do_release( GtkJunoButton* b )
{
    b->button = 0;	
    gtk_juno_button_expose( GTK_WIDGET(b), NULL );
    gtk_signal_emit( GTK_OBJECT( b ), button_signals[RELEASED] );
}
