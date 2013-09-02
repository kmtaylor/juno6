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
 * $Revision: 1.9.4.2 $$Date: 2000/02/25 20:40:43 $
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtkmain.h>
#include <gtk/gtksignal.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include "juno_keyboard.h"

#define NOTE_UNUSED 32768

static GtkWidgetClass *parent_class = NULL;

static int top_key_offsets[] = {13, 27, 40, 54, 66, 79, 93, 103, 117, 128, 142, 154};
static int bottom_key_offsets[] = {22, 22, 44, 44, 66, 88, 88, 110, 110, 132, 132, 154};

enum {
    KEY_PRESSED,
    KEY_RELEASED,
    LAST_SIGNAL
};

static guint keyboard_signals[LAST_SIGNAL] = {0};

static void
gtk_juno_keyboard_init( GtkJunoKeyboard *juno_keyboard );

static void
gtk_juno_keyboard_class_init( GtkJunoKeyboardClass *class );

static void
gtk_juno_keyboard_destroy( GtkObject *object );

static void
gtk_juno_keyboard_realize( GtkWidget *widget );

static gint
gtk_juno_keyboard_expose( GtkWidget      *widget,
			GdkEventExpose *event );

static gint
gtk_juno_keyboard_button_press( GtkWidget      *widget,
			      GdkEventButton *event );

static gint
gtk_juno_keyboard_button_release( GtkWidget      *widget,
				GdkEventButton *event );

static void 
gtk_juno_keyboard_size_request( GtkWidget      *widget,
			      GtkRequisition *requisition );

static void 
gtk_juno_keyboard_size_allocate( GtkWidget      *widget,
			       GtkAllocation *allocation );

void
gtk_juno_keyboard_set_update_policy( GtkJunoKeyboard *juno_keyboard,
				   GtkUpdateType  policy );


static gint
gtk_juno_keyboard_key_event(GtkWidget *widget,
			    GdkEventKey *event,
			    int pressed);

static gint
gtk_juno_keyboard_key_press(GtkWidget *widget,
			    GdkEventKey *event);

static gint
gtk_juno_keyboard_key_release(GtkWidget *widget,
			      GdkEventKey *event);



void draw_bottom( GtkJunoKeyboard* juno_keyboard, int, int );
void draw_left( GtkJunoKeyboard* juno_keyboard, int, int );
void draw_middle1( GtkJunoKeyboard* juno_keyboard, int, int );
void draw_middle2( GtkJunoKeyboard* juno_keyboard, int, int );
void draw_middle3( GtkJunoKeyboard* juno_keyboard, int, int );
void draw_right( GtkJunoKeyboard* juno_keyboard, int, int );
void draw_last( GtkJunoKeyboard* juno_keyboard, int, int );
void draw_black( GtkJunoKeyboard* juno_keyboard, int, int );


GtkWidget*
gtk_juno_keyboard_new(guint num_voices)
{
    GtkJunoKeyboard *juno_keyboard = gtk_type_new( gtk_juno_keyboard_get_type() );
    gint i;
    
    juno_keyboard->num_voices = num_voices;
    //FIXME: instead of malloc, use gtk memory management
    juno_keyboard->voices = (gint *)malloc(sizeof(gint) * num_voices);

    for (i = 0; i < juno_keyboard->num_voices; i++)
	juno_keyboard->voices[i] = NOTE_UNUSED;

    juno_keyboard->last_voice = -1;
    juno_keyboard->octave = 1;

    return GTK_WIDGET( juno_keyboard );
}

static void
gtk_juno_keyboard_destroy( GtkObject *object )
{
    //GtkJunoKeyboard *juno_keyboard;
    
    g_return_if_fail( object != NULL );
    g_return_if_fail( GTK_IS_JUNO_KEYBOARD( object ) );
    
    //juno_keyboard = GTK_JUNO_KEYBOARD( object );
        
    if ( GTK_OBJECT_CLASS (parent_class)->destroy )
	(* GTK_OBJECT_CLASS (parent_class)->destroy) (object);
}

guint
gtk_juno_keyboard_get_type()
{
    static guint juno_keyboard_type = 0;
    
    if (!juno_keyboard_type)
    {
	GtkTypeInfo juno_keyboard_info =
	{
	    "GtkJuno_Keyboard",
	    sizeof(GtkJunoKeyboard),
	    sizeof(GtkJunoKeyboardClass),
	    (GtkClassInitFunc)gtk_juno_keyboard_class_init,
	    (GtkObjectInitFunc)gtk_juno_keyboard_init,
	    (GtkArgSetFunc)NULL,
	    (GtkArgGetFunc)NULL,
	};
	
	juno_keyboard_type = gtk_type_unique( gtk_widget_get_type(), 
					    &juno_keyboard_info );
    }
    
    return(juno_keyboard_type);
}

