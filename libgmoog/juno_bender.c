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
 * $Revision: 1.10.4.1 $$Date: 2000/02/24 00:43:59 $
 */
#include <math.h>
#include <stdio.h>
#include <gtk/gtkmain.h>
#include <gtk/gtksignal.h>
#include <gdk/gdk.h>

#include "juno_bender.h"
extern char *bender_gradient_pix[];

//#include "juno_bender_pixmaps.h"

static GtkWidgetClass *parent_class = NULL;

static void gtk_juno_bender_init( GtkJunoBender             *juno_bender );

static void gtk_juno_bender_class_init( GtkJunoBenderClass  *class );

static void gtk_juno_bender_destroy( GtkObject            *object );

static void gtk_juno_bender_realize( GtkWidget            *widget );

static gint gtk_juno_bender_expose( GtkWidget             *widget,
				  GdkEventExpose        *event );

static void gtk_juno_bender_size_request( GtkWidget       *widget,
					GtkRequisition  *requisition );

static void gtk_juno_bender_size_allocate( GtkWidget      *widget,
					 GtkAllocation  *allocation );

static gint gtk_juno_bender_motion_notify( GtkWidget      *widget, 
					 GdkEventMotion *event );

static gint gtk_juno_bender_button_press(GtkWidget *widget, 
					 GdkEventButton *event);

static gint gtk_juno_bender_button_release(GtkWidget *widget, 
					 GdkEventButton *event);

enum 
{
    VALUE_CHANGED,
    LAST_SIGNAL
};

static GdkPixmap *bender_gradient = NULL;
static guint bender_signals[LAST_SIGNAL] = {0};

guint
gtk_juno_bender_get_type()
{
    static guint juno_bender_type = 0;
    
    if (!juno_bender_type)
    {
	GtkTypeInfo juno_bender_info =
	{
	    "GtkJuno_Bender",
	    sizeof(GtkJunoBender),
	    sizeof(GtkJunoBenderClass),
	    (GtkClassInitFunc)gtk_juno_bender_class_init,
	    (GtkObjectInitFunc)gtk_juno_bender_init,
	    (GtkArgSetFunc)NULL,
	    (GtkArgGetFunc)NULL,
	};
	
	juno_bender_type = gtk_type_unique( gtk_widget_get_type(), 
					  &juno_bender_info );
    }
    
    return(juno_bender_type);
}

static void
gtk_juno_bender_class_init (GtkJunoBenderClass *class)
{
    GtkObjectClass *object_class;
    GtkWidgetClass *widget_class;
    
    object_class = (GtkObjectClass*) class;
    widget_class = (GtkWidgetClass*) class;
    
    parent_class = gtk_type_class (gtk_widget_get_type ());
    
    object_class->destroy = gtk_juno_bender_destroy;
    
    widget_class->realize = gtk_juno_bender_realize;
    widget_class->expose_event = gtk_juno_bender_expose;
    widget_class->size_request = gtk_juno_bender_size_request;
    widget_class->size_allocate = gtk_juno_bender_size_allocate;
    widget_class->motion_notify_event = gtk_juno_bender_motion_notify;
    widget_class->button_press_event = gtk_juno_bender_button_press;
    widget_class->button_release_event = gtk_juno_bender_button_release;

    bender_signals[VALUE_CHANGED] = gtk_signal_new(
	"value_changed",
	GTK_RUN_FIRST,
	object_class->type, 
	GTK_SIGNAL_OFFSET(GtkJunoBenderClass, value_changed),
	gtk_marshal_NONE__NONE,
	GTK_TYPE_NONE,
	0);

    gtk_object_class_add_signals(object_class, bender_signals, LAST_SIGNAL);
}

static void
gtk_juno_bender_init (GtkJunoBender *juno_bender)
{
    juno_bender->in_drag = FALSE;
}

GtkWidget*
gtk_juno_bender_new()
{
    GtkJunoBender *juno_bender;
    
    juno_bender = gtk_type_new (gtk_juno_bender_get_type ());
    
    return GTK_WIDGET (juno_bender);
}

static void
gtk_juno_bender_destroy (GtkObject *object)
{
    if (GTK_OBJECT_CLASS (parent_class)->destroy)
	(* GTK_OBJECT_CLASS (parent_class)->destroy) (object);
}

static void
gtk_juno_bender_realize (GtkWidget *widget)
{
    //GtkJunoBender *juno_bender;
    GdkWindowAttr attributes;
    gint attributes_mask;
    
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_JUNO_BENDER (widget));
    
    GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);
    //juno_bender = GTK_JUNO_BENDER (widget);
    
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

    /* initialize the pixmap only once */
    if (!bender_gradient)
    {
	bender_gradient = gdk_pixmap_create_from_xpm_d(widget->window,
						       NULL,
						       //FIXME:transparent
						       NULL,
						       bender_gradient_pix);
    }
}

static void 
gtk_juno_bender_size_request( GtkWidget      *widget,
			      GtkRequisition *requisition )
{
    requisition->width  = JUNO_BENDER_DEFAULT_WIDTH;
    requisition->height = JUNO_BENDER_DEFAULT_HEIGHT;
}

