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
 * $Revision: 1.8.4.1 $$Date: 2000/02/24 00:43:59 $
 */
#include <math.h>
#include <stdio.h>
#include <gtk/gtkmain.h>
#include <gtk/gtksignal.h>
#include <gdk/gdk.h>

#include "juno_switch.h"
#include "juno_switch_pixmaps.h"

enum {
    GTK_JUNO_SWITCH_2WAY_UP_PIX,
    GTK_JUNO_SWITCH_2WAY_DOWN_PIX,
    GTK_JUNO_SWITCH_3WAY_UP_PIX,
    GTK_JUNO_SWITCH_3WAY_DOWN_PIX,
    GTK_JUNO_SWITCH_3WAY_MID_PIX
};

static GdkPixmap *switch_pixmaps[5];
static GtkWidgetClass *parent_class = NULL;
static guint switch_signals[1] = {0};

static void
gtk_juno_switch_init( GtkJunoSwitch *juno_switch );

static void
gtk_juno_switch_class_init( GtkJunoSwitchClass *class );

static void
gtk_juno_switch_destroy( GtkObject *object );

static void
gtk_juno_switch_realize( GtkWidget *widget );

static gint
gtk_juno_switch_expose( GtkWidget      *widget,
			GdkEventExpose *event );

static gint
gtk_juno_switch_button_press( GtkWidget      *widget,
			      GdkEventButton *event );

static void 
gtk_juno_switch_size_request( GtkWidget      *widget,
			      GtkRequisition *requisition );

static void 
gtk_juno_switch_size_allocate( GtkWidget      *widget,
			       GtkAllocation *allocation );

void
gtk_juno_switch_set_update_policy( GtkJunoSwitch *juno_switch,
				   GtkUpdateType  policy );

static gint
gtk_juno_switch_button_release( GtkWidget      *widget,
				GdkEventButton *event );

static gint
gtk_juno_switch_motion_notify( GtkWidget      *widget,
			       GdkEventMotion *event );

GtkWidget*
gtk_juno_switch_new( GtkJunoSwitchType type )
{
    GtkJunoSwitch *juno_switch = gtk_type_new( gtk_juno_switch_get_type() );
    juno_switch->type = type;
    return GTK_WIDGET( juno_switch );
}

static void
gtk_juno_switch_destroy( GtkObject *object )
{
    //GtkJunoSwitch *juno_switch;
    
    g_return_if_fail( object != NULL );
    g_return_if_fail( GTK_IS_JUNO_SWITCH( object ) );
    
    //juno_switch = GTK_JUNO_SWITCH( object );
    
    if ( GTK_OBJECT_CLASS (parent_class)->destroy )
	(* GTK_OBJECT_CLASS (parent_class)->destroy) (object);
}

guint
gtk_juno_switch_get_type()
{
    static guint juno_switch_type = 0;
    
    if (!juno_switch_type)
    {
	GtkTypeInfo juno_switch_info =
	{
	    "GtkJuno_Switch",
	    sizeof(GtkJunoSwitch),
	    sizeof(GtkJunoSwitchClass),
	    (GtkClassInitFunc)gtk_juno_switch_class_init,
	    (GtkObjectInitFunc)gtk_juno_switch_init,
	    (GtkArgSetFunc)NULL,
	    (GtkArgGetFunc)NULL,
	};
	
	juno_switch_type = gtk_type_unique( gtk_widget_get_type(), 
					    &juno_switch_info );
    }
    
    return(juno_switch_type);
}

static void
gtk_juno_switch_class_init( GtkJunoSwitchClass *class )
{
    GtkObjectClass *object_class;
    GtkWidgetClass *widget_class;
    
    object_class = (GtkObjectClass*) class;
    widget_class = (GtkWidgetClass*) class;
    
    parent_class = gtk_type_class (gtk_widget_get_type ());
    
    object_class->destroy = gtk_juno_switch_destroy;
    
    widget_class->realize = gtk_juno_switch_realize;
    widget_class->expose_event = gtk_juno_switch_expose;
    widget_class->size_request = gtk_juno_switch_size_request;
    widget_class->size_allocate = gtk_juno_switch_size_allocate;
    widget_class->button_press_event = gtk_juno_switch_button_press;
    widget_class->button_release_event = gtk_juno_switch_button_release;
    widget_class->motion_notify_event = gtk_juno_switch_motion_notify;

    switch_signals[0] = 
    gtk_signal_new ("switched",
                    GTK_RUN_FIRST,
                    object_class->type,
                    GTK_SIGNAL_OFFSET (GtkJunoSwitchClass, leave),
                    gtk_marshal_NONE__NONE,
                    GTK_TYPE_NONE, 0);
    
    gtk_object_class_add_signals (object_class, switch_signals, 1);
}

