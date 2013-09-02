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
#ifndef JUNO_LFOTRIG_H
#define JUNO_LFOTRIG_H

#include <gdk/gdk.h>
#include <gtk/gtkadjustment.h>
#include <gtk/gtkwidget.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
    
#define GTK_JUNO_LFOTRIG(obj) GTK_CHECK_CAST (obj, gtk_juno_lfotrig_get_type (), GtkJunoLfotrig)
#define GTK_JUNO_LFOTRIG_CLASS(klass) GTK_CHECK_CLASS_CAST (klass, gtk_juno_lfotrig_get_type (), GtkJunoLfotrigClass)
#define GTK_IS_JUNO_LFOTRIG(obj) GTK_CHECK_TYPE (obj, gtk_juno_lfotrig_get_type ())

typedef struct _GtkJunoLfotrig      GtkJunoLfotrig;
typedef struct _GtkJunoLfotrigClass GtkJunoLfotrigClass;

struct _GtkJunoLfotrig
{
    GtkWidget widget;
    int value;
    gint button;
};

struct _GtkJunoLfotrigClass
{
    GtkWidgetClass parent_class;
    void (* value_changed) (GtkJunoLfotrig *lfotrig);
};

GtkWidget*     gtk_juno_lfotrig_new();
guint          gtk_juno_lfotrig_get_type(void);
int            gtk_juno_lfotrig_get_value(GtkJunoLfotrig *);
void           gtk_juno_lfotrig_set_value(GtkJunoLfotrig *, int);


#define JUNO_LFOTRIG_DEFAULT_WIDTH  41
#define JUNO_LFOTRIG_DEFAULT_HEIGHT 19

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* JUNO_LFOTRIG_H */













