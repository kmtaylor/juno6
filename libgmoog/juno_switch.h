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
#ifndef JUNO_SWITCH_H
#define JUNO_SWITCH_H

#include <gdk/gdk.h>
#include <gtk/gtkadjustment.h>
#include <gtk/gtkwidget.h>
#include <gtk/gtksignal.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
    
#define GTK_JUNO_SWITCH(obj) GTK_CHECK_CAST (obj, gtk_juno_switch_get_type (), GtkJunoSwitch)
#define GTK_JUNO_SWITCH_CLASS(klass) GTK_CHECK_CLASS_CAST (klass, gtk_juno_switch_get_type (), GtkJunoSwitchClass)
#define GTK_IS_JUNO_SWITCH(obj) GTK_CHECK_TYPE (obj, gtk_juno_switch_get_type ())

typedef struct _GtkJunoSwitch      GtkJunoSwitch;
typedef struct _GtkJunoSwitchClass GtkJunoSwitchClass;

typedef enum 
{
    GTK_JUNO_SWITCH_2WAY,
    GTK_JUNO_SWITCH_3WAY
} GtkJunoSwitchType;

enum 
{
    JUNO_SWITCH_HILIGHT,
    JUNO_SWITCH_NORMAL,
    JUNO_SWITCH_SHADOW
};

typedef enum
{
    GTK_JUNO_SWITCH_POS_UP,
    GTK_JUNO_SWITCH_POS_DOWN,
    GTK_JUNO_SWITCH_POS_MID,
} GtkJunoSwitchPos;

struct _GtkJunoSwitch
{
    GtkWidget widget;

    GtkJunoSwitchPos pos;
    GtkJunoSwitchType type;
};

struct _GtkJunoSwitchClass
{
    GtkWidgetClass parent_class;

    void (* pressed)  (GtkJunoSwitch *sw);
    void (* released) (GtkJunoSwitch *sw);
    void (* clicked)  (GtkJunoSwitch *sw);
    void (* enter)    (GtkJunoSwitch *sw);
    void (* leave)    (GtkJunoSwitch *sw);
};

GtkWidget*     gtk_juno_switch_new( GtkJunoSwitchType type );
guint          gtk_juno_switch_get_type(void);
void           gtk_juno_switch_set_update_policy(GtkJunoSwitch *juno_switch,
						 GtkUpdateType   policy);

void           gtk_juno_switch_toggle(GtkJunoSwitch *sw);
void           gtk_juno_switch_toggle_dir(GtkJunoSwitch *sw, int);
void           gtk_juno_switch_set_pos(GtkJunoSwitch *, GtkJunoSwitchPos);
GtkJunoSwitchPos gtk_juno_switch_get_pos(GtkJunoSwitch *);
GtkJunoSwitchType gtk_juno_switch_get_switch_type(GtkJunoSwitch *);

#define GTK_JUNO_SWITCH_DEFAULT_WIDTH  8
#define GTK_JUNO_SWITCH_DEFAULT_HEIGHT 24

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* JUNO_SWITCH_H */
