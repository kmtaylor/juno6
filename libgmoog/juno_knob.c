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
 * $Revision: 1.9.4.1 $$Date: 2000/02/24 00:43:59 $
 */
#include <math.h>
#include <stdio.h>
#include <gtk/gtkmain.h>
#include <gtk/gtksignal.h>
#include <gdk/gdk.h>

#include "juno_knob.h"

static GtkWidgetClass *parent_class = NULL;

static void gtk_juno_knob_init( GtkJunoKnob             *juno_knob );

static void gtk_juno_knob_class_init( GtkJunoKnobClass  *class );

static void gtk_juno_knob_destroy( GtkObject            *object );

static void gtk_juno_knob_realize( GtkWidget            *widget );

static gint gtk_juno_knob_expose( GtkWidget             *widget,
				  GdkEventExpose        *event );

static void gtk_juno_knob_size_request( GtkWidget       *widget,
					GtkRequisition  *requisition );

static void gtk_juno_knob_size_allocate( GtkWidget      *widget,
					 GtkAllocation  *allocation );

static gint gtk_juno_knob_motion_notify( GtkWidget      *widget, 
					 GdkEventMotion *event );

static gint
gtk_juno_knob_button_press (GtkWidget *widget, GdkEventButton *event);

static gint
gtk_juno_knob_button_release (GtkWidget *widget, GdkEventButton *event);

enum 
{
    VALUE_CHANGED,
    LAST_SIGNAL
};

static guint knob_signals[LAST_SIGNAL] = {0};

guint
gtk_juno_knob_get_type()
{
    static guint juno_knob_type = 0;
    
    if (!juno_knob_type)
    {
	GtkTypeInfo juno_knob_info =
	{
	    "GtkJuno_Knob",
	    sizeof(GtkJunoKnob),
	    sizeof(GtkJunoKnobClass),
	    (GtkClassInitFunc)gtk_juno_knob_class_init,
	    (GtkObjectInitFunc)gtk_juno_knob_init,
	    (GtkArgSetFunc)NULL,
	    (GtkArgGetFunc)NULL,
	};
	
	juno_knob_type = gtk_type_unique( gtk_widget_get_type(), 
					  &juno_knob_info );
    }
    
    return(juno_knob_type);
}

static void
gtk_juno_knob_class_init (GtkJunoKnobClass *class)
{
    GtkObjectClass *object_class;
    GtkWidgetClass *widget_class;
    
    object_class = (GtkObjectClass*) class;
    widget_class = (GtkWidgetClass*) class;
    
    parent_class = gtk_type_class (gtk_widget_get_type ());
    
    object_class->destroy = gtk_juno_knob_destroy;
    
    widget_class->realize = gtk_juno_knob_realize;
    widget_class->expose_event = gtk_juno_knob_expose;
    widget_class->size_request = gtk_juno_knob_size_request;
    widget_class->size_allocate = gtk_juno_knob_size_allocate;
    widget_class->button_press_event = gtk_juno_knob_button_press;
    widget_class->button_release_event = gtk_juno_knob_button_release;
    widget_class->motion_notify_event = gtk_juno_knob_motion_notify;

    knob_signals[VALUE_CHANGED] = gtk_signal_new(
	"value_changed",
	GTK_RUN_FIRST,
	object_class->type, 
	GTK_SIGNAL_OFFSET(GtkJunoKnobClass, value_changed),
	gtk_marshal_NONE__NONE,
	GTK_TYPE_NONE,
	0);

    gtk_object_class_add_signals(object_class, knob_signals, LAST_SIGNAL);
}

static void
gtk_juno_knob_init (GtkJunoKnob *juno_knob)
{
}

GtkWidget*
gtk_juno_knob_new()
{
    GtkJunoKnob *juno_knob;
    
    juno_knob = gtk_type_new (gtk_juno_knob_get_type ());
    
    return GTK_WIDGET (juno_knob);
}

static void
gtk_juno_knob_destroy (GtkObject *object)
{
    if (GTK_OBJECT_CLASS (parent_class)->destroy)
	(* GTK_OBJECT_CLASS (parent_class)->destroy) (object);
}

static void
gtk_juno_knob_realize (GtkWidget *widget)
{
    //GtkJunoKnob *juno_knob;
    GdkWindowAttr attributes;
    gint attributes_mask;
    
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_JUNO_KNOB (widget));
    
    GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);
    //juno_knob = GTK_JUNO_KNOB (widget);
    
    attributes.x = widget->allocation.x;
    attributes.y = widget->allocation.y;
    attributes.width = widget->allocation.width;
    attributes.height = widget->allocation.height;
    attributes.wclass = GDK_INPUT_OUTPUT;
    attributes.window_type = GDK_WINDOW_CHILD;

    attributes.event_mask = gtk_widget_get_events (widget) | 
	GDK_EXPOSURE_MASK | GDK_POINTER_MOTION_MASK | 
        GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK |
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
gtk_juno_knob_size_request( GtkWidget      *widget,
			      GtkRequisition *requisition )
{
    requisition->width  = JUNO_KNOB_DEFAULT_WIDTH;
    requisition->height = JUNO_KNOB_DEFAULT_HEIGHT;
}

