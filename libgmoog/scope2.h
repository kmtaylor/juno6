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
 * $Revision: 1.7.4.2 $$Date: 2000/02/25 20:40:43 $
 */
#ifndef GTK_SCOPE_H
#define GTK_SCOPE_H

#include <gdk/gdk.h>
#include <gtk/gtkadjustment.h>
#include <gtk/gtkwidget.h>
#include <gtk/gtksignal.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
    
#define GTK_SCOPE(obj) GTK_CHECK_CAST (obj, gtk_scope_get_type (), GtkScope)

#define GTK_SCOPE_CLASS(klass) GTK_CHECK_CLASS_CAST (klass, gtk_scope_get_type (), GtkScopeClass)

#define GTK_IS_SCOPE(obj) GTK_CHECK_TYPE (obj, gtk_scope_get_type ())

typedef struct _GtkScope      GtkScope;
typedef struct _GtkScopeClass GtkScopeClass;

struct _GtkScope
{
    GtkWidget widget;
    
    /* Button currently pressed or 0 if none */
    guint8 button;

    double* points;
    int pos;
    int width;
    int height;
    int height_2;

    int resetCounter;
    int wavelength;

    GdkGC* darkGreen;
    GdkFont* fixed;
};

struct _GtkScopeClass
{
    GtkWidgetClass parent_class;

    void (* pressed)  (GtkScope *scope);
    void (* released) (GtkScope *scope);
    void (* clicked)  (GtkScope *scope);
    void (* enter)    (GtkScope *scope);
    void (* leave)    (GtkScope *scope);
};

GtkWidget*     gtk_scope_new();
guint          gtk_scope_get_type(void);
GtkAdjustment* gtk_scope_get_adjustment(GtkScope *scope);
void           gtk_scope_set_update_policy(GtkScope *scope,
						 GtkUpdateType   policy);
int            gtk_scope_plot( GtkScope* scope, int reset, double point );
void           gtk_scope_draw( GtkScope* scope );

void           gtk_scope_plot_buff( GtkScope* scope, double* data, size_t n );

#define SCOPE_DEFAULT_WIDTH  940
#define SCOPE_DEFAULT_HEIGHT 200

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* GTK_SCOPE_H */













