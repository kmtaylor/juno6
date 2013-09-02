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
 * $Revision: 1.7.4.2 $$Date: 2000/02/25 20:40:43 $
 */
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <gtk/gtkmain.h>
#include <gtk/gtksignal.h>
#include <gdk/gdk.h>
#include <stdlib.h>
#include "scope2.h"
#include <util/fastmemcpy.h>

static GtkWidgetClass *parent_class = NULL;

static void
gtk_scope_init( GtkScope *scope );

static void
gtk_scope_class_init( GtkScopeClass *class );

static void
gtk_scope_destroy( GtkObject *object );

static void
gtk_scope_realize( GtkWidget *widget );

static gint
gtk_scope_expose( GtkWidget      *widget,
			GdkEventExpose *event );

static gint
gtk_scope_button_press( GtkWidget      *widget,
			      GdkEventButton *event );

static void 
gtk_scope_size_request( GtkWidget      *widget,
			      GtkRequisition *requisition );

static void 
gtk_scope_size_allocate( GtkWidget      *widget,
			       GtkAllocation *allocation );

void
gtk_scope_set_update_policy( GtkScope *scope,
				   GtkUpdateType  policy );

static gint
gtk_scope_button_release( GtkWidget      *widget,
				GdkEventButton *event );

static gint
gtk_scope_motion_notify( GtkWidget      *widget,
			       GdkEventMotion *event );

enum {
  PRESSED,
  RELEASED,
  CLICKED,
  ENTER,
  LEAVE,
  LAST_SIGNAL
};

static guint scope_signals[LAST_SIGNAL] = { 0 };

GtkWidget*
gtk_scope_new()
{
    GtkScope *scope = gtk_type_new( gtk_scope_get_type() );
    return GTK_WIDGET( scope );
}

static void
gtk_scope_destroy( GtkObject *object )
{
    //GtkScope *scope;
    
    g_return_if_fail( object != NULL );
    g_return_if_fail( GTK_IS_SCOPE( object ) );
    
    //scope = GTK_SCOPE( object );
    
    if ( GTK_OBJECT_CLASS (parent_class)->destroy )
	(* GTK_OBJECT_CLASS (parent_class)->destroy) (object);
}

guint
gtk_scope_get_type()
{
    static guint scope_type = 0;
    
    if (!scope_type)
    {
	GtkTypeInfo scope_info =
	{
	    "GtkScope",
	    sizeof(GtkScope),
	    sizeof(GtkScopeClass),
	    (GtkClassInitFunc)gtk_scope_class_init,
	    (GtkObjectInitFunc)gtk_scope_init,
	    (GtkArgSetFunc)NULL,
	    (GtkArgGetFunc)NULL,
	};
	
	scope_type = gtk_type_unique( gtk_widget_get_type(), 
					    &scope_info );
    }
    
    return(scope_type);
}

static void
gtk_scope_class_init( GtkScopeClass *class )
{
    GtkObjectClass *object_class;
    GtkWidgetClass *widget_class;
    
    object_class = (GtkObjectClass*) class;
    widget_class = (GtkWidgetClass*) class;
    
    parent_class = gtk_type_class (gtk_widget_get_type ());
    
    object_class->destroy = gtk_scope_destroy;
    
    widget_class->realize = gtk_scope_realize;
    widget_class->expose_event = gtk_scope_expose;
    widget_class->size_request = gtk_scope_size_request;
    widget_class->size_allocate = gtk_scope_size_allocate;
    widget_class->button_press_event = gtk_scope_button_press;
    widget_class->button_release_event = gtk_scope_button_release;
    widget_class->motion_notify_event = gtk_scope_motion_notify;

    scope_signals[PRESSED] =
    gtk_signal_new ("pressed",
                    GTK_RUN_FIRST,
                    object_class->type,
                    GTK_SIGNAL_OFFSET (GtkScopeClass, pressed),
                    gtk_marshal_NONE__NONE,
                    GTK_TYPE_NONE, 0);
    scope_signals[RELEASED] =
    gtk_signal_new ("released",
                    GTK_RUN_FIRST,
                    object_class->type,
                    GTK_SIGNAL_OFFSET (GtkScopeClass, released),
                    gtk_marshal_NONE__NONE,
                    GTK_TYPE_NONE, 0);
    scope_signals[CLICKED] =
    gtk_signal_new ("clicked",
                    GTK_RUN_FIRST | GTK_RUN_ACTION,
                    object_class->type,
                    GTK_SIGNAL_OFFSET (GtkScopeClass, clicked),
                    gtk_marshal_NONE__NONE,
                    GTK_TYPE_NONE, 0);
    scope_signals[ENTER] =
    gtk_signal_new ("enter",
                    GTK_RUN_FIRST,
                    object_class->type,
                    GTK_SIGNAL_OFFSET (GtkScopeClass, enter),
                    gtk_marshal_NONE__NONE,
                    GTK_TYPE_NONE, 0);
    scope_signals[LEAVE] =
    gtk_signal_new ("leave",
                    GTK_RUN_FIRST,
                    object_class->type,
                    GTK_SIGNAL_OFFSET (GtkScopeClass, leave),
                    gtk_marshal_NONE__NONE,
                    GTK_TYPE_NONE, 0);
    
  gtk_object_class_add_signals (object_class, scope_signals, LAST_SIGNAL);

}