static void
gtk_juno_bender_size_allocate( GtkWidget     *widget,
			       GtkAllocation *allocation )
{
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_JUNO_BENDER (widget));
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
gtk_juno_bender_expose( GtkWidget      *widget,
			GdkEventExpose *event )
{
    GtkJunoBender* juno_bender = GTK_JUNO_BENDER( widget );

    GdkColor     gray       = {0,0x4040,0x4040,0x4040};
    GdkColor     background = {0,0x3333,0x3333,0x3333};
    GdkGC*       blackgc    = widget->style->black_gc;
    GdkGC*       whitegc    = widget->style->white_gc;
    GdkGC*       gc;
    GdkColormap* map;
    gint basex1, basex2;
    gint pointx1, pointx2;
    double angle;
    double midpoint;

    if (widget == NULL || widget->window == NULL)
	return FALSE;

    gc = gdk_gc_new( widget->window );
    map = gdk_colormap_new(gtk_widget_get_visual( widget ), TRUE);

    gdk_colormap_alloc_color(map, &gray, TRUE, TRUE);
    gdk_colormap_alloc_color(map, &background, TRUE, TRUE);

    gdk_gc_set_foreground(gc, &background);
    gdk_draw_rectangle( widget->window, 
			gc,
			TRUE, 0, 0, 
			widget->allocation.width, widget->allocation.height );		    

    gdk_draw_rectangle( widget->window, 
			blackgc, TRUE, 4, 0, 
			widget->allocation.width - 8, widget->allocation.height );

    gdk_gc_set_foreground(gc, &gray);
    gdk_draw_pixmap(widget->window,
		    gc,
		    bender_gradient,
		    0,
		    0,
		    5,
		    1,
		    50,
		    6 );

    midpoint = widget->allocation.width / 2.0;
    angle = M_PI/2 - (juno_bender->value * M_PI/4);
    basex1 = cos(angle + M_PI/20) * 24 + midpoint;
    basex2 = cos(angle - M_PI/20) * 24 + midpoint;
    pointx1 = cos(angle + M_PI/50) * 36 + midpoint;
    pointx2 = cos(angle - M_PI/50) * 36 + midpoint;

    if ( basex1 < pointx1 )
	gdk_draw_rectangle(widget->window, gc, TRUE,
			   basex1, 1, pointx2 - basex1 + 2, 6);
    else
	gdk_draw_rectangle(widget->window, gc, TRUE,
			   pointx1, 1, basex2 - pointx1 + 1, 6);

    gdk_draw_rectangle(widget->window, whitegc, TRUE,
		       pointx1, 1, (pointx2 - pointx1) + 1, 6);

    gdk_draw_point(widget->window, gc, pointx1, 1);
    gdk_draw_point(widget->window, gc, pointx1, 6);
    gdk_draw_point(widget->window, gc, pointx2, 1);
    gdk_draw_point(widget->window, gc, pointx2, 6);

    gdk_gc_destroy( gc );
    gdk_flush();
 
    return FALSE;  
}

static gint
gtk_juno_bender_motion_notify (GtkWidget      *widget,
			     GdkEventMotion *event)
{
    GtkJunoBender *juno_bender;
    GdkModifierType state;
    gint x, y;
    
    g_return_val_if_fail (widget != NULL, FALSE);
    g_return_val_if_fail (GTK_IS_JUNO_BENDER (widget), FALSE);
    g_return_val_if_fail (event != NULL, FALSE);
    
    juno_bender = GTK_JUNO_BENDER( widget );
    
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
    
    if (state & GDK_BUTTON1_MASK && juno_bender->in_drag)
    {
	gint midpoint;

	midpoint = GTK_WIDGET(juno_bender)->allocation.width / 2;

	juno_bender->value = (double)(x - midpoint) / midpoint;

	if (juno_bender->value > 1.0)
	    juno_bender->value = 1.0;
	else if (juno_bender->value < -1.0)
	    juno_bender->value = -1.0;

	gtk_juno_bender_expose( widget, NULL );
	gtk_signal_emit(GTK_OBJECT(juno_bender),bender_signals[VALUE_CHANGED]);
    }
    
    return FALSE;
}

double gtk_juno_bender_get_value(GtkJunoBender *jb)
{
    return jb->value;
}

void gtk_juno_bender_set_value(GtkJunoBender *jb, double value)
{
    if (value < -1.0 || value > 1.0)
	return;

    jb->value = value;

    gtk_juno_bender_expose(GTK_WIDGET(jb), NULL);
    gtk_signal_emit(GTK_OBJECT(jb), bender_signals[VALUE_CHANGED]);
}

static gint 
gtk_juno_bender_button_press(GtkWidget *widget, 
			     GdkEventButton *event)
{
    if (event->button == 1)
    {
	GTK_JUNO_BENDER(widget)->in_drag = TRUE;
	gtk_grab_add(widget);
    }

    return TRUE;
}

static gint
gtk_juno_bender_button_release(GtkWidget *widget, 
			       GdkEventButton *event)
{
    GtkJunoBender *jb = GTK_JUNO_BENDER(widget);

    if (event->button == 1 && jb->in_drag)
    {
	gtk_grab_remove(widget);
	gtk_juno_bender_set_value(jb, 0.0);
	jb->in_drag = 0;
    }

    return TRUE;
}