static void
gtk_juno_switch_init( GtkJunoSwitch *juno_switch )
{
    juno_switch->pos = GTK_JUNO_SWITCH_POS_UP;
//    juno_switch->policy = GTK_UPDATE_CONTINUOUS;
}

void
gtk_juno_switch_set_update_policy(GtkJunoSwitch *juno_switch,
				  GtkUpdateType  policy)
{
    g_return_if_fail (juno_switch != NULL);
    g_return_if_fail (GTK_IS_JUNO_SWITCH (juno_switch));
    
//    juno_switch->policy = policy;
}

static void
gtk_juno_switch_realize (GtkWidget *widget)
{
    //GtkJunoSwitch *juno_switch;
    GdkWindowAttr attributes;
    gint attributes_mask;
    
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_JUNO_SWITCH (widget));
    
    GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);
    //juno_switch = GTK_JUNO_SWITCH (widget);
    
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

    /* initialize the pixmaps once only */
    if (switch_pixmaps[0] == NULL)
    {
	switch_pixmaps[GTK_JUNO_SWITCH_2WAY_UP_PIX] =
	gdk_pixmap_create_from_xpm_d(widget->window,
				     NULL,
				     //FIXME: transparency!
				     NULL,
				     sw_2way_up_pix);

	switch_pixmaps[GTK_JUNO_SWITCH_2WAY_DOWN_PIX] =
	gdk_pixmap_create_from_xpm_d(widget->window,
				     NULL,
				     //FIXME: transparency!
				     NULL,
				     sw_2way_down_pix);

	switch_pixmaps[GTK_JUNO_SWITCH_3WAY_UP_PIX] =
	gdk_pixmap_create_from_xpm_d(widget->window,
				     NULL,
				     //FIXME: transparency!
				     NULL,
				     sw_3way_up_pix);

	switch_pixmaps[GTK_JUNO_SWITCH_3WAY_DOWN_PIX] =
	gdk_pixmap_create_from_xpm_d(widget->window,
				     NULL,
				     //FIXME: transparency!
				     NULL,
				     sw_3way_down_pix);

	switch_pixmaps[GTK_JUNO_SWITCH_3WAY_MID_PIX] =
	gdk_pixmap_create_from_xpm_d(widget->window,
				     NULL,
				     //FIXME: transparency!
				     NULL,
				     sw_3way_mid_pix);
    }
}

static void 
gtk_juno_switch_size_request( GtkWidget      *widget,
			      GtkRequisition *requisition )
{
    requisition->width  = GTK_JUNO_SWITCH_DEFAULT_WIDTH;
    requisition->height = GTK_JUNO_SWITCH_DEFAULT_HEIGHT;
}

static void
gtk_juno_switch_size_allocate( GtkWidget     *widget,
			       GtkAllocation *allocation )
{
    //GtkJunoSwitch *juno_switch;
    
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_JUNO_SWITCH (widget));
    g_return_if_fail (allocation != NULL);
    
    widget->allocation = *allocation;
    if (GTK_WIDGET_REALIZED (widget))
    {
	//juno_switch = GTK_JUNO_SWITCH (widget);
	
	gdk_window_move_resize (widget->window,
				allocation->x, allocation->y,
				allocation->width, allocation->height);
    }
}
 
static gint
gtk_juno_switch_expose( GtkWidget      *widget,
			GdkEventExpose *event )
{
    GtkJunoSwitch* juno_switch = GTK_JUNO_SWITCH (widget);
    GdkGC* gc;
    int index                  = juno_switch->pos;

    if (widget == NULL || widget->window == NULL)
	return FALSE;

    gc = gdk_gc_new( widget->window );

    if (juno_switch->type == GTK_JUNO_SWITCH_3WAY)
	index += 2;

    gdk_draw_pixmap(widget->window,
		    gc,
		    switch_pixmaps[index],
		    0,
		    0,
		    0,
		    0,
		    GTK_JUNO_SWITCH_DEFAULT_WIDTH,
		    GTK_JUNO_SWITCH_DEFAULT_HEIGHT
		    );
		    
    
    gdk_gc_destroy( gc );
    gdk_flush();

    return FALSE;  
}

static gint
gtk_juno_switch_button_press( GtkWidget      *widget,
			      GdkEventButton *event )
{
    GtkJunoSwitch *juno_switch; 
    
    g_return_val_if_fail( widget != NULL, FALSE );
    g_return_val_if_fail( GTK_IS_JUNO_SWITCH (widget), FALSE );
    g_return_val_if_fail( event != NULL, FALSE );

    juno_switch = GTK_JUNO_SWITCH( widget );

    if (event->type == GDK_BUTTON_PRESS)
    {
	if (!GTK_WIDGET_HAS_FOCUS (widget))
	    gtk_widget_grab_focus (widget);

	if( event->button == 1 )
	{
	    gtk_grab_add( widget );
	    gtk_juno_switch_toggle_dir(juno_switch, (event->y >=  GTK_JUNO_SWITCH_DEFAULT_HEIGHT / 2));
	}
    }

    return TRUE;
}