static void
gtk_scope_init( GtkScope *scope )
{
    scope->button = 0;
    scope->pos = 0;
    scope->resetCounter = 0;
    scope->wavelength = 0;
//    button->policy = GTK_UPDATE_CONTINUOUS;
}

void
gtk_scope_set_update_policy(GtkScope *scope,
				  GtkUpdateType  policy)
{
    g_return_if_fail (scope != NULL);
    g_return_if_fail (GTK_IS_SCOPE (scope));
    
//    scope->policy = policy;
}

static void
gtk_scope_realize (GtkWidget *widget)
{
    //GtkScope *scope;
    GdkWindowAttr attributes;
    gint attributes_mask;
    
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_SCOPE (widget));
    
    GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);
    //scope = GTK_SCOPE (widget);
    
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
gtk_scope_size_request( GtkWidget      *widget,
			GtkRequisition *requisition )
{
    requisition->width  = SCOPE_DEFAULT_WIDTH;
    requisition->height = SCOPE_DEFAULT_HEIGHT;
}

GdkColor green     = {0,0x0000,0xFFFF,0x0000};

static void
gtk_scope_size_allocate( GtkWidget     *widget,
			 GtkAllocation *allocation )
{
    GtkScope *scope;
    
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_SCOPE (widget));
    g_return_if_fail (allocation != NULL);
    
    widget->allocation = *allocation;
    if (GTK_WIDGET_REALIZED (widget))
    {
	GdkColormap* map;
	GdkColor color = {0,0x0000,0x8080,0x0000};

	scope = GTK_SCOPE (widget);
	
	gdk_window_move_resize (widget->window,
				allocation->x, allocation->y,
				allocation->width, allocation->height);

	//FIXME: try to free this buffer sometime
	scope->points = (double*)calloc( allocation->width, sizeof( double ) );
	scope->width    = allocation->width;
	scope->height   = allocation->height;
	scope->height_2 = allocation->height / 2;

	map = gdk_colormap_new( gtk_widget_get_visual( widget ), 
				TRUE );
	
	scope->darkGreen = gdk_gc_new( widget->window );
	gdk_colormap_alloc_color( map, &color, TRUE, TRUE );
	gdk_gc_set_foreground( scope->darkGreen, &color );

	widget->style->fg[4] = green;
	gdk_colormap_alloc_color( map, &widget->style->fg[4], TRUE, TRUE );
	gdk_gc_set_foreground( widget->style->fg_gc[4], &widget->style->fg[4] );
	scope->fixed = gdk_font_load( "fixed" );
    }
}
 
static gint
gtk_scope_expose( GtkWidget      *widget,
			GdkEventExpose *event )
{
    return FALSE;  
}

static gint
gtk_scope_button_press( GtkWidget      *widget,
			      GdkEventButton *event )
{
    GtkScope *scope; 
    
    g_return_val_if_fail( widget != NULL, FALSE );
    g_return_val_if_fail( GTK_IS_SCOPE (widget), FALSE );
    g_return_val_if_fail( event != NULL, FALSE );
    
    scope = GTK_SCOPE( widget );

    scope->button = event->button;

    if (event->type == GDK_BUTTON_PRESS)
    {
	if (!GTK_WIDGET_HAS_FOCUS (widget))
	    gtk_widget_grab_focus (widget);

	if( event->button == 1 )
	{
	    gtk_grab_add( widget );
	}
    }

    return FALSE;
}

static gint
gtk_scope_button_release (GtkWidget      *widget,
			 GdkEventButton *event)
{
    //GtkScope *scope;
    
    g_return_val_if_fail (widget != NULL, FALSE);
    g_return_val_if_fail (GTK_IS_SCOPE (widget), FALSE);
    g_return_val_if_fail (event != NULL, FALSE);
    
    //scope = GTK_SCOPE (widget);
    
    if( event->button == 1 )
    {
	gtk_grab_remove( widget ); // what does this do?
    }
    
    return FALSE;
}

