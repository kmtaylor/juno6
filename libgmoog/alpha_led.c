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
 * $Revision: 1.6.4.1 $$Date: 2000/02/24 00:43:59 $
 */
#include <math.h>
#include <stdio.h>
#include <gtk/gtkmain.h>
#include <gtk/gtksignal.h>
#include <gdk/gdk.h>

#include "alpha_led.h"

static GtkWidgetClass *parent_class = NULL;

static void gtk_alpha_led_init( GtkAlpha_Led             *alpha_led );

static void gtk_alpha_led_class_init( GtkAlpha_LedClass  *class );

static void gtk_alpha_led_destroy( GtkObject            *object );

static void gtk_alpha_led_realize( GtkWidget            *widget );

static gint gtk_alpha_led_expose( GtkWidget             *widget,
				  GdkEventExpose        *event );

static void gtk_alpha_led_size_request( GtkWidget       *widget,
					GtkRequisition  *requisition );

static void gtk_alpha_led_size_allocate( GtkWidget      *widget,
					 GtkAllocation  *allocation );

static gint gtk_alpha_led_motion_notify( GtkWidget      *widget, 
					 GdkEventMotion *event );

static gint gtk_alpha_led_button_press(GtkWidget *widget, 
					 GdkEventButton *event);

static gint gtk_alpha_led_button_release(GtkWidget *widget, 
					 GdkEventButton *event);

enum 
{
    VALUE_CHANGED,
    LAST_SIGNAL
};

static int masks[] =
{
    1<<0 | 1<<1 | 1<<2 | 1<<3 | 1<<4 | 1<<5 | 0<<6,
    0<<0 | 0<<1 | 1<<2 | 1<<3 | 0<<4 | 0<<5 | 0<<6,
    0<<0 | 1<<1 | 1<<2 | 0<<3 | 1<<4 | 1<<5 | 1<<6,
    0<<0 | 1<<1 | 1<<2 | 1<<3 | 1<<4 | 0<<5 | 1<<6,
    1<<0 | 0<<1 | 1<<2 | 1<<3 | 0<<4 | 0<<5 | 1<<6,
    1<<0 | 1<<1 | 0<<2 | 1<<3 | 1<<4 | 0<<5 | 1<<6,
    1<<0 | 1<<1 | 0<<2 | 1<<3 | 1<<4 | 1<<5 | 1<<6,
    1<<0 | 1<<1 | 1<<2 | 1<<3 | 0<<4 | 0<<5 | 0<<6,
    1<<0 | 1<<1 | 1<<2 | 1<<3 | 1<<4 | 1<<5 | 1<<6,
    1<<0 | 1<<1 | 1<<2 | 1<<3 | 1<<4 | 0<<5 | 1<<6
};

static guint alpha_led_signals[LAST_SIGNAL] = {0};

guint
gtk_alpha_led_get_type()
{
    static guint alpha_led_type = 0;
    
    if (!alpha_led_type)
    {
	GtkTypeInfo alpha_led_info =
	{
	    "GtkAlpha_Led",
	    sizeof(GtkAlpha_Led),
	    sizeof(GtkAlpha_LedClass),
	    (GtkClassInitFunc)gtk_alpha_led_class_init,
	    (GtkObjectInitFunc)gtk_alpha_led_init,
	    (GtkArgSetFunc)NULL,
	    (GtkArgGetFunc)NULL,
	};
	
	alpha_led_type = gtk_type_unique( gtk_widget_get_type(), 
					  &alpha_led_info );
    }
    
    return(alpha_led_type);
}

