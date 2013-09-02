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
 * $Revision: 1.1.4.1 $$Date: 2000/02/24 00:43:59 $
 */
#ifndef JUNO_BUTTON_H
#define JUNO_BUTTON_H

#include <gdk/gdk.h>
#include <gtk/gtkadjustment.h>
#include <gtk/gtkwidget.h>
#include <gtk/gtksignal.h>
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
    
#define GTK_JUNO_BUTTON(obj) GTK_CHECK_CAST (obj, gtk_juno_button_get_type (), GtkJunoButton)
#define GTK_JUNO_BUTTON_CLASS(klass) GTK_CHECK_CLASS_CAST (klass, gtk_juno_button_get_type (), GtkJunoButtonClass)
#define GTK_IS_JUNO_BUTTON(obj) GTK_CHECK_TYPE (obj, gtk_juno_button_get_type ())

typedef struct _GtkJunoButton      GtkJunoButton;
typedef struct _GtkJunoButtonClass GtkJunoButtonClass;

typedef enum 
{
    GTK_JUNO_BUTTON_CREAM,
    GTK_JUNO_BUTTON_YELLOW,
    GTK_JUNO_BUTTON_ORANGE
} GtkJunoButtonType;

enum 
{
    JUNO_BUTTON_HILIGHT,
    JUNO_BUTTON_NORMAL,
    JUNO_BUTTON_SHADOW
};

struct _GtkJunoButton
{
    GtkWidget widget;

    /* Button currently pressed or 0 if none */
    guint8 button;

    GtkJunoButtonType type;
};

struct _GtkJunoButtonClass
{
    GtkWidgetClass parent_class;

    void (* pressed)  (GtkJunoButton *button);
    void (* released) (GtkJunoButton *button);
    void (* clicked)  (GtkJunoButton *button);
    void (* enter)    (GtkJunoButton *button);
    void (* leave)    (GtkJunoButton *button);
};

GtkWidget*     gtk_juno_button_new( GtkJunoButtonType type );
guint          gtk_juno_button_get_type(void);
GtkAdjustment* gtk_juno_button_get_adjustment(GtkJunoButton *juno_button);
void           gtk_juno_button_set_update_policy(GtkJunoButton *juno_button,
						 GtkUpdateType   policy);
void           gtk_juno_button_set_adjustment(GtkJunoButton *juno_button,
					      GtkAdjustment *adjustment);

void gtk_juno_button_do_press( GtkJunoButton* b );
void gtk_juno_button_do_release( GtkJunoButton* b );

#define JUNO_BUTTON_DEFAULT_WIDTH  17
#define JUNO_BUTTON_DEFAULT_HEIGHT 19

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* JUNO_BUTTON_H */













