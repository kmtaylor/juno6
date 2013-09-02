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
#ifndef JUNO_KEYBOARD_H
#define JUNO_KEYBOARD_H

#include <gdk/gdk.h>
#include <gtk/gtkwidget.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
    
#define GTK_JUNO_KEYBOARD(obj) GTK_CHECK_CAST (obj, gtk_juno_keyboard_get_type (), GtkJunoKeyboard)
#define GTK_JUNO_KEYBOARD_CLASS(klass) GTK_CHECK_CLASS_CAST (klass, gtk_juno_keyboard_get_type (), GtkJunoKeyboardClass)
#define GTK_IS_JUNO_KEYBOARD(obj) GTK_CHECK_TYPE (obj, gtk_juno_keyboard_get_type ())

typedef struct _GtkJunoKeyboard      GtkJunoKeyboard;
typedef struct _GtkJunoKeyboardClass GtkJunoKeyboardClass;

struct _GtkJunoKeyboard
{
    GtkWidget widget;

    /* update policy (GTK_UPDATE_[CONTINUOUS/DELAYED/DISCONTINUOUS]) */
    guint policy : 2;

    /* Button currently pressed or 0 if none */
    guint8 button;

    gint lastx;
    gint lasty;

    int key_states[ 61 ];
    guint num_voices;
    gint *voices;
    gint last_voice;
    gint octave;

    GdkGC* verydark;
    GdkGC* dark;
    GdkGC* mid;
    GdkGC* light;
    GdkGC* white;
};

struct _GtkJunoKeyboardClass
{
    GtkWidgetClass parent_class;
    void (*key_pressed)(GtkJunoKeyboard*);
    void (*key_released)(GtkJunoKeyboard*);
};

GtkWidget*     gtk_juno_keyboard_new(guint num_voices);
guint          gtk_juno_keyboard_get_type(void);
GtkAdjustment* gtk_juno_keyboard_get_adjustment(GtkJunoKeyboard *juno_keyboard);
void           gtk_juno_keyboard_set_update_policy(GtkJunoKeyboard *juno_keyboard,
						 GtkUpdateType   policy);
void           gtk_juno_keyboard_set_adjustment(GtkJunoKeyboard *juno_keyboard,
					      GtkAdjustment *adjustment);

void gtk_juno_keyboard_note_on( GtkJunoKeyboard*, int );
void gtk_juno_keyboard_note_off( GtkJunoKeyboard*, int );

#define JUNO_KEYBOARD_DEFAULT_WIDTH  792
#define JUNO_KEYBOARD_DEFAULT_HEIGHT 108

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* JUNO_KEYBOARD_H */