static void
gtk_juno_keyboard_class_init( GtkJunoKeyboardClass *class )
{
    GtkObjectClass *object_class;
    GtkWidgetClass *widget_class;
    
    object_class = (GtkObjectClass*) class;
    widget_class = (GtkWidgetClass*) class;
    
    parent_class = gtk_type_class (gtk_widget_get_type ());
    
    object_class->destroy = gtk_juno_keyboard_destroy;
    
    widget_class->realize = gtk_juno_keyboard_realize;
    widget_class->expose_event = gtk_juno_keyboard_expose;
    widget_class->size_request = gtk_juno_keyboard_size_request;
    widget_class->size_allocate = gtk_juno_keyboard_size_allocate;
    widget_class->button_press_event = gtk_juno_keyboard_button_press;
    widget_class->button_release_event = gtk_juno_keyboard_button_release;
    widget_class->key_press_event = gtk_juno_keyboard_key_press;
    widget_class->key_release_event = gtk_juno_keyboard_key_release;

    keyboard_signals[KEY_PRESSED] = 
	gtk_signal_new ("key_pressed",
			GTK_RUN_FIRST,
			object_class->type,
			GTK_SIGNAL_OFFSET(GtkJunoKeyboardClass, key_pressed),
			gtk_marshal_NONE__INT_INT,
			GTK_TYPE_NONE,
			2,
			GTK_TYPE_UINT,
			GTK_TYPE_UINT);

    keyboard_signals[KEY_RELEASED] = 
	gtk_signal_new ("key_released",
			GTK_RUN_FIRST,
			object_class->type,
			GTK_SIGNAL_OFFSET(GtkJunoKeyboardClass, key_released),
			gtk_marshal_NONE__INT,
			GTK_TYPE_NONE, 
			1,
			GTK_TYPE_UINT);

    gtk_object_class_add_signals(object_class, keyboard_signals, LAST_SIGNAL);
}

static void
gtk_juno_keyboard_init( GtkJunoKeyboard *juno_keyboard )
{
    juno_keyboard->button = 0;
    juno_keyboard->policy = GTK_UPDATE_CONTINUOUS;
    memset( juno_keyboard->key_states, 0, sizeof( int ) * 61 );

}

void
gtk_juno_keyboard_set_update_policy(GtkJunoKeyboard *juno_keyboard,
				  GtkUpdateType  policy)
{
    g_return_if_fail (juno_keyboard != NULL);
    g_return_if_fail (GTK_IS_JUNO_KEYBOARD (juno_keyboard));
    
    juno_keyboard->policy = policy;
}

static void
gtk_juno_keyboard_realize (GtkWidget *widget)
{
    //GtkJunoKeyboard *juno_keyboard;
    GdkWindowAttr attributes;
    gint attributes_mask;
    
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_JUNO_KEYBOARD (widget));
    
    GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);
    GTK_WIDGET_SET_FLAGS (widget, GTK_CAN_FOCUS);
    //juno_keyboard = GTK_JUNO_KEYBOARD (widget);
    
    attributes.x = widget->allocation.x;
    attributes.y = widget->allocation.y;
    attributes.width = widget->allocation.width;
    attributes.height = widget->allocation.height;
    attributes.wclass = GDK_INPUT_OUTPUT;
    attributes.window_type = GDK_WINDOW_CHILD;
    attributes.event_mask = gtk_widget_get_events (widget) | 
	GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK | 
	GDK_BUTTON_RELEASE_MASK | GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK;
    attributes.visual = gtk_widget_get_visual (widget);
    attributes.colormap = gtk_widget_get_colormap (widget);
    
    attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
    widget->window = gdk_window_new (widget->parent->window, &attributes, attributes_mask);
    
    widget->style = gtk_style_attach (widget->style, widget->window);
    
    gdk_window_set_user_data (widget->window, widget);
    
    gtk_style_set_background (widget->style, widget->window, GTK_STATE_ACTIVE);
}

static void 
gtk_juno_keyboard_size_request( GtkWidget      *widget,
			      GtkRequisition *requisition )
{
    requisition->width  = JUNO_KEYBOARD_DEFAULT_WIDTH;
    requisition->height = JUNO_KEYBOARD_DEFAULT_HEIGHT;
}

