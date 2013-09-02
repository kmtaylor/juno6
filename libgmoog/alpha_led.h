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
#ifndef ALPHA_LED_H
#define ALPHA_LED_H

#include <gdk/gdk.h>
#include <gtk/gtkadjustment.h>
#include <gtk/gtkwidget.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
    
#define GTK_ALPHA_LED(obj) GTK_CHECK_CAST (obj, gtk_alpha_led_get_type (), GtkAlpha_Led)
#define GTK_ALPHA_LED_CLASS(klass) GTK_CHECK_CLASS_CAST (klass, gtk_alpha_led_get_type (), GtkAlpha_LedClass)
#define GTK_IS_ALPHA_LED(obj) GTK_CHECK_TYPE (obj, gtk_alpha_led_get_type ())

typedef struct GtkAlpha_Led      GtkAlpha_Led;
typedef struct GtkAlpha_LedClass GtkAlpha_LedClass;

struct GtkAlpha_Led
{
    GtkWidget widget;
    int value;
};

struct GtkAlpha_LedClass
{
    GtkWidgetClass parent_class;
    void (* value_changed) (GtkAlpha_Led *alpha_led);
};

GtkWidget*     gtk_alpha_led_new();
guint          gtk_alpha_led_get_type(void);
double         gtk_alpha_led_get_value(GtkAlpha_Led *alpha_led);
void           gtk_alpha_led_set_value(GtkAlpha_Led *alpha_led, int);


#define ALPHA_LED_DEFAULT_WIDTH  13
#define ALPHA_LED_DEFAULT_HEIGHT 19

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ALPHA_LED_H */













