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
 * $Revision: 1.5.4.1 $$Date: 2000/02/24 00:43:59 $
 */
#include <math.h>
#include <stdio.h>
#include <gtk/gtkmain.h>
#include <gtk/gtksignal.h>
#include <gdk/gdk.h>

#include "juno_lfotrig.h"

static GtkWidgetClass *parent_class = NULL;

static void gtk_juno_lfotrig_init( GtkJunoLfotrig             *juno_lfotrig );

static void gtk_juno_lfotrig_class_init( GtkJunoLfotrigClass  *class );

static void gtk_juno_lfotrig_destroy( GtkObject            *object );

static void gtk_juno_lfotrig_realize( GtkWidget            *widget );

static gint gtk_juno_lfotrig_expose( GtkWidget             *widget,
				  GdkEventExpose        *event );

static void gtk_juno_lfotrig_size_request( GtkWidget       *widget,
					GtkRequisition  *requisition );

static void gtk_juno_lfotrig_size_allocate( GtkWidget      *widget,
					 GtkAllocation  *allocation );

static gint gtk_juno_lfotrig_button_press(GtkWidget *widget, 
					 GdkEventButton *event);

static gint gtk_juno_lfotrig_button_release(GtkWidget *widget, 
					 GdkEventButton *event);

enum 
{
    VALUE_CHANGED,
    LAST_SIGNAL
};

static guint lfotrig_signals[LAST_SIGNAL] = {0};

guint
gtk_juno_lfotrig_get_type()
{
    static guint juno_lfotrig_type = 0;

    if (!juno_lfotrig_type)
    {
	GtkTypeInfo juno_lfotrig_info =
	{
	    "GtkJuno_Lfotrig",
	    sizeof(GtkJunoLfotrig),
	    sizeof(GtkJunoLfotrigClass),
	    (GtkClassInitFunc)gtk_juno_lfotrig_class_init,
	    (GtkObjectInitFunc)gtk_juno_lfotrig_init,
	    (GtkArgSetFunc)NULL,
	    (GtkArgGetFunc)NULL,
	};

	juno_lfotrig_type = gtk_type_unique( gtk_widget_get_type(), 
					  &juno_lfotrig_info );
    }

    return(juno_lfotrig_type);
}

static void
gtk_juno_lfotrig_class_init (GtkJunoLfotrigClass *class)
{
    GtkObjectClass *object_class;
    GtkWidgetClass *widget_class;

    object_class = (GtkObjectClass*) class;
    widget_class = (GtkWidgetClass*) class;

    parent_class = gtk_type_class (gtk_widget_get_type ());

    object_class->destroy = gtk_juno_lfotrig_destroy;

    widget_class->realize = gtk_juno_lfotrig_realize;
    widget_class->expose_event = gtk_juno_lfotrig_expose;
    widget_class->size_request = gtk_juno_lfotrig_size_request;
    widget_class->size_allocate = gtk_juno_lfotrig_size_allocate;
    widget_class->button_press_event = gtk_juno_lfotrig_button_press;
    widget_class->button_release_event = gtk_juno_lfotrig_button_release;

    lfotrig_signals[VALUE_CHANGED] = gtk_signal_new(
	"value_changed",
	GTK_RUN_FIRST,
	object_class->type, 
	GTK_SIGNAL_OFFSET(GtkJunoLfotrigClass, value_changed),
	gtk_marshal_NONE__NONE,
	GTK_TYPE_NONE,
	0);

    gtk_object_class_add_signals(object_class, lfotrig_signals, LAST_SIGNAL);
}

static void
gtk_juno_lfotrig_init (GtkJunoLfotrig *ps)
{
    ps->value = 0;
    ps->button = 0;
}

GtkWidget*
gtk_juno_lfotrig_new()
{
    GtkJunoLfotrig *juno_lfotrig;
    
    juno_lfotrig = gtk_type_new (gtk_juno_lfotrig_get_type ());
    
    return GTK_WIDGET (juno_lfotrig);
}

static void
gtk_juno_lfotrig_destroy (GtkObject *object)
{
    if (GTK_OBJECT_CLASS (parent_class)->destroy)
	(* GTK_OBJECT_CLASS (parent_class)->destroy) (object);
}

static void
gtk_juno_lfotrig_realize (GtkWidget *widget)
{
    //GtkJunoLfotrig *juno_lfotrig;
    GdkWindowAttr attributes;
    gint attributes_mask;
    
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_JUNO_LFOTRIG (widget));
    
    GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);
    //juno_lfotrig = GTK_JUNO_LFOTRIG (widget);
    
    attributes.x = widget->allocation.x;
    attributes.y = widget->allocation.y;
    attributes.width = widget->allocation.width;
    attributes.height = widget->allocation.height;
    attributes.wclass = GDK_INPUT_OUTPUT;
    attributes.window_type = GDK_WINDOW_CHILD;

    attributes.event_mask = gtk_widget_get_events (widget) | 
        GDK_EXPOSURE_MASK | 
        GDK_BUTTON_PRESS_MASK | 
        GDK_BUTTON_RELEASE_MASK;

    attributes.visual = gtk_widget_get_visual (widget);
    attributes.colormap = gtk_widget_get_colormap (widget);
    
    attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
    widget->window = gdk_window_new (widget->parent->window, &attributes, attributes_mask);
    
    widget->style = gtk_style_attach (widget->style, widget->window);
    
    gdk_window_set_user_data (widget->window, widget);
    
    gtk_style_set_background (widget->style, widget->window, GTK_STATE_ACTIVE);
}