static gint
gtk_juno_switch_button_release (GtkWidget      *widget,
			 GdkEventButton *event)
{
    //GtkJunoSwitch *juno_switch;
    
    g_return_val_if_fail (widget != NULL, FALSE);
    g_return_val_if_fail (GTK_IS_JUNO_SWITCH (widget), FALSE);
    g_return_val_if_fail (event != NULL, FALSE);
    
    //juno_switch = GTK_JUNO_SWITCH (widget);
    
    if( event->button == 1 )
	gtk_grab_remove( widget );
    
    return TRUE;
}

static gint
gtk_juno_switch_motion_notify (GtkWidget      *widget,
			GdkEventMotion *event)
{
/*
    GtkJunoSwitch *juno_switch;
    GdkModifierType mods;
    gint x, y, mask;
    
    g_return_val_if_fail (widget != NULL, FALSE);
    g_return_val_if_fail (GTK_IS_JUNO_SWITCH (widget), FALSE);
    g_return_val_if_fail (event != NULL, FALSE);
    
    printf( "switch motion notify\n" );

    juno_switch = GTK_JUNO_SWITCH( widget );
   
    if (juno_switch->pos != 0)
    {
	x = event->x;
	y = event->y;
	
	if (event->is_hint || (event->window != widget->window))
	    gdk_window_get_pointer (widget->window, &x, &y, &mods);
	
	switch (juno_switch->pos)
	{
	case 1:
	    mask = GDK_BUTTON1_MASK;
	    break;
	case 2:
	    mask = GDK_BUTTON2_MASK;
	    break;
	case 3:
	    mask = GDK_BUTTON3_MASK;
	    break;
	default:
	    mask = 0;
	    break;
	}
	
//	if (mods & mask)
//	    gtk_juno_switch_update_mouse (juno_switch, x,y);

	gtk_juno_switch_expose( widget, NULL );
    }
  
*/  
    return FALSE;
}

void gtk_juno_switch_toggle(GtkJunoSwitch *sw)
{
    if (sw->type == GTK_JUNO_SWITCH_2WAY)
    {
	sw->pos = 1 - sw->pos;
    }
    else
    {
	switch(sw->pos)
	{
	case GTK_JUNO_SWITCH_POS_UP:
	    sw->pos = GTK_JUNO_SWITCH_POS_MID;
	    break;

	case GTK_JUNO_SWITCH_POS_MID:
	    sw->pos = GTK_JUNO_SWITCH_POS_DOWN;
	    break;
	    
	case GTK_JUNO_SWITCH_POS_DOWN:
	    sw->pos = GTK_JUNO_SWITCH_POS_UP;
	    break;
	}
    }
    
    gtk_juno_switch_expose(GTK_WIDGET(sw), NULL );
    gtk_signal_emit(GTK_OBJECT(sw), switch_signals[0]);
}

void gtk_juno_switch_toggle_dir(GtkJunoSwitch *sw, int dir)
{
    if (sw->type == GTK_JUNO_SWITCH_2WAY)
    {
	sw->pos = 1 - sw->pos;
    }
    else
    {
	switch(sw->pos)
	{
	case GTK_JUNO_SWITCH_POS_UP:
	case GTK_JUNO_SWITCH_POS_DOWN:
	    sw->pos = GTK_JUNO_SWITCH_POS_MID;
	    break;
	    
	case GTK_JUNO_SWITCH_POS_MID:
	    if (dir)
		sw->pos = GTK_JUNO_SWITCH_POS_DOWN;
	    else
		sw->pos = GTK_JUNO_SWITCH_POS_UP;
	    
	}
    }
    
    gtk_juno_switch_expose(GTK_WIDGET(sw), NULL );
    gtk_signal_emit(GTK_OBJECT(sw), switch_signals[0]);
}

void gtk_juno_switch_set_pos(GtkJunoSwitch *sw, GtkJunoSwitchPos pos)
{
    sw->pos = pos;
    gtk_juno_switch_expose(GTK_WIDGET(sw), NULL );
    gtk_signal_emit(GTK_OBJECT(sw), switch_signals[0]);
}

GtkJunoSwitchPos gtk_juno_switch_get_pos(GtkJunoSwitch *sw)
{
    return sw->pos;
}

GtkJunoSwitchType gtk_juno_switch_get_switch_type(GtkJunoSwitch *sw)
{
    return sw->type;
}