static void
gtk_juno_keyboard_size_allocate( GtkWidget     *widget,
			       GtkAllocation *allocation )
{
    GtkJunoKeyboard *juno_keyboard;
    
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_JUNO_KEYBOARD (widget));
    g_return_if_fail (allocation != NULL);
    
    widget->allocation = *allocation;
    if (GTK_WIDGET_REALIZED (widget))
    {
	GdkColormap* map;
	GdkColor c0 = {0,0x4c4c,0x4c4c,0x4c4c};
	GdkColor c1 = {0,0x8080,0x8080,0x8080};
	GdkColor c2 = {0,0xb3b3,0xb3b3,0xb3b3};
	GdkColor c3 = {0,0xdddd,0xdddd,0xdddd};
	GdkColor c4 = {0,0xffff,0xffff,0xffff};

	juno_keyboard = GTK_JUNO_KEYBOARD (widget);
	
	gdk_window_move_resize (widget->window,
				allocation->x, allocation->y,
				allocation->width, allocation->height);
	
	map = gdk_colormap_new( gtk_widget_get_visual( widget ), 
				TRUE );

	juno_keyboard->verydark = gdk_gc_new( widget->window );
	gdk_colormap_alloc_color( map, &c0, TRUE, TRUE );
	gdk_gc_set_foreground( juno_keyboard->verydark, &c0 );

	juno_keyboard->dark = gdk_gc_new( widget->window );
	gdk_colormap_alloc_color( map, &c1, TRUE, TRUE );
	gdk_gc_set_foreground( juno_keyboard->dark, &c1 );

	juno_keyboard->mid = gdk_gc_new( widget->window );
	gdk_colormap_alloc_color( map, &c2, TRUE, TRUE );
	gdk_gc_set_foreground( juno_keyboard->mid, &c2 );

	juno_keyboard->light = gdk_gc_new( widget->window );
	gdk_colormap_alloc_color( map, &c3, TRUE, TRUE );
	gdk_gc_set_foreground( juno_keyboard->light, &c3 );

	juno_keyboard->white = gdk_gc_new( widget->window );
	gdk_colormap_alloc_color( map, &c4, TRUE, TRUE );
	gdk_gc_set_foreground( juno_keyboard->white, &c4 );
    }
}
 
static gint
gtk_juno_keyboard_expose( GtkWidget      *widget,
			  GdkEventExpose *event )
{
    int i;
    GtkJunoKeyboard* juno_keyboard;

    g_return_val_if_fail( widget != NULL, FALSE );
    juno_keyboard = GTK_JUNO_KEYBOARD( widget );

    if (widget == NULL || widget->window == NULL)
	return FALSE;

    for( i = 0; i < 5; i++ )
    {
	int key = i * 12;
	int x = i * 154;
	draw_left  ( juno_keyboard, key     , x       );
	draw_black ( juno_keyboard, key +  1, x + 13  );
	draw_middle1( juno_keyboard, key +  2, x + 22  );
	draw_black ( juno_keyboard, key +  3, x + 40  );
	draw_right ( juno_keyboard, key +  4, x + 44  );
	draw_left  ( juno_keyboard, key +  5, x + 66  );
	draw_black ( juno_keyboard, key +  6, x + 79  );
	draw_middle2( juno_keyboard, key +  7, x + 88 );
	draw_black ( juno_keyboard, key +  8, x + 103 );
	draw_middle3( juno_keyboard, key +  9, x + 110 );
	draw_black ( juno_keyboard, key + 10, x + 128 );
	draw_right ( juno_keyboard, key + 11, x + 132 );
    }

    draw_last( juno_keyboard, 60, 769 );

    gdk_flush();
    return FALSE;  
}

void draw_bottom( GtkJunoKeyboard* juno_keyboard, int x, int pressed )
{
    GtkWidget* widget = GTK_WIDGET( juno_keyboard );

    if ( pressed == 1 )
    {
	gdk_draw_line( widget->window,
		       juno_keyboard->light,
		       x + 3, 105, x + 19, 105 );

	gdk_draw_line( widget->window,
		       juno_keyboard->mid,
		       x + 3, 106, x + 19, 106 );
	gdk_draw_line( widget->window,
		       juno_keyboard->mid,
		       x + 2, 107, x + 20, 107 );
    }
    else
    {
	gdk_draw_line( widget->window,
		       juno_keyboard->mid,
		       x + 3, 105, x + 19, 105 );
	gdk_draw_line( widget->window,
		       juno_keyboard->mid,
		       x + 2, 106, x + 20, 106 );
	gdk_draw_line( widget->window,
		       widget->style->black_gc,
		       x, 107, x + 21, 107 );
    }
}