static void
gtk_juno_knob_size_allocate( GtkWidget     *widget,
			       GtkAllocation *allocation )
{
    //GtkJunoKnob *juno_knob;
    
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_JUNO_KNOB (widget));
    g_return_if_fail (allocation != NULL);
    
    widget->allocation = *allocation;
    if (GTK_WIDGET_REALIZED (widget))
    {
	//juno_knob = GTK_JUNO_KNOB (widget);
	
	gdk_window_move_resize (widget->window,
				allocation->x, allocation->y,
				allocation->width, allocation->height);
    }
}
 
static gint
gtk_juno_knob_expose( GtkWidget      *widget,
			GdkEventExpose *event )
{
    GtkJunoKnob* juno_knob = GTK_JUNO_KNOB( widget );
    GdkColormap* map;
    GdkGC* gc;

    GdkColor darkGray = {0,0x2020,0x2020,0x2020};
    GdkColor gray     = {0,0x8080,0x8080,0x8080};
    GdkColor background = {0,0x3333,0x3333,0x3333};

    GdkGC* black       = widget->style->black_gc;
    //GdkGC* white       = widget->style->white_gc;

    if (widget == NULL || widget->window == NULL)
	return FALSE;

    map = gdk_colormap_new( gtk_widget_get_visual( widget ), TRUE );
    gc = gdk_gc_new( widget->window );
    
    gdk_colormap_alloc_color( map, &darkGray, TRUE, TRUE );
    gdk_colormap_alloc_color( map, &gray, TRUE, TRUE );
    gdk_colormap_alloc_color( map, &background, TRUE, TRUE );

    // background
    gdk_gc_set_foreground( gc, &background );
    gdk_draw_rectangle( widget->window, 
			gc, TRUE, 0, 0, 
			widget->allocation.width, widget->allocation.height );

    gdk_draw_arc( widget->window, black, TRUE,
		  5, 5, 8, 8, 0, 360 * 64 );

    gdk_gc_set_foreground( gc, &darkGray );
    gdk_draw_arc( widget->window, gc, FALSE,
		  5, 5, 8, 8, 0, 360 * 64 );

    gdk_gc_set_foreground( gc, &gray );
    gdk_draw_line(widget->window, gc, 
		  (int)(9.5 - cos(juno_knob->angle) * 3), 
		  (int)(9.5 + sin(juno_knob->angle) * 3),
		  (int)(9.5 + cos(juno_knob->angle) * 3), 
		  (int)(9.5 - sin(juno_knob->angle) * 3));

    gdk_gc_destroy( gc );
    gdk_flush();

    return FALSE;  
}

static gint
gtk_juno_knob_button_press (GtkWidget      *widget,
			 GdkEventButton *event)
{
    gtk_grab_add( widget );
    return TRUE;
}

static gint
gtk_juno_knob_button_release (GtkWidget      *widget,
			 GdkEventButton *event)
{
    gtk_grab_remove( widget );
    return TRUE;
}

static gint
gtk_juno_knob_motion_notify (GtkWidget      *widget,
			     GdkEventMotion *event)
{
    GtkJunoKnob *juno_knob;
    GdkModifierType state;
    gint x, y;
    
    g_return_val_if_fail (widget != NULL, FALSE);
    g_return_val_if_fail (GTK_IS_JUNO_KNOB (widget), FALSE);
    g_return_val_if_fail (event != NULL, FALSE);
    
    juno_knob = GTK_JUNO_KNOB( widget );
    
    if ((event->is_hint) || (event->window != widget->window))
    {
	gdk_window_get_pointer (widget->window, &x, &y, &state);
    }
    else
    {
	x = event->x;
	y = event->y;
	state = event->state;
    }
    
    if (state & GDK_BUTTON1_MASK)
    {
	gint xc, yc;
	gdouble angle;
	gdouble value;

	xc = GTK_WIDGET(juno_knob)->allocation.width / 2;
	yc = GTK_WIDGET(juno_knob)->allocation.height / 2;
    
	angle = atan2(yc-y, x-xc);

	if (angle < 0)
	    angle += 2 * M_PI;

	if (angle <= 7 * M_PI / 6 || angle >= 11 * M_PI / 6)
	{
	    value = 7 * M_PI / 6 - angle;
	    
	    if (value < 0)
		value += 2 * M_PI;

	    value = value / (4 * M_PI / 3);

	    juno_knob->value = value;
	    juno_knob->angle = angle;

//	    printf( "%f %f\n", juno_knob->angle, juno_knob->value );
	    gtk_signal_emit(GTK_OBJECT(juno_knob),knob_signals[VALUE_CHANGED]);
	}

	gtk_juno_knob_expose( widget, NULL );
    }
    
    return FALSE;
}

double gtk_juno_knob_get_value(GtkJunoKnob *jk)
{
    return jk->value;
}

void gtk_juno_knob_set_value(GtkJunoKnob *jk, double value)
{
    if (value < 0 || value >= 1.0)
	return;

    jk->value = value;
    jk->angle = 7 * M_PI / 6 - value * 4 * M_PI/3;

    gtk_juno_knob_expose(GTK_WIDGET(jk), NULL);
    gtk_signal_emit(GTK_OBJECT(jk), knob_signals[VALUE_CHANGED]);
}