static gint
gtk_scope_motion_notify (GtkWidget      *widget,
			GdkEventMotion *event)
{
    GtkScope *scope;
    GdkModifierType mods;
    gint x, y; // mask;
    
    g_return_val_if_fail (widget != NULL, FALSE);
    g_return_val_if_fail (GTK_IS_SCOPE (widget), FALSE);
    g_return_val_if_fail (event != NULL, FALSE);
    
    scope = GTK_SCOPE( widget );
   
    if (scope->button != 0)
    {
	x = event->x;
	y = event->y;
	
	if (event->is_hint || (event->window != widget->window))
	    gdk_window_get_pointer (widget->window, &x, &y, &mods);
	
	switch (scope->button)
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
//	    gtk_button_update_mouse (button, x,y);

	gtk_scope_expose( widget, NULL );
    }
    
    return FALSE;
}

#if 0
static void
gtk_scope_update( GtkScope *scope )
{
    g_return_if_fail (scope != NULL);
    g_return_if_fail (GTK_IS_SCOPE (scope));
    
    gtk_widget_draw (GTK_WIDGET(scope), NULL);
}
#endif

//#define RELATIVE_GRID
#define ABSOLUTE_GRID

void gtk_scope_draw( GtkScope* scope )
{
    int i;
    double lastpoint, point;
    int width = scope->pos;
    char hz[64];

    GtkWidget* widget = GTK_WIDGET( scope );
    
    if ( width > scope->width )
	width = scope->width;
    
    gdk_draw_rectangle( widget->window,
			widget->style->black_gc,
			TRUE,
			0, 0, scope->width, scope->height );
    
#ifdef RELATIVE_GRID
    for( i = 0; i < 10; i++ )
    {
	double scale = i / 10.0;
	
	gdk_draw_line( widget->window,
		       scope->darkGreen,
		       0, 
		       scope->height * scale,
		       width,
		       scope->height * scale );
	
	gdk_draw_line( widget->window,
		       scope->darkGreen,
		       width * scale, 
		       0,
		       width * scale, 
		       scope->height );
	
    }
#endif
    
#ifdef ABSOLUTE_GRID
    for( i = 0; i < width; i += 20 )
    {
	gdk_draw_line( widget->window,
		       scope->darkGreen,
		       i, 
		       0,
		       i, 
		       scope->height );
	
    }
    
    for( i = 0; i < 10; i++ )
    {
	double scale = i / 10.0;
	
	gdk_draw_line( widget->window,
		       scope->darkGreen,
		       0, 
		       scope->height * scale,
		       width,
		       scope->height * scale );
    }
#endif
    
    sprintf( hz, "%.2f hz", 44100.0 / scope->wavelength );

    gdk_draw_rectangle( widget->window, 
			widget->style->black_gc,
			TRUE, 1, 1, 41, 19 );
    
    gdk_draw_string( widget->window,
		     scope->fixed,
		     scope->darkGreen,
		     4, 14, hz );
    
    gdk_draw_line( widget->window,
		   widget->style->white_gc,
		   0, 
		   scope->height * .5,
		   width,
		   scope->height * .5 );
    
    lastpoint = scope->height - ( scope->height_2 + scope->height_2 * scope->points[0] );
    
    for( i = 1; i < scope->pos; i++ )
    {
	point = scope->height - ( scope->height_2 + scope->height_2 * scope->points[i] );
	gdk_draw_line( widget->window,
		       widget->style->fg_gc[4], 
		       i - 1, 
		       lastpoint,
		       i, 
		       point );
	    
	lastpoint = point;
    }

    scope->pos = 0;
    gdk_flush();
}

int gtk_scope_plot( GtkScope* scope, int reset, double data )
{
    if ( scope->points == NULL )
	return(0);

    if ( scope->pos < scope->width )
	scope->points[ scope->pos++ ] = data;

    scope->resetCounter++;

    if ( reset )
    {
	scope->wavelength = scope->resetCounter;
	scope->resetCounter = 0;

	if ( scope->pos >= scope->width )
	{
	    return(1);
	}
    }

    return( 0 );
}

void
gtk_scope_plot_buff( GtkScope* scope, double* data, size_t n )
{
    if ( n > scope->width )
	scope->pos = scope->width;
    else
	scope->pos = n;

    memcpy( scope->points, data, scope->pos * sizeof( double ) );

    gtk_scope_draw( scope );
}