void
draw_left( GtkJunoKeyboard* juno_keyboard, int key, int x )
{
    GtkWidget* widget = GTK_WIDGET( juno_keyboard );
    int pressed = juno_keyboard->key_states[ key ];

    // background
    gdk_draw_rectangle( widget->window,
			juno_keyboard->light,
			TRUE, x + 4, 0, 7, 57 );

    gdk_draw_rectangle( widget->window,
			juno_keyboard->light,
			TRUE, x + 4, 57, 16, 48 );

    // left edge
    gdk_draw_line( widget->window,
		   widget->style->black_gc,
		   x, 0, x, 107 );

    gdk_draw_point( widget->window,
		    widget->style->black_gc,
		    x + 1, 106 );

    gdk_draw_line( widget->window,
		   pressed ? widget->style->black_gc : juno_keyboard->dark,
		   x + 1, 0, x + 1, 105 );

    // hilight
    gdk_draw_line( widget->window,
		   pressed ? juno_keyboard->dark : juno_keyboard->white,
		   x + 2, 0, x + 2, 106 );
    
    gdk_draw_line( widget->window,
		   juno_keyboard->white,
		   x + 3, 0, x + 3, 105 );

    draw_bottom( juno_keyboard, x, pressed );

    // right edge
    gdk_draw_rectangle( widget->window,
			juno_keyboard->dark,
			TRUE, x + 11, 0, 2, 57 );
    
    gdk_draw_line( widget->window,
		   juno_keyboard->dark,
		   x + 11, 56, x + 20, 56 );
    
    gdk_draw_rectangle( widget->window,
			juno_keyboard->dark,
			TRUE, x + 20, 56, 2, 50 );

    if ( pressed )
    {
	gdk_draw_rectangle( widget->window,
			    juno_keyboard->dark,
			    TRUE, x + 11, 57, 2, 2 );

	gdk_draw_rectangle( widget->window,
		       widget->style->black_gc,
		       TRUE, x + 13, 56, 9, 3 );

//	gdk_draw_line( widget->window,
//		       widget->style->black_gc,
//		       x + 21, 56, x + 21, 106 );

    }

    gdk_draw_point( widget->window,
		    widget->style->black_gc,
		    x + 21, 106 );
}

void
draw_middle1( GtkJunoKeyboard* juno_keyboard, int key, int x )
{
    GtkWidget* widget = GTK_WIDGET( juno_keyboard );
    int pressed = juno_keyboard->key_states[ key ];
    // background
    gdk_draw_rectangle( widget->window,
		        juno_keyboard->light,
			TRUE, x + 7, 0, 9, 57 );

    gdk_draw_rectangle( widget->window,
			juno_keyboard->light,
			TRUE, x + 4, 57, 16, 48 );

    // left side
    gdk_draw_line( widget->window,
		   pressed ? widget->style->black_gc : juno_keyboard->dark,
		   x + 5, 0, x + 5, 56 );

    gdk_draw_line( widget->window,
		   juno_keyboard->dark,
		   x + 1, 56, x + 4, 56 );

    gdk_draw_line( widget->window,
		   pressed ? widget->style->black_gc : juno_keyboard->dark,
		   x + 1, 56, x + 1, 106 );

    gdk_draw_line( widget->window,
		   widget->style->black_gc,
		   x, 56, x, 106 );

    gdk_draw_point( widget->window,
		    widget->style->black_gc,
		    x + 1, 106 );

    // hilight
    gdk_draw_line( widget->window,
		   juno_keyboard->white,
		   x + 6, 0, x + 6, 56 );

    gdk_draw_line( widget->window,
		   pressed ? juno_keyboard->dark : juno_keyboard->white,
		   x + 2, 57, x + 2, 106 );
    
    gdk_draw_line( widget->window,
		   juno_keyboard->white,
		   x + 3, 57, x + 3, 105 );

    if ( pressed == 1 )
    {
	gdk_draw_rectangle( widget->window,
			    widget->style->black_gc,
			    TRUE, x + 2, 56, 4, 3 );


	gdk_draw_line( widget->window,
		       juno_keyboard->white,
		       x + 6, 56, x + 6, 58 );
    }

    draw_bottom( juno_keyboard, x, pressed );

    // right side
    gdk_draw_rectangle( widget->window,
			juno_keyboard->dark,
			TRUE, x + 16, 0, 2, 57 );

    gdk_draw_line( widget->window,
		   juno_keyboard->dark,
		   x + 18, 56, x + 19, 56 );

    gdk_draw_rectangle( widget->window,
			juno_keyboard->dark,
			TRUE, x + 20, 56, 2, 50 );

    gdk_draw_point( widget->window,
		    widget->style->black_gc,
		    x + 21, 106 );

    if ( pressed == 1 )
    {
	gdk_draw_rectangle( widget->window,
			    juno_keyboard->dark,
			    TRUE, x + 16, 56, 2, 3 );

	gdk_draw_rectangle( widget->window,
			    widget->style->black_gc,
			    TRUE, x + 18, 56, 4, 3 );
    }
}

