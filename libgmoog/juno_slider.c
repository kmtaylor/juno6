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
 * $Revision: 1.13.4.1 $$Date: 2000/02/24 00:43:59 $
 */
#include <math.h>
#include <stdio.h>
#include <gtk/gtkmain.h>
#include <gtk/gtksignal.h>
#include <gdk/gdk.h>

#include "juno_slider.h"

static GtkWidgetClass *parent_class = NULL;

static void
gtk_juno_slider_init( GtkJunoSlider *juno_slider );

static void
gtk_juno_slider_class_init( GtkJunoSliderClass *class );

static void
gtk_juno_slider_destroy( GtkObject *object );

static void
gtk_juno_slider_realize( GtkWidget *widget );

static gint
gtk_juno_slider_expose( GtkWidget      *widget,
			GdkEventExpose *event );

static gint
gtk_juno_slider_button_press( GtkWidget      *widget,
			      GdkEventButton *event );

static void 
gtk_juno_slider_size_request( GtkWidget      *widget,
			      GtkRequisition *requisition );

static void 
gtk_juno_slider_size_allocate( GtkWidget      *widget,
			       GtkAllocation *allocation );

GtkAdjustment*
gtk_juno_slider_get_adjustment( GtkJunoSlider *juno_slider );

void
gtk_juno_slider_set_update_policy( GtkJunoSlider *juno_slider,
				   GtkUpdateType  policy );

void
gtk_juno_slider_set_adjustment( GtkJunoSlider *juno_slider,
			       GtkAdjustment *adjustment );

static gint
gtk_juno_slider_button_release( GtkWidget      *widget,
				GdkEventButton *event );

static gint
gtk_juno_slider_motion_notify( GtkWidget      *widget,
			       GdkEventMotion *event );

//static void
//gtk_juno_slider_update_mouse( GtkJunoSlider *juno_slider, gint x, gint y );

static void
gtk_juno_slider_update( GtkJunoSlider *juno_slider );

static void
gtk_juno_slider_adjustment_changed( GtkAdjustment *adjustment,
				    gpointer       data );

static void
gtk_juno_slider_adjustment_value_changed( GtkAdjustment *adjustment,
					  gpointer       data );

GtkWidget*
gtk_juno_slider_new( GtkAdjustment *adjustment )
{
    GtkJunoSlider *juno_slider = gtk_type_new( gtk_juno_slider_get_type() );
    
    if( !adjustment )
	adjustment = (GtkAdjustment*)gtk_adjustment_new( 0.0, 0.0, 1.0, 0.01, 0.01, 0.01 );
    
    gtk_juno_slider_set_adjustment( juno_slider, adjustment );
    
    return GTK_WIDGET( juno_slider );
}

static void
gtk_juno_slider_destroy( GtkObject *object )
{
    GtkJunoSlider *juno_slider;
    
    g_return_if_fail( object != NULL );
    g_return_if_fail( GTK_IS_JUNO_SLIDER( object ) );
    
    juno_slider = GTK_JUNO_SLIDER( object );
    
    if ( juno_slider->adjustment )
	gtk_object_unref( GTK_OBJECT( juno_slider->adjustment ) );
    
    if ( GTK_OBJECT_CLASS (parent_class)->destroy )
	(* GTK_OBJECT_CLASS (parent_class)->destroy) (object);
}

guint
gtk_juno_slider_get_type()
{
    static guint juno_slider_type = 0;
    
    if (!juno_slider_type)
    {
	GtkTypeInfo juno_slider_info =
	{
	    "GtkJuno_Slider",
	    sizeof(GtkJunoSlider),
	    sizeof(GtkJunoSliderClass),
	    (GtkClassInitFunc)gtk_juno_slider_class_init,
	    (GtkObjectInitFunc)gtk_juno_slider_init,
	    (GtkArgSetFunc)NULL,
	    (GtkArgGetFunc)NULL,
	};
	
	juno_slider_type = gtk_type_unique( gtk_widget_get_type(), 
					    &juno_slider_info );
    }
    
    return(juno_slider_type);
}

