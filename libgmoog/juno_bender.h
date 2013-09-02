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
 * $Revision: 1.4.4.1 $$Date: 2000/02/24 00:43:59 $
 */
#ifndef JUNO_BENDER_H
#define JUNO_BENDER_H

#include <gdk/gdk.h>
#include <gtk/gtkadjustment.h>
#include <gtk/gtkwidget.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
    
#define GTK_JUNO_BENDER(obj) GTK_CHECK_CAST (obj, gtk_juno_bender_get_type (), GtkJunoBender)
#define GTK_JUNO_BENDER_CLASS(klass) GTK_CHECK_CLASS_CAST (klass, gtk_juno_bender_get_type (), GtkJunoBenderClass)
#define GTK_IS_JUNO_BENDER(obj) GTK_CHECK_TYPE (obj, gtk_juno_bender_get_type ())

typedef struct _GtkJunoBender      GtkJunoBender;
typedef struct _GtkJunoBenderClass GtkJunoBenderClass;

struct _GtkJunoBender
{
    GtkWidget widget;
    double value;
    gboolean in_drag;
};

struct _GtkJunoBenderClass
{
    GtkWidgetClass parent_class;
    void (*value_changed) (GtkJunoBender *bender);
};

GtkWidget*     gtk_juno_bender_new();
guint          gtk_juno_bender_get_type(void);
double         gtk_juno_bender_get_value(GtkJunoBender *bender);
void           gtk_juno_bender_set_value(GtkJunoBender *bender, double);


#define JUNO_BENDER_DEFAULT_WIDTH  60
#define JUNO_BENDER_DEFAULT_HEIGHT 8

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* JUNO_BENDER_H */