void
draw_middle2( GtkJunoKeyboard* juno_keyboard, int key, int x )
{
    GtkWidget* widget = GTK_WIDGET( juno_keyboard );
    int pressed = juno_keyboard->key_states[ key ];
    // background
    gdk_draw_rectangle( widget->window,
			juno_keyboard->light,
			TRUE, x + 7, 0, 6, 57 );

    gdk_draw_rectangle( widget->window,
			juno_keyboard->light,
			TRUE, x + 4, 57, 16, 48 );


    // left side
    gdk_draw_line( widget->window,
		    juno_keyboard->dark,
		    x + 5, 0, x + 5, 56 );

    gdk_draw_line( widget->window,
		    juno_keyboard->dark,
		    x + 1, 56, x + 4, 56 );

    gdk_draw_line( widget->window,
		    pressed ? widget->style->black_gc : juno_keyboard->dark,
		    x + 1, 56, x + 1, 106 );

    gdk_draw_line( widget->window,
		   widget->style->black_gc,
		   x, 56, x, 106 );

    gdk_draw_point( widget->window,
		    widget->style->black_gc,
		    x + 1, 106 );

    // hilight
    gdk_draw_line( widget->window,
		   juno_keyboard->white,
		   x + 6, 0, x + 6, 56 );

    gdk_draw_line( widget->window,
		   pressed ? juno_keyboard->dark : juno_keyboard->white,
		   x + 2, 57, x + 2, 106 );
    
    gdk_draw_line( widget->window,
		   juno_keyboard->white,
		   x + 3, 57, x + 3, 105 );

    if ( pressed == 1 )
    {
	gdk_draw_rectangle( widget->window,
			    widget->style->black_gc,
			    TRUE, x + 1, 56, 4, 3 );

	gdk_draw_line( widget->window,
		       juno_keyboard->dark,
		       x + 5, 56, x + 5, 58 );

	gdk_draw_line( widget->window,
		       juno_keyboard->white,
		       x + 6, 56, x + 6, 58 );
    }

    draw_bottom( juno_keyboard, x, pressed );

    // right side
    gdk_draw_point( widget->window,
		    widget->style->black_gc,
		    x + 21, 106 );

    gdk_draw_rectangle( widget->window,
			juno_keyboard->dark,
			TRUE, x + 13, 0, 2, 57 );

    gdk_draw_line( widget->window,
		   juno_keyboard->dark,
		   x + 15, 56, x + 19, 56 );

    gdk_draw_rectangle( widget->window,
			juno_keyboard->dark,
			TRUE, x + 20, 56, 2, 50 );

    if ( pressed == 1 )
    {
	gdk_draw_rectangle( widget->window,
			    juno_keyboard->dark,
			    TRUE, x + 13, 56, 2, 3 );

	gdk_draw_rectangle( widget->window,
			    widget->style->black_gc,
			    TRUE, x + 15, 56, 7, 3 );
    }
}

