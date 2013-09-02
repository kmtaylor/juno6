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
 * $Revision: 1.2.4.1 $$Date: 2000/02/24 00:43:59 $
 */
#ifndef JUNO_LED_H
#define JUNO_LED_H

#include <gdk/gdk.h>
#include <gtk/gtkwidget.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
    
#define GTK_JUNO_LED(obj) GTK_CHECK_CAST (obj, gtk_juno_led_get_type (), GtkJunoLed)
#define GTK_JUNO_LED_CLASS(klass) GTK_CHECK_CLASS_CAST (klass, gtk_juno_led_get_type (), GtkJunoLedClass)
#define GTK_IS_JUNO_LED(obj) GTK_CHECK_TYPE (obj, gtk_juno_led_get_type ())

typedef struct _GtkJunoLed      GtkJunoLed;
typedef struct _GtkJunoLedClass GtkJunoLedClass;

struct _GtkJunoLed
{
    GtkWidget widget;

    gint state;
};

struct _GtkJunoLedClass
{
    GtkWidgetClass parent_class;
};

GtkWidget*     gtk_juno_led_new( void );
guint          gtk_juno_led_get_type( void );
void           gtk_juno_led_set_state( GtkJunoLed *juno_led,
				       gint state );
gint           gtk_juno_led_get_state( GtkJunoLed *juno_led );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* JUNO_LED_H */