static void
gtk_alpha_led_class_init (GtkAlpha_LedClass *class)
{
    GtkObjectClass *object_class;
    GtkWidgetClass *widget_class;
    
    object_class = (GtkObjectClass*) class;
    widget_class = (GtkWidgetClass*) class;
    
    parent_class = gtk_type_class (gtk_widget_get_type ());
    
    object_class->destroy = gtk_alpha_led_destroy;
    
    widget_class->realize = gtk_alpha_led_realize;
    widget_class->expose_event = gtk_alpha_led_expose;
    widget_class->size_request = gtk_alpha_led_size_request;
    widget_class->size_allocate = gtk_alpha_led_size_allocate;
    widget_class->motion_notify_event = gtk_alpha_led_motion_notify;
    widget_class->button_press_event = gtk_alpha_led_button_press;
    widget_class->button_release_event = gtk_alpha_led_button_release;

    alpha_led_signals[VALUE_CHANGED] = gtk_signal_new(
	"value_changed",
	GTK_RUN_FIRST,
	object_class->type, 
	GTK_SIGNAL_OFFSET(GtkAlpha_LedClass, value_changed),
	gtk_marshal_NONE__NONE,
	GTK_TYPE_NONE,
	0);

    gtk_object_class_add_signals(object_class, alpha_led_signals, LAST_SIGNAL);
}

static void
gtk_alpha_led_init (GtkAlpha_Led *alpha_led)
{
}

GtkWidget*
gtk_alpha_led_new()
{
    GtkAlpha_Led *alpha_led;
    
    alpha_led = gtk_type_new (gtk_alpha_led_get_type ());
    
    return GTK_WIDGET (alpha_led);
}

static void
gtk_alpha_led_destroy (GtkObject *object)
{
    if (GTK_OBJECT_CLASS (parent_class)->destroy)
	(* GTK_OBJECT_CLASS (parent_class)->destroy) (object);
}

static void
gtk_alpha_led_realize (GtkWidget *widget)
{
    //GtkAlpha_Led *alpha_led;
    GdkWindowAttr attributes;
    gint attributes_mask;
    
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_ALPHA_LED (widget));
    
    GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);
    //alpha_led = GTK_ALPHA_LED (widget);
    
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
gtk_alpha_led_size_request( GtkWidget      *widget,
			      GtkRequisition *requisition )
{
    requisition->width  = ALPHA_LED_DEFAULT_WIDTH;
    requisition->height = ALPHA_LED_DEFAULT_HEIGHT;
}

