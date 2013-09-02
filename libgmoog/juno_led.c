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

#include "juno_led.h"

static GtkWidgetClass *parent_class = NULL;

static void 
gtk_juno_led_class_init( GtkJunoLedClass *class );
static void 
gtk_juno_led_init( GtkJunoLed *juno_led );
static void
gtk_juno_led_realize( GtkWidget *widget );
static gint
gtk_juno_led_expose( GtkWidget *widget, GdkEventExpose *event );
static void 
gtk_juno_led_size_request( GtkWidget *widget, GtkRequisition *requisition );
static void 
gtk_juno_led_size_allocate( GtkWidget *widget, GtkAllocation *allocation );

GtkWidget*     
gtk_juno_led_new()
{
    GtkJunoLed *juno_led = gtk_type_new( gtk_juno_led_get_type () );
    return GTK_WIDGET( juno_led );
}

guint
gtk_juno_led_get_type( void )
{
    static guint juno_led_type = 0;
	
    if ( !juno_led_type )
    {
	GtkTypeInfo juno_led_info =
	{
	    "GtkJuno_Led",
	    sizeof(GtkJunoLed),
	    sizeof(GtkJunoLedClass),
	    (GtkClassInitFunc)gtk_juno_led_class_init,
	    (GtkObjectInitFunc)gtk_juno_led_init,
	    (GtkArgSetFunc)NULL,
	    (GtkArgGetFunc)NULL,
	};
	
	juno_led_type = gtk_type_unique( gtk_widget_get_type(), 
					    &juno_led_info );
    }
    
    return( juno_led_type );
}

static void
gtk_juno_led_class_init( GtkJunoLedClass *class )
{
//    GtkObjectClass *object_class = (GtkObjectClass*) class;
    GtkWidgetClass *widget_class = (GtkWidgetClass*) class;

    parent_class = gtk_type_class( gtk_widget_get_type() );

    widget_class->realize = gtk_juno_led_realize;
    widget_class->expose_event = gtk_juno_led_expose;
    widget_class->size_request = gtk_juno_led_size_request;
    widget_class->size_allocate = gtk_juno_led_size_allocate;
}

static void
gtk_juno_led_init( GtkJunoLed *juno_led )
{
}

static void
gtk_juno_led_realize (GtkWidget *widget)
{
    //GtkJunoLed *juno_led;
    GdkWindowAttr attributes;
    gint attributes_mask;
    
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_JUNO_LED (widget));
    
    GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);
    //juno_led = GTK_JUNO_LED (widget);
    
    attributes.x = widget->allocation.x;
    attributes.y = widget->allocation.y;
    attributes.width = widget->allocation.width;
    attributes.height = widget->allocation.height;
    attributes.wclass = GDK_INPUT_OUTPUT;
    attributes.window_type = GDK_WINDOW_CHILD;
    attributes.event_mask = gtk_widget_get_events (widget) | 
    GDK_EXPOSURE_MASK;
    attributes.visual = gtk_widget_get_visual (widget);
    attributes.colormap = gtk_widget_get_colormap (widget);
    
    attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
    widget->window = gdk_window_new (widget->parent->window, &attributes, attributes_mask);
    
    widget->style = gtk_style_attach (widget->style, widget->window);
    
    gdk_window_set_user_data (widget->window, widget);
    
    gtk_style_set_background (widget->style, widget->window, GTK_STATE_ACTIVE);
}

static gint
gtk_juno_led_expose( GtkWidget      *widget,
		     GdkEventExpose *event )
{
    GtkJunoLed* led = GTK_JUNO_LED( widget );
    GdkColormap* map;
    GdkGC* gc;

    GdkColor dark       = {0,0x4c4c,0x1515,0x0a0a};
    GdkColor bright     = {0,0xE3E3,0x4141,0x1F1F};
    GdkColor white      = {0,0xFFFF,0xFFFF,0xFFFF};
    GdkColor background = {0,0x3333,0x3333,0x3333};

    GdkGC* black       = widget->style->black_gc;

    if (widget == NULL || widget->window == NULL)
	return FALSE;

    map = gdk_colormap_new( gtk_widget_get_visual( widget ), TRUE );
    gc = gdk_gc_new( widget->window );

    gdk_colormap_alloc_color( map, &dark, TRUE, TRUE );
    gdk_colormap_alloc_color( map, &bright, TRUE, TRUE );
    gdk_colormap_alloc_color( map, &white, TRUE, TRUE );
    gdk_colormap_alloc_color( map, &background, TRUE, TRUE );

    // background
    gdk_gc_set_foreground( gc, &background);
    gdk_draw_rectangle( widget->window, 
			gc, TRUE, 0, 0, widget->allocation.width, widget->allocation.height );

    // draw the color of the led
    gdk_gc_set_foreground( gc, led->state == 0 ? &dark : &bright );
    gdk_draw_rectangle( widget->window, gc, 
			TRUE, 1, 1, 5, 5 );

    // outline
    gdk_draw_point( widget->window, black, 1, 1 );
    gdk_draw_line( widget->window, black, 2, 0, 4, 0 );
    gdk_draw_point( widget->window, black, 5, 1 );
    gdk_draw_line( widget->window, black, 6, 2, 6, 4 );
    gdk_draw_point( widget->window, black, 1, 5 );
    gdk_draw_line( widget->window, black, 2, 6, 4, 6 );
    gdk_draw_point( widget->window, black, 5, 5 );
    gdk_draw_line( widget->window, black, 0, 2, 0, 4 );

    // hilight
    gdk_gc_set_foreground( gc, led->state == 0 ? &bright : &white );
    gdk_draw_line( widget->window, gc, 2, 2, 2, 3 );

    gdk_gc_destroy( gc );
    return TRUE;
}

void
gtk_juno_led_set_state( GtkJunoLed *juno_led,
			gint state )
{
    juno_led->state = state;
    gtk_juno_led_expose( &juno_led->widget, NULL );
}

gint
gtk_juno_led_get_state( GtkJunoLed *juno_led )
{
    return( juno_led->state );
}

static void 
gtk_juno_led_size_request( GtkWidget      *widget,
			   GtkRequisition *requisition )
{
    requisition->width  = 7;
    requisition->height = 7;
}

static void 
gtk_juno_led_size_allocate( GtkWidget *widget, GtkAllocation *allocation )
{
    //GtkJunoLed *juno_led;
    
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_JUNO_LED (widget));
    g_return_if_fail (allocation != NULL);
    
    widget->allocation = *allocation;
    if (GTK_WIDGET_REALIZED (widget))
    {
	//juno_led = GTK_JUNO_LED (widget);
	
	gdk_window_move_resize( widget->window,
				allocation->x, allocation->y,
				allocation->width, allocation->height );
    }
}











