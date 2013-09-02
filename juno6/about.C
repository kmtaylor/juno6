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
 * $Revision: 1.1.4.1 $$Date: 2000/02/24 00:43:47 $
 */
#include <gtk/gtk.h>
#include "version.h"

static GtkWidget *about_window = (GtkWidget*)NULL;
static GtkWidget *about_vbox, *about_notebook;
static GtkWidget *about_credits_vbox;
static GtkWidget *about_credits_text_hbox, *about_credits_text, *about_credits_text_scrollbar;
static GtkWidget *bbox, *close_btn;

static const gchar *credit_text = "\
      BrainSlayer\nJuno6 algorithm basement by Ultramaster\n";

void show_about_window(void)
{
    GtkWidget *label;
    GdkFont *font;
    gchar *text;
    
    if (!about_window)
    {
	about_window = gtk_window_new(GTK_WINDOW_DIALOG);
	gtk_window_set_title(GTK_WINDOW(about_window), "About Juno-666");
	gtk_window_set_policy(GTK_WINDOW(about_window), FALSE, FALSE, FALSE);
	gtk_window_set_position(GTK_WINDOW(about_window), GTK_WIN_POS_MOUSE);
	gtk_container_set_border_width(GTK_CONTAINER(about_window), 10);
	gtk_signal_connect(GTK_OBJECT(about_window), "destroy", GTK_SIGNAL_FUNC(gtk_widget_destroyed), &about_window);
	gtk_widget_realize(about_window);
	
	about_vbox = gtk_vbox_new(FALSE, 10);
	gtk_container_add(GTK_CONTAINER(about_window), about_vbox);
	
	about_notebook = gtk_notebook_new();
	gtk_box_pack_start(GTK_BOX(about_vbox), about_notebook, FALSE, FALSE, 0);
	
	about_credits_vbox = gtk_vbox_new(FALSE, 5);
	gtk_container_set_border_width(GTK_CONTAINER(about_credits_vbox), 10);
			
	text = g_strdup_printf("Juno-666 %s - Virtual Polyphonic Synthesizer", VERSION);
	label = gtk_label_new(text);
	g_free(text);
	
	gtk_box_pack_start(GTK_BOX(about_credits_vbox), label, FALSE, FALSE, 0);
	gtk_widget_show(label);
	
	label = gtk_label_new("CUD");
	gtk_box_pack_start(GTK_BOX(about_credits_vbox), label, FALSE, FALSE, 0);
	gtk_widget_show(label);
	
	about_credits_text_hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(about_credits_vbox), about_credits_text_hbox, FALSE, FALSE, 0);
	
	about_credits_text = gtk_text_new((GtkAdjustment*)NULL, (GtkAdjustment*)NULL);
	gtk_widget_set_usize(about_credits_text, 420, 100);
	font = gdk_font_load("-adobe-courier-medium-r-normal--*-120-*-*-*-*-*-*");
	gtk_text_insert(GTK_TEXT(about_credits_text), font, (GdkColor*)NULL, (GdkColor*)NULL, credit_text, -1);
	gdk_font_unref(font);
	gtk_text_set_editable(GTK_TEXT(about_credits_text), FALSE);
	gtk_box_pack_start(GTK_BOX(about_credits_text_hbox), about_credits_text, TRUE, TRUE, 0);
	gtk_widget_show(about_credits_text);
	
	about_credits_text_scrollbar = gtk_vscrollbar_new(GTK_TEXT(about_credits_text)->vadj);
	gtk_box_pack_start(GTK_BOX(about_credits_text_hbox), about_credits_text_scrollbar, FALSE, FALSE, 0);
	gtk_widget_show(about_credits_text_scrollbar);
        
	gtk_widget_show(about_credits_text_hbox); 
	
	gtk_notebook_append_page(GTK_NOTEBOOK(about_notebook), about_credits_vbox, gtk_label_new("Credits"));
	gtk_widget_show(about_credits_vbox);
	
	gtk_widget_show(about_notebook);
	
	bbox = gtk_hbutton_box_new();
	gtk_button_box_set_layout(GTK_BUTTON_BOX(bbox), GTK_BUTTONBOX_END);
	gtk_button_box_set_spacing(GTK_BUTTON_BOX(bbox), 5);
	gtk_box_pack_start(GTK_BOX(about_vbox), bbox, FALSE, FALSE, 0);
	
	close_btn = gtk_button_new_with_label("Close");
	gtk_signal_connect_object(GTK_OBJECT(close_btn), "clicked", GTK_SIGNAL_FUNC(gtk_widget_destroy), GTK_OBJECT(about_window));                GTK_WIDGET_SET_FLAGS(close_btn, GTK_CAN_DEFAULT);
	gtk_box_pack_start(GTK_BOX(bbox), close_btn, TRUE, TRUE, 0);
	gtk_widget_show(close_btn);
	gtk_widget_grab_default(close_btn);
	
	gtk_widget_show(bbox);
	gtk_widget_show(about_vbox);
	gtk_widget_show(about_window);
    }
}