static void
gtk_alpha_led_size_allocate( GtkWidget     *widget,
			       GtkAllocation *allocation )
{
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_ALPHA_LED (widget));
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
gtk_alpha_led_expose( GtkWidget      *widget,
		      GdkEventExpose *event )
{
    GtkAlpha_Led *alpha_led = GTK_ALPHA_LED( widget );
    GdkColormap  *map;
    GdkGC        *gc;

    GdkColor ground   = {0,0x3333,0x3333,0x3333};
    GdkColor darkred  = {0,0xc4c4,0x0808,0x0808};
    GdkColor red      = {0,0xFFFF,0x0000,0x0000};
    
    if (widget == NULL || widget->window == NULL)
	return FALSE;

    map = gdk_colormap_new( gtk_widget_get_visual( widget ), TRUE );
    gc = gdk_gc_new( widget->window );

    gdk_colormap_alloc_color( map, &ground, TRUE, TRUE );
    gdk_colormap_alloc_color( map, &darkred, TRUE, TRUE );
    gdk_colormap_alloc_color( map, &red, TRUE, TRUE );

    gdk_gc_set_foreground( gc, &ground );    

    gdk_draw_rectangle( widget->window, 
                        gc, 
			TRUE, 0, 0, 
			widget->allocation.width, widget->allocation.height );
    
    gdk_draw_rectangle( widget->window, 
			widget->style->black_gc, FALSE, 0, 0, 
			widget->allocation.width - 1, 
			widget->allocation.height - 1 );

    if ( masks[ alpha_led->value ] & (1 << 0) )
    {
	gdk_gc_set_foreground( gc, &darkred );    
	gdk_draw_line( widget->window, gc, 2, 3, 2, 7 );
	gdk_gc_set_foreground( gc, &red );    
	gdk_draw_line( widget->window, gc, 3, 4, 3, 7 );
    }

    if ( masks[ alpha_led->value ] & (1 << 1) )
    {
	gdk_gc_set_foreground( gc, &darkred );    
	gdk_draw_line( widget->window, gc, 3, 2, 9, 2 );
	gdk_gc_set_foreground( gc, &red );    
	gdk_draw_line( widget->window, gc, 4, 3, 8, 3 );
    }

    if ( masks[ alpha_led->value ] & (1 << 2) )
    {
	gdk_gc_set_foreground( gc, &darkred );    
	gdk_draw_line( widget->window, gc, 10, 3, 10, 7 );
	gdk_gc_set_foreground( gc, &red );    
	gdk_draw_line( widget->window, gc, 9, 4, 9, 7 );
    }

    if ( masks[ alpha_led->value ] & (1 << 3) )
    {
	gdk_gc_set_foreground( gc, &darkred );    
	gdk_draw_line( widget->window, gc, 10, 10, 10, 15 );
	gdk_gc_set_foreground( gc, &red );    
	gdk_draw_line( widget->window, gc, 9, 10, 9, 14 );
    }

    if ( masks[ alpha_led->value ] & (1 << 4) )
    {
	gdk_gc_set_foreground( gc, &darkred );    
	gdk_draw_line( widget->window, gc, 3, 16, 9, 16 );
	gdk_gc_set_foreground( gc, &red );    
	gdk_draw_line( widget->window, gc, 4, 15, 8, 15 );
    }

    if ( masks[ alpha_led->value ] & (1 << 5) )
    {
	gdk_gc_set_foreground( gc, &darkred );    
	gdk_draw_line( widget->window, gc, 2, 10, 2, 15 );
	gdk_gc_set_foreground( gc, &red );    
	gdk_draw_line( widget->window, gc, 3, 10, 3, 14 );
    }

    if ( masks[ alpha_led->value ] & (1 << 6) )
    {
	gdk_gc_set_foreground( gc, &red );    
	gdk_draw_line( widget->window, gc, 4, 8, 8, 8 );
	gdk_draw_line( widget->window, gc, 4, 9, 8, 9 );
    }

    gdk_gc_destroy( gc );

    return FALSE;  
}

static gint
gtk_alpha_led_motion_notify (GtkWidget      *widget,
			     GdkEventMotion *event)
{
    //GtkAlpha_Led *alpha_led;
    GdkModifierType state;
    gint x, y;
    
    g_return_val_if_fail (widget != NULL, FALSE);
    g_return_val_if_fail (GTK_IS_ALPHA_LED (widget), FALSE);
    g_return_val_if_fail (event != NULL, FALSE);
    
    //alpha_led = GTK_ALPHA_LED( widget );
    
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
    }
    
    return FALSE;
}

double gtk_alpha_led_get_value(GtkAlpha_Led *jb)
{
    return jb->value;
}

void gtk_alpha_led_set_value(GtkAlpha_Led *jb, int value)
{
    while ( value < 0 )
	value += 10;

    while ( value > 9 )
	value -= 10;

    jb->value = value;

    //printf("alpha_led value= %d\n", value);

    gtk_alpha_led_expose(GTK_WIDGET(jb), NULL);
    gtk_signal_emit(GTK_OBJECT(jb), alpha_led_signals[VALUE_CHANGED]);
}

static gint 
gtk_alpha_led_button_press(GtkWidget *widget, 
			     GdkEventButton *event)
{
    GtkAlpha_Led* led = GTK_ALPHA_LED( widget );

    if ( event->type != GDK_BUTTON_PRESS )
	return( TRUE );

    if ( event->y > 10 )
	gtk_alpha_led_set_value( led, led->value - 1 );

    else
	gtk_alpha_led_set_value( led, led->value + 1 );

    //printf("alpha_led button press %f %d %d\n", event->x, event->state, event->button);
    return TRUE;
}

static gint
gtk_alpha_led_button_release(GtkWidget *widget, 
			       GdkEventButton *event)
{
    return TRUE;
}

