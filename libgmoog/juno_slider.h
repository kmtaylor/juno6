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
#ifndef JUNO_SLIDER_H
#define JUNO_SLIDER_H

#include <gdk/gdk.h>
#include <gtk/gtkadjustment.h>
#include <gtk/gtkwidget.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
    
#define GTK_JUNO_SLIDER(obj) GTK_CHECK_CAST (obj, gtk_juno_slider_get_type (), GtkJunoSlider)
#define GTK_JUNO_SLIDER_CLASS(klass) GTK_CHECK_CLASS_CAST (klass, gtk_juno_slider_get_type (), GtkJunoSliderClass)
#define GTK_IS_JUNO_SLIDER(obj) GTK_CHECK_TYPE (obj, gtk_juno_slider_get_type ())

typedef struct _GtkJunoSlider      GtkJunoSlider;
typedef struct _GtkJunoSliderClass GtkJunoSliderClass;

struct _GtkJunoSlider
{
    GtkWidget widget;

    /* update policy (GTK_UPDATE_[CONTINUOUS/DELAYED/DISCONTINUOUS]) */
    guint policy : 2;

    /* Button currently pressed or 0 if none */
    guint8 button;

    gint lastx;
    gint lasty;

    /* Old values from adjustment stored so we know when something changes */
    gfloat old_value;
    gfloat old_lower;
    gfloat old_upper;

    GtkAdjustment *adjustment;
};

struct _GtkJunoSliderClass
{
    GtkWidgetClass parent_class;
};

GtkWidget*     gtk_juno_slider_new(GtkAdjustment *adjustment);
guint          gtk_juno_slider_get_type(void);
GtkAdjustment* gtk_juno_slider_get_adjustment(GtkJunoSlider *juno_slider);
void           gtk_juno_slider_set_update_policy(GtkJunoSlider *juno_slider,
						 GtkUpdateType   policy);
void           gtk_juno_slider_set_adjustment(GtkJunoSlider *juno_slider,
					      GtkAdjustment *adjustment);

#define JUNO_SLIDER_DEFAULT_WIDTH  13
#define JUNO_SLIDER_DEFAULT_HEIGHT 49

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* JUNO_SLIDER_H */