void
draw_middle3( GtkJunoKeyboard* juno_keyboard, int key, int x )
{
    GtkWidget* widget = GTK_WIDGET( juno_keyboard );
    int pressed = juno_keyboard->key_states[ key ];
    // background
    gdk_draw_rectangle( widget->window,
			juno_keyboard->light,
			TRUE, x + 9, 0, 7, 57 );

    gdk_draw_rectangle( widget->window,
			juno_keyboard->light,
			TRUE, x + 4, 57, 16, 48 );

    // left side
    gdk_draw_line( widget->window,
		    juno_keyboard->dark,
		    x + 7, 0, x + 7, 56 );

    gdk_draw_line( widget->window,
		    juno_keyboard->dark,
		    x + 1, 56, x + 6, 56 );

    gdk_draw_line( widget->window,
		    pressed ? widget->style->black_gc : juno_keyboard->dark,
		    x + 1, 56, x + 1, 106 );

    gdk_draw_line( widget->window,
		   widget->style->black_gc,
		   x, 56, x, 106 );

    gdk_draw_point( widget->window,
		    widget->style->black_gc,
		    x + 1, 106 );

    // hilight
    gdk_draw_line( widget->window,
		   juno_keyboard->white,
		   x + 8, 0, x + 8, 56 );

    gdk_draw_line( widget->window,
		   pressed ? juno_keyboard->dark : juno_keyboard->white,
		   x + 2, 57, x + 2, 106 );
    
    gdk_draw_line( widget->window,
		   juno_keyboard->white,
		   x + 3, 57, x + 3, 105 );

    if ( pressed == 1 )
    {
	gdk_draw_rectangle( widget->window,
			    widget->style->black_gc,
			    TRUE, x + 1, 56, 6, 3 );

	gdk_draw_line( widget->window,
		       juno_keyboard->dark,
		       x + 7, 56, x + 7, 58 );

	gdk_draw_line( widget->window,
		       juno_keyboard->white,
		       x + 8, 56, x + 8, 58 );
    }

    draw_bottom( juno_keyboard, x, pressed );

    // right side
    gdk_draw_point( widget->window,
		    widget->style->black_gc,
		    x + 21, 106 );

    gdk_draw_rectangle( widget->window,
			juno_keyboard->dark,
			TRUE, x + 16, 0, 2, 57 );

    gdk_draw_line( widget->window,
		   juno_keyboard->dark,
		   x + 18, 56, x + 19, 56 );

    gdk_draw_rectangle( widget->window,
			juno_keyboard->dark,
			TRUE, x + 20, 56, 2, 50 );

    if ( pressed == 1 )
    {
	gdk_draw_rectangle( widget->window,
			    juno_keyboard->dark,
			    TRUE, x + 16, 56, 2, 3 );

	gdk_draw_rectangle( widget->window,
			    widget->style->black_gc,
			    TRUE, x + 18, 56, 4, 3 );
    }
}

void
draw_right( GtkJunoKeyboard* juno_keyboard, int key, int x )
{
    GtkWidget* widget = GTK_WIDGET( juno_keyboard );
    int pressed = juno_keyboard->key_states[ key ];
    // background
    gdk_draw_rectangle( widget->window,
			juno_keyboard->light,
			TRUE, x + 12, 0, 8, 57 );

    gdk_draw_rectangle( widget->window,
			juno_keyboard->light,
			TRUE, x + 4, 57, 16, 48 );

    // left side
    gdk_draw_line( widget->window,
		   widget->style->black_gc,
		   x, 56, x, 107 );

    gdk_draw_point( widget->window,
		   widget->style->black_gc,
		   x + 1, 106 );

    gdk_draw_line( widget->window,
		    juno_keyboard->dark,
		    x + 10, 0, x + 10, 56 );

    gdk_draw_line( widget->window,
		    juno_keyboard->dark,
		    x + 1, 56, x + 9, 56 );

    gdk_draw_line( widget->window,
		    pressed ? widget->style->black_gc : juno_keyboard->dark,
		    x + 1, 56, x + 1, 106 );

    // hilight
    gdk_draw_line( widget->window,
		   juno_keyboard->white,
		   x + 11, 0, x + 11, 56 );

    gdk_draw_line( widget->window,
		   pressed ? juno_keyboard->dark : juno_keyboard->white,
		   x + 2, 57, x + 2, 106 );
    
    gdk_draw_line( widget->window,
		   juno_keyboard->white,
		   x + 3, 57, x + 3, 105 );

    if ( pressed == 1 )
    {
	gdk_draw_line( widget->window, juno_keyboard->dark,
		       x + 10, 56, x + 10, 58);

	gdk_draw_line( widget->window, juno_keyboard->white,
		       x + 11, 56, x + 11, 58);

	gdk_draw_rectangle( widget->window, widget->style->black_gc,
			    TRUE, x + 1, 56, 9, 3 );
    }

    draw_bottom( juno_keyboard, x, pressed );

    // right side
    gdk_draw_rectangle( widget->window,
			juno_keyboard->dark,
			TRUE, x + 20, 0, 2, 106 );

    gdk_draw_point( widget->window,
		   widget->style->black_gc,
		   x + 21, 106 );
}

void
draw_black( GtkJunoKeyboard* juno_keyboard, int key, int x )
{
    GtkWidget* widget = GTK_WIDGET( juno_keyboard );
    int pressed = juno_keyboard->key_states[ key ];
    int y = 0;
    if ( pressed )
    {
	y = 2;
    }

    gdk_draw_rectangle( widget->window,
			widget->style->black_gc,
			TRUE, x, 0, 14, 56 );

    gdk_draw_line( widget->window,
		   juno_keyboard->dark,
		   x + 1, 0, x + 1, 50 );

    gdk_draw_line( widget->window,
		   juno_keyboard->dark,
		   x + 1, 52, x + 1, 54 );


    gdk_draw_line( widget->window,
		   juno_keyboard->light,
		   x + 2, 49 + y, x + 2, 50 + y );

    gdk_draw_line( widget->window,
		   juno_keyboard->light,
		   x + 3, 51 + y, x + 4, 51 + y );
}