static void
gtk_juno_slider_class_init( GtkJunoSliderClass *class )
{
    GtkObjectClass *object_class;
    GtkWidgetClass *widget_class;
    
    object_class = (GtkObjectClass*) class;
    widget_class = (GtkWidgetClass*) class;
    
    parent_class = gtk_type_class (gtk_widget_get_type ());
    
    object_class->destroy = gtk_juno_slider_destroy;
    
    widget_class->realize = gtk_juno_slider_realize;
    widget_class->expose_event = gtk_juno_slider_expose;
    widget_class->size_request = gtk_juno_slider_size_request;
    widget_class->size_allocate = gtk_juno_slider_size_allocate;
    widget_class->button_press_event = gtk_juno_slider_button_press;
    widget_class->button_release_event = gtk_juno_slider_button_release;
    widget_class->motion_notify_event = gtk_juno_slider_motion_notify;
}

static void
gtk_juno_slider_init( GtkJunoSlider *juno_slider )
{
    juno_slider->button = 0;
    juno_slider->policy = GTK_UPDATE_CONTINUOUS;
    juno_slider->old_value = 0.0;
    juno_slider->old_lower = 0.0;
    juno_slider->old_upper = 0.0;
    juno_slider->adjustment = NULL;
}

GtkAdjustment*
gtk_juno_slider_get_adjustment(GtkJunoSlider *juno_slider)
{
    g_return_val_if_fail (juno_slider != NULL, NULL);
    g_return_val_if_fail (GTK_IS_JUNO_SLIDER (juno_slider), NULL);
    
    return juno_slider->adjustment;
}
void
gtk_juno_slider_set_update_policy(GtkJunoSlider *juno_slider,
				  GtkUpdateType  policy)
{
    g_return_if_fail (juno_slider != NULL);
    g_return_if_fail (GTK_IS_JUNO_SLIDER (juno_slider));
    
    juno_slider->policy = policy;
}

void
gtk_juno_slider_set_adjustment( GtkJunoSlider *juno_slider,
				GtkAdjustment *adjustment )
{
    g_return_if_fail(juno_slider != NULL);
    g_return_if_fail(GTK_IS_JUNO_SLIDER (juno_slider));
    
    if (juno_slider->adjustment)
    {
	gtk_signal_disconnect_by_data(GTK_OBJECT (juno_slider->adjustment), (gpointer) juno_slider);
	gtk_object_unref(GTK_OBJECT (juno_slider->adjustment));
    }
    
    juno_slider->adjustment = adjustment;
    gtk_object_ref (GTK_OBJECT (juno_slider->adjustment));
    
    gtk_signal_connect (GTK_OBJECT (adjustment), "changed",
			(GtkSignalFunc) gtk_juno_slider_adjustment_changed,
			(gpointer) juno_slider);
    gtk_signal_connect (GTK_OBJECT (adjustment), "value_changed",
			(GtkSignalFunc) gtk_juno_slider_adjustment_value_changed,
			(gpointer) juno_slider);
    
    juno_slider->old_value = adjustment->value;
    juno_slider->old_lower = adjustment->lower;
    juno_slider->old_upper = adjustment->upper;
    
    gtk_juno_slider_update (juno_slider);
}

static void
gtk_juno_slider_realize (GtkWidget *widget)
{
//    GtkJunoSlider *juno_slider;
    GdkWindowAttr attributes;
    gint attributes_mask;
    
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_JUNO_SLIDER (widget));
    
    GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);
//    juno_slider = GTK_JUNO_SLIDER (widget);
    
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
gtk_juno_slider_size_request( GtkWidget      *widget,
			      GtkRequisition *requisition )
{
    requisition->width  = JUNO_SLIDER_DEFAULT_WIDTH;
    requisition->height = JUNO_SLIDER_DEFAULT_HEIGHT;
}