static void 
gtk_juno_lfotrig_size_request( GtkWidget      *widget,
			      GtkRequisition *requisition )
{
    requisition->width  = JUNO_LFOTRIG_DEFAULT_WIDTH;
    requisition->height = JUNO_LFOTRIG_DEFAULT_HEIGHT;
}

static void
gtk_juno_lfotrig_size_allocate( GtkWidget     *widget,
			       GtkAllocation *allocation )
{
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_JUNO_LFOTRIG (widget));
    g_return_if_fail (allocation != NULL);
    
    widget->allocation = *allocation;
    if (GTK_WIDGET_REALIZED (widget))
    {
	gdk_window_move_resize (widget->window,
				allocation->x, allocation->y,
				allocation->width, allocation->height);
    }
}
 
static gint
gtk_juno_lfotrig_expose( GtkWidget      *widget,
			GdkEventExpose *event )
{
    GtkJunoLfotrig* ps = GTK_JUNO_LFOTRIG( widget );

    GdkColor     gray      = {0,0x2525,0x2525,0x2525};
    GdkColor     cream     = {0,0xdcdc,0xdcdc,0xb2b2};
    GdkGC*       blackgc   = widget->style->black_gc;
    GdkGC*       gc;
    GdkColormap* map;

    if (widget == NULL || widget->window == NULL)
	return FALSE;

    gc = gdk_gc_new( widget->window );
    map = gdk_colormap_new(gtk_widget_get_visual( widget ), TRUE);

    gdk_colormap_alloc_color(map, &gray, TRUE, TRUE);
    gdk_colormap_alloc_color(map, &cream, TRUE, TRUE);    

    gdk_draw_rectangle( widget->window, 
			blackgc, FALSE, 0, 0, 
			widget->allocation.width-1, widget->allocation.height-1);

    gdk_gc_set_foreground(gc, &cream);
    gdk_draw_rectangle( widget->window, 
			gc, TRUE, 1, 1, 
			widget->allocation.width-2, 
			widget->allocation.height-2);

    if ( ps->value )
    {
	gdk_draw_rectangle( widget->window, 
			    blackgc, FALSE, 1, 1, 
			    widget->allocation.width-3, widget->allocation.height-3);

	gdk_gc_set_foreground(gc, &gray);
	gdk_draw_line( widget->window, 
		       gc, 6, 4, widget->allocation.width - 7, 4 );
	
	gdk_draw_line( widget->window, 
		       gc, 6, widget->allocation.height- 5, 
		       widget->allocation.width - 7, 
		       widget->allocation.height- 5 );
	
	
	gdk_draw_line( widget->window, 
		       gc, 5, 5, 5, widget->allocation.height - 6 );
	
	gdk_draw_line( widget->window, 
		       gc, widget->allocation.width - 6, 
		       5, 
		       widget->allocation.width - 6, 
		       widget->allocation.height - 6 );
    }

    else
    {
	gdk_gc_set_foreground(gc, &gray);
	gdk_draw_line( widget->window, 
		       gc, 5, 4, widget->allocation.width - 6, 4 );
	
	gdk_draw_line( widget->window, 
		       gc, 5, widget->allocation.height- 5, widget->allocation.width - 6, widget->allocation.height- 5 );
	
	
	gdk_draw_line( widget->window, 
		       gc, 4, 5, 4, widget->allocation.height - 6 );
	
	gdk_draw_line( widget->window, 
		       gc, widget->allocation.width -5, 5, widget->allocation.width - 5, widget->allocation.height - 6 );
    }


    gdk_gc_destroy( gc );
    gdk_flush();
 
    return FALSE;  
}

int gtk_juno_lfotrig_get_value(GtkJunoLfotrig *ps)
{
    return ps->value;
}

void gtk_juno_lfotrig_set_value(GtkJunoLfotrig *ps, int value)
{
    ps->value = value;

    gtk_juno_lfotrig_expose(GTK_WIDGET(ps), NULL);
    gtk_signal_emit(GTK_OBJECT(ps), lfotrig_signals[VALUE_CHANGED]);
}

static gint gtk_juno_lfotrig_button_press(GtkWidget *widget, 
					  GdkEventButton *event)
{
    GtkJunoLfotrig *jlt = GTK_JUNO_LFOTRIG(widget);

    if (jlt->button == 0)
    {
	gtk_juno_lfotrig_set_value(jlt, 1 );
	jlt->button = event->button;
	gtk_grab_add(widget);
    }

    return TRUE;
}

static gint gtk_juno_lfotrig_button_release(GtkWidget *widget, 
					    GdkEventButton *event)
{
    GtkJunoLfotrig *jlt = GTK_JUNO_LFOTRIG(widget);

    if (jlt->button != 0 && jlt->button == event->button)
    {
	gtk_juno_lfotrig_set_value(jlt, 0 );
	jlt->button = 0;
	gtk_grab_remove(widget);
    }

    return TRUE;
}