void
draw_last( GtkJunoKeyboard* juno_keyboard, int key, int x )
{
    GtkWidget* widget = GTK_WIDGET( juno_keyboard );
    int pressed = juno_keyboard->key_states[ key ];

    // background
    gdk_draw_rectangle( widget->window,
			juno_keyboard->light,
			TRUE, x + 4, 0, 16, 107 );

    // left edge
    gdk_draw_line( widget->window,
		   widget->style->black_gc,
		   x, 0, x, 107 );

    gdk_draw_point( widget->window,
		    widget->style->black_gc,
		    x + 1, 106 );

    gdk_draw_line( widget->window,
		   pressed ? widget->style->black_gc : juno_keyboard->dark,
		   x + 1, 0, x + 1, 105 );

    // hilight
    gdk_draw_line( widget->window,
		   pressed ? juno_keyboard->dark : juno_keyboard->white,
		   x + 2, 0, x + 2, 106 );
    
    gdk_draw_line( widget->window,
		   juno_keyboard->white,
		   x + 3, 0, x + 3, 105 );

    // bottom   
    if ( pressed == 1 )
    {
	gdk_draw_line( widget->window,
		       juno_keyboard->mid,
		       x + 3, 106, x + 19, 106 );
	gdk_draw_line( widget->window,
		       juno_keyboard->mid,
		       x + 2, 107, x + 20, 107 );
    }
    else
    {
	gdk_draw_line( widget->window,
		       juno_keyboard->mid,
		       x + 3, 105, x + 19, 105 );
	gdk_draw_line( widget->window,
		       juno_keyboard->mid,
		       x + 2, 106, x + 20, 106 );
	
	gdk_draw_line( widget->window,
		       widget->style->black_gc,
		       x, 107, x + 21, 107 );
    }

    // right side
    gdk_draw_rectangle( widget->window,
			juno_keyboard->dark,
			TRUE, x + 20, 0, 2, 106 );

    gdk_draw_line( widget->window,
		   widget->style->black_gc,
		   x + 22, 0, x + 22, 107 );

    gdk_draw_point( widget->window,
		    widget->style->black_gc,
		    x + 21, 106 );
}

static gint
gtk_juno_keyboard_button_press( GtkWidget      *widget,
			      GdkEventButton *event )
{
    int i;
    int key = -1;
    int octave;
    int offset;
    int *key_pos;
    GtkJunoKeyboard *juno_keyboard; 

    juno_keyboard = GTK_JUNO_KEYBOARD( widget );

    octave = event->x / 154;
    offset = (int)event->x % 154;
    
    if (event->y <= 56)
	key_pos = top_key_offsets;
    else
	key_pos = bottom_key_offsets;

    for (i = 0; i < 12; i++)
    {
	if (offset < key_pos[i])
	{
	    key = octave * 12 + i;
	    break;
	}
    }

    if (!juno_keyboard->key_states[key])
	gtk_juno_keyboard_note_on(juno_keyboard, key);
    else
	gtk_juno_keyboard_note_off(juno_keyboard, key);



    return TRUE;
}

static gint
gtk_juno_keyboard_button_release (GtkWidget      *widget,
			 GdkEventButton *event)
{
    GtkJunoKeyboard *juno_keyboard;
    
    g_return_val_if_fail (widget != NULL, FALSE);
    g_return_val_if_fail (GTK_IS_JUNO_KEYBOARD (widget), FALSE);
    g_return_val_if_fail (event != NULL, FALSE);
    
    juno_keyboard = GTK_JUNO_KEYBOARD (widget);
    
    if( event->button == 1 )
    {
        juno_keyboard->lastx = (gint)event->x;
        juno_keyboard->lasty = (gint)event->y;
	
	gtk_juno_keyboard_expose( widget, NULL );
    }

    if (juno_keyboard->button == event->button)
    {
	gtk_grab_remove (widget);
	
	juno_keyboard->button = 0;
	
//	if (juno_keyboard->policy == GTK_UPDATE_DELAYED)
//	    gtk_timeout_remove (juno_keyboard->timer);
    }
    
    return TRUE;
}