static void
gtk_juno_slider_size_allocate( GtkWidget     *widget,
			       GtkAllocation *allocation )
{
//    GtkJunoSlider *juno_slider;
    
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_JUNO_SLIDER (widget));
    g_return_if_fail (allocation != NULL);
    
    widget->allocation = *allocation;
    if (GTK_WIDGET_REALIZED (widget))
    {
//	juno_slider = GTK_JUNO_SLIDER (widget);
	
	gdk_window_move_resize (widget->window,
				allocation->x, allocation->y,
				allocation->width, allocation->height);
    }
}
 
static gint
gtk_juno_slider_expose( GtkWidget      *widget,
			GdkEventExpose *event )
{
    int y;

    GdkColormap * map; 
    GdkGC* gc;

    GdkColor dark       = {0,0x4040,0x4040,0x4040};
    GdkColor light      = {0,0x9999,0x9999,0x9999};
    GdkColor background = {0,0x3333,0x3333,0x3333};

    GdkGC* white       = widget->style->white_gc;
    GdkGC* black       = widget->style->black_gc;    

    if (widget == NULL || widget->window == NULL)
	return FALSE;

    map = gdk_colormap_new( gtk_widget_get_visual( widget ), TRUE );
    gc = gdk_gc_new( widget->window );

    gdk_colormap_alloc_color( map, &dark, TRUE, TRUE );
    gdk_colormap_alloc_color( map, &light, TRUE, TRUE );
    gdk_colormap_alloc_color( map, &background, TRUE, TRUE );

    // background
    gdk_gc_set_foreground( gc, &background );
    gdk_draw_rectangle( widget->window, 
			gc, TRUE, 0, 0, 
			widget->allocation.width, widget->allocation.height );
    
    // primary lines
    gdk_draw_line( widget->window, white, 0,  4, 13,  4 );    
    gdk_draw_line( widget->window, white, 0, 24, 13, 24 );    
    gdk_draw_line( widget->window, white, 0, 44, 13, 44 );    

    // secondary lines
    gdk_gc_set_foreground( gc, &light );
    gdk_draw_line( widget->window, gc, 0,  8, 13,  8 );    
    gdk_draw_line( widget->window, gc, 0, 12, 13, 12 );    
    gdk_draw_line( widget->window, gc, 0, 16, 13, 16 );    
    gdk_draw_line( widget->window, gc, 0, 20, 13, 20 );    

    gdk_draw_line( widget->window, gc, 0, 28, 13, 28 );    
    gdk_draw_line( widget->window, gc, 0, 32, 13, 32 );    
    gdk_draw_line( widget->window, gc, 0, 36, 13, 36 );    
    gdk_draw_line( widget->window, gc, 0, 40, 13, 40 );    

    // fader well
    gdk_draw_rectangle( widget->window, black, TRUE, 5, 1, 3, 47 );

    gdk_gc_set_foreground( gc, &dark );
    gdk_draw_line( widget->window, gc, 4,  1, 4,  47 );    
    gdk_draw_line( widget->window, gc, 8,  1, 8,  47 );    
    gdk_draw_line( widget->window, gc, 5,  0, 7,  0 );    
    gdk_draw_line( widget->window, gc, 5,  48, 7,  48 );    

    y = 40 - GTK_JUNO_SLIDER(widget)->adjustment->value * 40 + 4;

    // the fader
    gdk_draw_rectangle( widget->window, black, FALSE, 1, y - 2, 10, 4 );
    gdk_gc_set_foreground( gc, &dark );
    gdk_draw_line( widget->window, gc, 2,  y-1, 10,  y - 1 );    
    gdk_draw_line( widget->window, gc, 2,  y+1, 10,  y + 1 );    

    gdk_draw_line( widget->window, white, 2,  y, 10,  y );    

    gdk_flush();
    return FALSE;  
}

