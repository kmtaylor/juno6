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
#ifndef JUNO_KNOB_H
#define JUNO_KNOB_H

#include <gdk/gdk.h>
#include <gtk/gtkadjustment.h>
#include <gtk/gtkwidget.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
    
#define GTK_JUNO_KNOB(obj) GTK_CHECK_CAST (obj, gtk_juno_knob_get_type (), GtkJunoKnob)
#define GTK_JUNO_KNOB_CLASS(klass) GTK_CHECK_CLASS_CAST (klass, gtk_juno_knob_get_type (), GtkJunoKnobClass)
#define GTK_IS_JUNO_KNOB(obj) GTK_CHECK_TYPE (obj, gtk_juno_knob_get_type ())

typedef struct _GtkJunoKnob      GtkJunoKnob;
typedef struct _GtkJunoKnobClass GtkJunoKnobClass;

struct _GtkJunoKnob
{
    GtkWidget widget;
    double angle;
    double value;
};

struct _GtkJunoKnobClass
{
    GtkWidgetClass parent_class;
    void (* value_changed) (GtkJunoKnob *knob);
};

GtkWidget*     gtk_juno_knob_new();
guint          gtk_juno_knob_get_type(void);
double         gtk_juno_knob_get_value(GtkJunoKnob *knob);
void           gtk_juno_knob_set_value(GtkJunoKnob *knob, double);


#define JUNO_KNOB_DEFAULT_WIDTH  19
#define JUNO_KNOB_DEFAULT_HEIGHT 19

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* JUNO_KNOB_H */