void gtk_juno_keyboard_note_on( GtkJunoKeyboard* juno_keyboard, int note )
{
    gint start;
    gint i;

    if (note >= 0 && note < 61)
    {
	if (juno_keyboard->key_states[note] == 1)
	    return;

	juno_keyboard->key_states[note] = 1;
	gtk_juno_keyboard_expose( GTK_WIDGET( juno_keyboard ), NULL );
    }

    i = start = (juno_keyboard->last_voice + 1) % juno_keyboard->num_voices;

    do
    {
	if (juno_keyboard->voices[i] == NOTE_UNUSED)
	{
	    juno_keyboard->voices[i] = note;
	    gtk_signal_emit(GTK_OBJECT(juno_keyboard), 
			    keyboard_signals[KEY_PRESSED], 
			    i, 
			    note);
	    juno_keyboard->last_voice = i;
	    break;
	}

	i = (i + 1) % juno_keyboard->num_voices;
    } while (i != start);
}

void gtk_juno_keyboard_note_off( GtkJunoKeyboard* juno_keyboard, int note )
{
    gint i = 0;

    if (note >= 0 && note < 61)
    {
	juno_keyboard->key_states[note] = 0;
	gtk_juno_keyboard_expose( GTK_WIDGET( juno_keyboard ), NULL );
    }

    for (i = 0; i < juno_keyboard->num_voices; i++)
    {
	if (juno_keyboard->voices[i] == note)
	{
	    gtk_signal_emit(GTK_OBJECT(juno_keyboard), 
			    keyboard_signals[KEY_RELEASED], 
			    i);	    
	    juno_keyboard->voices[i] = NOTE_UNUSED;

	    //FIXME: in a perfect world, we would break out of the loop here
	    //having found the voice playing the note, but sometimes
	    //we screw up...
	    //break;
	}
    }
}

static gint gtk_juno_keyboard_key_event(GtkWidget *widget, GdkEventKey *event, int pressed)
{
    GtkJunoKeyboard *jk;
    int note = NOTE_UNUSED;

    jk = GTK_JUNO_KEYBOARD(widget);

    switch(event->keyval)
    {
    case GDK_Q:
    case GDK_q:
	note = -3;
	break;
    case GDK_2:
    case GDK_at:
	note = -2;
	break;
    case GDK_W:
    case GDK_w:
	note = -1;
	break;


    case GDK_E:
    case GDK_e:
	note = 0;
	break;
    case GDK_4:
    case GDK_dollar:
	note = 1;
	break;
    case GDK_R:
    case GDK_r:
	note = 2;
	break;
    case GDK_5:
    case GDK_percent:
	note = 3;
	break;
    case GDK_T:
    case GDK_t:
	note = 4;
	break;
    case GDK_Y:
    case GDK_y:
	note = 5;
	break;
    case GDK_7:
    case GDK_ampersand:
	note = 6;
	break;
    case GDK_U:
    case GDK_u:
	note = 7;
	break;
    case GDK_8:
    case GDK_asterisk:
	note = 8;
	break;
    case GDK_I:
    case GDK_i:
	note = 9;
	break;
    case GDK_9:
    case GDK_parenleft:
	note = 10;
	break;
    case GDK_O:
    case GDK_o:
	note = 11;
	break;
    case GDK_P:
    case GDK_p:
        note = 12;
	break;
    case GDK_underscore:
    case GDK_minus:
	note = 13;
	break;
    case GDK_braceleft:
    case GDK_bracketleft:
	note = 14;
	break;
    case GDK_plus:
    case GDK_equal:
	note = 15;
	break;
    case GDK_braceright:
    case GDK_bracketright:
	note = 16;
	break;
    case GDK_bar:
    case GDK_backslash:
	note = 17;
	break;
	
    case GDK_A:
    case GDK_a:
	jk->octave = 0;
	break;
    case GDK_S:
    case GDK_s:
	jk->octave = 1;
	break;
    case GDK_D:
    case GDK_d:
	jk->octave = 2;
	break;
    case GDK_F:
    case GDK_f:
	jk->octave = 3;
	break;
    case GDK_G:
    case GDK_g:
	jk->octave = 4;
	break;
    }

    if (note != NOTE_UNUSED)
    {
	if (pressed)
	    gtk_juno_keyboard_note_on(jk, note + 12 * jk->octave);
	else
	    gtk_juno_keyboard_note_off(jk, note + 12 * jk->octave);
    }

    return TRUE;
}

static gint gtk_juno_keyboard_key_press(GtkWidget *widget,GdkEventKey *event)
{
    return gtk_juno_keyboard_key_event(widget, event, TRUE);
}

static gint gtk_juno_keyboard_key_release(GtkWidget *widget,GdkEventKey *event)
{
    return gtk_juno_keyboard_key_event(widget, event, FALSE);
}