static gint
gtk_juno_slider_button_press( GtkWidget      *widget,
			      GdkEventButton *event )
{
    int y;
    GtkJunoSlider *juno_slider; 
    
    g_return_val_if_fail( widget != NULL, FALSE );
    g_return_val_if_fail( GTK_IS_JUNO_SLIDER (widget), FALSE );
    g_return_val_if_fail( event != NULL, FALSE );
    
    juno_slider = GTK_JUNO_SLIDER( widget );

    if (juno_slider->button != 0)
	return FALSE;

    juno_slider->button = event->button;

    if( event->button == 1 )
    {
        juno_slider->lastx = (gint)event->x;
        juno_slider->lasty = (gint)event->y;

	y = event->y;
	if ( y < 4 ) y = 4;
	if ( y > 44 ) y = 44;
	y -= 4;
	
	gtk_adjustment_set_value( juno_slider->adjustment, ( 40 - y ) / 40.0 );
    }

    juno_slider->lasty = event->y;
    gtk_grab_add(widget);

    return TRUE;
}

static gint
gtk_juno_slider_button_release (GtkWidget      *widget,
			 GdkEventButton *event)
{
    GtkJunoSlider *juno_slider;
    
    g_return_val_if_fail (widget != NULL, FALSE);
    g_return_val_if_fail (GTK_IS_JUNO_SLIDER (widget), FALSE);
    g_return_val_if_fail (event != NULL, FALSE);
    
    juno_slider = GTK_JUNO_SLIDER (widget);
    
    if (juno_slider->button == event->button)
    {
	gtk_grab_remove (widget);
	
	juno_slider->button = 0;
	
//	if (juno_slider->policy == GTK_UPDATE_DELAYED)
 //	    gtk_timeout_remove (juno_slider->timer);
	
	if ((juno_slider->policy != GTK_UPDATE_CONTINUOUS) &&
	    (juno_slider->old_value != juno_slider->adjustment->value))
	    gtk_signal_emit_by_name (GTK_OBJECT (juno_slider->adjustment), "value_changed");
    }
    
    return TRUE;
}

static gint
gtk_juno_slider_motion_notify (GtkWidget      *widget,
			GdkEventMotion *event)
{
    GtkJunoSlider *juno_slider;
    GdkModifierType mods;
    gint x, y; //mask;
    
    g_return_val_if_fail (widget != NULL, FALSE);
    g_return_val_if_fail (GTK_IS_JUNO_SLIDER (widget), FALSE);
    g_return_val_if_fail (event != NULL, FALSE);
    
    juno_slider = GTK_JUNO_SLIDER( widget );
   
    if (juno_slider->button != 0)
    {
	x = event->x;
	y = event->y;
	
	if (event->is_hint || (event->window != widget->window))
	    gdk_window_get_pointer (widget->window, &x, &y, &mods);

	switch (juno_slider->button)
	{
	case 1:
	    //mask = GDK_BUTTON1_MASK;
	    gtk_adjustment_set_value( juno_slider->adjustment,
				      juno_slider->adjustment->value +
				      ( juno_slider->lasty - y ) / 50.0 );
	    break;
	case 2:
	    //mask = GDK_BUTTON2_MASK;
	    gtk_adjustment_set_value( juno_slider->adjustment,
				      juno_slider->adjustment->value +
				      ( juno_slider->lasty - y ) / 100.0 );
	    break;
	case 3:
	    //mask = GDK_BUTTON3_MASK;
	    gtk_adjustment_set_value( juno_slider->adjustment,
				      juno_slider->adjustment->value +
				      ( juno_slider->lasty - y ) / 1000.0 );
	    break;
	default:
	    //mask = 0;
	    break;
	}

	juno_slider->lasty = (gint)y;
    }
    
    return FALSE;
}

/*
static void
gtk_juno_slider_update_mouse (GtkJunoSlider *juno_slider, gint x, gint y)
{
    gint xc, yc;
    gfloat old_value;
    
    g_return_if_fail (juno_slider != NULL);
    g_return_if_fail (GTK_IS_JUNO_SLIDER (juno_slider));
    
    xc = GTK_WIDGET(juno_slider)->allocation.width / 2;
    yc = GTK_WIDGET(juno_slider)->allocation.height / 2;
    
    old_value = juno_slider->adjustment->value;

    juno_slider->angle = atan2(yc-y, x-xc);

    if (juno_slider->angle < -M_PI/2.)
	juno_slider->angle += 2*M_PI;
    
    if (juno_slider->angle < -M_PI/6)
	juno_slider->angle = -M_PI/6;
    
    if (juno_slider->angle > 7.*M_PI/6.)
	juno_slider->angle = 7.*M_PI/6.;
    
    juno_slider->adjustment->value = juno_slider->adjustment->lower + (7.*M_PI/6 - juno_slider->angle) *
    (juno_slider->adjustment->upper - juno_slider->adjustment->lower) / (4.*M_PI/3.);  
    if (juno_slider->adjustment->value != old_value)
    {
	if (juno_slider->policy == GTK_UPDATE_CONTINUOUS)
	{
	    gtk_signal_emit_by_name (GTK_OBJECT (juno_slider->adjustment), "value_changed");
	}
	else
	{
	    gtk_widget_draw (GTK_WIDGET(juno_slider), NULL);
	    
	    if (juno_slider->policy == GTK_UPDATE_DELAYED)
	    {
		if (juno_slider->timer)
		    gtk_timeout_remove (juno_slider->timer);
		
		juno_slider->timer = gtk_timeout_add (SCROLL_DELAY_LENGTH,
					       (GtkFunction) gtk_juno_slider_timer,
					       (gpointer) juno_slider);
	    }
	}
    }
}
*/

static void
gtk_juno_slider_update( GtkJunoSlider *juno_slider )
{
    gfloat new_value;
    
    g_return_if_fail (juno_slider != NULL);
    g_return_if_fail (GTK_IS_JUNO_SLIDER (juno_slider));
    
    new_value = juno_slider->adjustment->value;
    
    if (new_value < juno_slider->adjustment->lower)
	new_value = juno_slider->adjustment->lower;
    
    if (new_value > juno_slider->adjustment->upper)
	new_value = juno_slider->adjustment->upper;
    
    if (new_value != juno_slider->adjustment->value)
    {
	juno_slider->adjustment->value = new_value;
	gtk_signal_emit_by_name (GTK_OBJECT (juno_slider->adjustment), "value_changed");
    }
        
    gtk_widget_draw (GTK_WIDGET(juno_slider), NULL);
}

static void
gtk_juno_slider_adjustment_changed( GtkAdjustment *adjustment,
				    gpointer       data )
{
    GtkJunoSlider *juno_slider;
    
    g_return_if_fail (adjustment != NULL);
    g_return_if_fail (data != NULL);
    
    juno_slider = GTK_JUNO_SLIDER (data);
    
    if ((juno_slider->old_value != adjustment->value) ||
	(juno_slider->old_lower != adjustment->lower) ||
	(juno_slider->old_upper != adjustment->upper))
    {
	gtk_juno_slider_update (juno_slider);
	
	juno_slider->old_value = adjustment->value;
	juno_slider->old_lower = adjustment->lower;
	juno_slider->old_upper = adjustment->upper;
    }
}

static void
gtk_juno_slider_adjustment_value_changed( GtkAdjustment *adjustment,
					  gpointer       data )
{
    GtkJunoSlider *juno_slider;
    
    g_return_if_fail (adjustment != NULL);
    g_return_if_fail (data != NULL);
    
    juno_slider = GTK_JUNO_SLIDER (data);
    
    if ( juno_slider->old_value != adjustment->value )
    {
	gtk_juno_slider_update (juno_slider);
	juno_slider->old_value = adjustment->value;
    }
}
