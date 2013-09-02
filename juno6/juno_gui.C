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
 * $Revision: 1.50.4.1 $$Date: 2000/02/24 00:43:48 $
 */

#include <stdio.h>
#include <gtk/gtk.h>
#ifndef POSIX
#include <moog/MidiInputWin32.h>
#else
#include <moog/MidiInput.h>
#endif
#include <gmoog/juno_widgets.h>
extern char *juno_background_data[];
#define juno_background_mask_width 940
#define juno_background_mask_height 224
extern char juno_background_mask_bits[]; 
//#include "juno_background.xpm"
//#include "juno_background_mask.xbm"

#include "juno_gui.h"
#include "juno_wrappers.h"
#include "juno_patch.h"
#include "about.h"
#include <gmoog/Scope.h>

extern String patchFileName;


void init();
void initPerformanceSection();
void initTransposeHoldSection();
void initArpeggioSection();
void initLfoSection();
void initDcoSection();
void initHpfSection();
void initVcfSection();
void initVcaSection();
void initEnvSection();
void initChorusSection();
void initPatchSection();
void layout( GtkFixed* f );
void layoutPerformanceSection( GtkFixed* f );
void layoutTransposeHoldSection( GtkFixed* f );
void layoutArpeggioSection( GtkFixed* f );
void layoutLfoSection( GtkFixed* f );
void layoutDcoSection( GtkFixed* f );
void layoutHpfSection( GtkFixed* f );
void layoutVcfSection( GtkFixed* f );
void layoutVcaSection( GtkFixed* f );
void layoutEnvSection( GtkFixed* f );
void layoutChorusSection( GtkFixed* f );
void layoutPatchSection( GtkFixed* f );

//FIXME: these should either be global or not... decide
Settings* settings;
JunoControl* junoControl;
MidiInput* midiInput;
JunoKeyboard* keyboard;

GtkWidget* mainWindow;
GtkItemFactory* mainMenu;

int offset_x, offset_y, indrag;
extern Scope* scope;

void mainWindow_press( GtkWidget* widget, 
		       GdkEventButton* event, gpointer callback_data )
{
    if ( event->button == 1 )
    {
	gdk_window_raise(widget->window );
	gdk_pointer_grab(widget->window, FALSE, (GdkEventMask)
	    ( GDK_BUTTON_MOTION_MASK | 
	    GDK_BUTTON_RELEASE_MASK ), GDK_NONE, GDK_NONE, GDK_CURRENT_TIME);
	offset_x = (int)event->x;
	offset_y = (int)event->y;
	indrag=1;
    }

    else if ( event->button == 3 )
    {
	int x,y;
	gdk_window_get_pointer( NULL, &x, &y, NULL );

	gtk_item_factory_popup( GTK_ITEM_FACTORY( mainMenu ), 
				x, 
				y, 
				3, 
				GDK_CURRENT_TIME );
    }
}

void mainWindow_release( GtkWidget* widget, 
			 GdkEventButton* event, 
			 gpointer callback_data )
{
    if ( widget != mainWindow )
	return;

    if ( event->button == 1 )
    {
	indrag=0;
	gdk_pointer_ungrab(GDK_CURRENT_TIME);
    }
}

void mainWindow_motion( GtkWidget* widget, 
			GdkEventMotion* event, gpointer callback_data )
{
    if ( indrag )
    {
	gint x, y;
	gdk_window_get_pointer( NULL, &x, &y, NULL );
	gtk_widget_set_uposition( widget, x - offset_x, y - offset_y ); 
	gdk_flush();
    }
}

void mainWindow_enter( GtkWidget* widget, 
		       GdkEventCrossing *event, 
		       gpointer data)
{
    if (event->mode == 0 && event->state == 0)
    {
	gdk_key_repeat_disable();
	//printf("key repeat is off\n");
	gtk_widget_grab_focus(keyboard->widget);
    }
}

void mainWindow_leave( GtkWidget* widget, 
		       GdkEventCrossing *event, 
		       gpointer data)
{
    if (event->mode == 0 && event->state == 0)
    {
	gdk_key_repeat_restore();
	//printf("key repeat is on\n");
    }
}

enum menu_actions
{
    ABOUT_DIALOG,
    TOGGLE_SCOPE,
    EXIT
};

void mainwin_general_menu_callback(gpointer cb_data, guint action, GtkWidget * w)
{
    switch( action )
    {
    case ABOUT_DIALOG:
	show_about_window();
	break;
    case TOGGLE_SCOPE:
	scope->toggle();
	break;
    case EXIT:
	gtk_main_quit();
	break;
    default:
	printf( "Unhandled menu action %d\n", action );
     }
}

#define mainwin_general_menu_entry_count 4
GtkItemFactoryEntry mainwin_general_menu_entries[] =
{
        {"/About", NULL, (GtkItemFactoryCallback)mainwin_general_menu_callback, ABOUT_DIALOG, "<Item>"},
        {"/Scope Visible", NULL, (GtkItemFactoryCallback)mainwin_general_menu_callback, TOGGLE_SCOPE, "<ToggleItem>"},
        {"/-", NULL, NULL, 0, "<Separator>"},
        {"/Exit", NULL, (GtkItemFactoryCallback)mainwin_general_menu_callback, EXIT, "<Item>"}
};

void initGui( JunoControl* _junoControl, 
	      Settings* _settings, 
	      MidiInput* _midiInput, 
	      int numVoices)
{
    junoControl = _junoControl;
    settings    = _settings;
    midiInput   = _midiInput;

    keyboard = new JunoKeyboard(numVoices);
    
    mainMenu = gtk_item_factory_new(GTK_TYPE_MENU, "<Main>", NULL);
    gtk_item_factory_create_items(mainMenu, mainwin_general_menu_entry_count,
				  mainwin_general_menu_entries, NULL);
    GtkStyle* menuStyle = gtk_style_new(); // a default style
    gtk_widget_set_style( mainMenu->widget, menuStyle );

    GtkWidget* mainContainer;

    mainWindow = gtk_window_new( GTK_WINDOW_TOPLEVEL );

    gtk_widget_add_events( GTK_WIDGET( mainWindow ), 
			   GDK_BUTTON_RELEASE_MASK |
			   GDK_POINTER_MOTION_MASK | 
			   GDK_POINTER_MOTION_HINT_MASK |
			   GDK_ENTER_NOTIFY_MASK | 
			   GDK_LEAVE_NOTIFY_MASK | 
			   GDK_KEY_PRESS_MASK | 
			   GDK_KEY_RELEASE_MASK	);

    gtk_window_set_default_size( GTK_WINDOW( mainWindow ), 940, 224 );
    gtk_window_set_policy(GTK_WINDOW(mainWindow), FALSE, FALSE, TRUE);

    gtk_widget_realize( mainWindow );
    gdk_window_set_decorations( mainWindow->window, (GdkWMDecoration)0 );
    gtk_widget_set_app_paintable(mainWindow, TRUE);

    gtk_signal_connect(GTK_OBJECT(mainWindow), "button_press_event", 
		       GTK_SIGNAL_FUNC(mainWindow_press), NULL);
    gtk_signal_connect(GTK_OBJECT(mainWindow), "button_release_event", 
		       GTK_SIGNAL_FUNC(mainWindow_release), NULL);
    gtk_signal_connect(GTK_OBJECT(mainWindow), "motion_notify_event", 
		       GTK_SIGNAL_FUNC(mainWindow_motion), NULL);
    gtk_signal_connect(GTK_OBJECT(mainWindow), "enter_notify_event",
		       GTK_SIGNAL_FUNC(mainWindow_enter), NULL);
    gtk_signal_connect(GTK_OBJECT(mainWindow), "leave_notify_event",
		       GTK_SIGNAL_FUNC(mainWindow_leave), NULL);

    mainContainer = gtk_fixed_new();

    gtk_widget_show( mainWindow );
    gtk_widget_show( mainContainer );

    GdkPixmap* image = gdk_pixmap_create_from_xpm_d( mainWindow->window, 
						     NULL, 
						     NULL, 
						     juno_background_data );
    GtkWidget* pixmap = gtk_pixmap_new( image, NULL );
    gtk_widget_show( pixmap );
    gtk_fixed_put( GTK_FIXED( mainContainer ), pixmap, 0, 0 );

    gtk_container_add( GTK_CONTAINER( mainWindow ), mainContainer );

    init();
    layout( GTK_FIXED( mainContainer ) );

    GdkBitmap* mask = gdk_bitmap_create_from_data( mainWindow->window, 
						   juno_background_mask_bits,
						   juno_background_mask_width,
						   juno_background_mask_height );

    gtk_widget_shape_combine_mask( mainWindow, mask, 0, 0 );
}

GtkJunoPowerswitch* powerswitch;
GtkWidget*          powerled;
JunoKnob*           tuneKnob;

struct _performanceSection
{
    JunoBender* pitchBender;
    JunoSlider* dcoSlider;
    JunoSlider* vcfSlider;
    JunoLfoTrigger* lfoTrig;
    JunoSwitch* octaveTransposeSwitch;
} performanceSection;

struct _transposeHoldSection
{
    JunoButton* transposeButton;
    JunoButton* holdButton;
} transposeHoldSection;

struct _arpeggioSection
{
    JunoSlider* rateSlider;
    JunoButton* onOffButton;
    JunoSwitch* modeSwitch;
    JunoSwitch* rangeSwitch;
} arpeggioSection;

struct _lfoSection
{
    JunoSlider* rateSlider;
    JunoSlider* delaySlider;
    JunoSwitch* modeSwitch;
} lfoSection;

struct _dcoSection
{
    JunoSlider* lfoSlider;
    JunoSlider* pwmSlider;
    JunoSlider* subSlider;
    JunoSlider* noiseSlider;
    JunoButton* pulseButton;
    JunoButton* sawButton;
    JunoButton* subButton;
    JunoSwitch* pwmSwitch;
} dcoSection;

struct _hpfSection
{
    JunoSlider* frqSlider;
} hpfSection;

struct _vcfSection
{
    JunoSlider* frqSlider;
    JunoSlider* resSlider;
    JunoSlider* envSlider;
    JunoSlider* lfoSlider;
    JunoSlider* kbdSlider;
    JunoSwitch* invertSwitch;
} vcfSection;

struct _vcaSection
{
    JunoSwitch* envSwitch;
    JunoSlider* volumeSlider;
} vcaSection;

struct _envSection
{
    JunoSlider* attackSlider;
    JunoSlider* decaySlider;
    JunoSlider* sustainSlider;
    JunoSlider* releaseSlider;
} envSection;

struct _chorusSection
{
    JunoButton* offButton;
    JunoButton* oneButton;
    JunoButton* twoButton;
} chorusSection;

struct _patchSection
{
    JunoAlphaLed* bankLed;
    JunoAlphaLed* patchLed;
    JunoButton* patchButtons[10];    
    JunoButton* saveButton;    
    JunoButton* loadButton;
    GtkWidget* usedLed;
} patchSection;

void savePatch( juno_patch* patch )
{
    patch->used = 1;

    patch->bender_dco = performanceSection.dcoSlider->getValue();
    patch->bender_vcf = performanceSection.vcfSlider->getValue();
//    patch->lfo_trigger = performanceSection.lfoTrigger->getValue();
    patch->volume = vcaSection.volumeSlider->getValue();
    patch->octave_transpose = performanceSection.octaveTransposeSwitch->getValue();
    
    patch->arpeggio_switch = arpeggioSection.onOffButton->getValue();
    patch->arpeggio_mode = arpeggioSection.modeSwitch->getValue();
    patch->arpeggio_range = arpeggioSection.rangeSwitch->getValue();
    patch->arpeggio_rate = arpeggioSection.rateSlider->getValue();
    
    patch->lfo_rate = lfoSection.rateSlider->getValue();
    patch->lfo_delay = lfoSection.delaySlider->getValue();
    patch->lfo_mode = lfoSection.modeSwitch->getValue();
    
    patch->dco_lfo = dcoSection.lfoSlider->getValue();
    patch->dco_pwm = dcoSection.pwmSlider->getValue();
    patch->dco_pwm_mod = dcoSection.pwmSwitch->getValue();
    patch->dco_pulse_switch = dcoSection.pulseButton->getValue();
    patch->dco_saw_switch = dcoSection.sawButton->getValue();
    patch->dco_sub_switch = dcoSection.subButton->getValue();
    patch->dco_sub = dcoSection.subSlider->getValue();
    patch->dco_noise = dcoSection.noiseSlider->getValue();
    
    patch->hpf_frq = hpfSection.frqSlider->getValue();
    
    patch->vcf_frq = vcfSection.frqSlider->getValue();
    patch->vcf_res = vcfSection.resSlider->getValue();
    patch->vcf_env_invert = vcfSection.invertSwitch->getValue();
    patch->vcf_env = vcfSection.envSlider->getValue();
    patch->vcf_lfo = vcfSection.lfoSlider->getValue();
    patch->vcf_kbd = vcfSection.kbdSlider->getValue();
    
    patch->vca_mode = vcaSection.envSwitch->getValue();
    
    patch->env_attack = envSection.attackSlider->getValue();
    patch->env_decay = envSection.decaySlider->getValue();
    patch->env_sustain = envSection.releaseSlider->getValue();
    patch->env_release = envSection.sustainSlider->getValue();
    
    patch->chorus_I_switch = chorusSection.oneButton->getValue();
    patch->chorus_II_switch = chorusSection.twoButton->getValue();

    save_patches( patchFileName, patches );

    gtk_juno_led_set_state( GTK_JUNO_LED(patchSection.usedLed), 1);
}

void loadPatch( juno_patch* patch )
{
    performanceSection.dcoSlider->setValue( patch->bender_dco );
    performanceSection.vcfSlider->setValue( patch->bender_vcf );
//    performanceSection.lfoTrigger->setValue( patch->lfo_trigger );
    vcaSection.volumeSlider->setValue( patch->volume );
    performanceSection.octaveTransposeSwitch->setValue( patch->octave_transpose );

    arpeggioSection.onOffButton->setValue( patch->arpeggio_switch );
    arpeggioSection.modeSwitch->setValue( patch->arpeggio_mode );
    arpeggioSection.rangeSwitch->setValue( patch->arpeggio_range );
    arpeggioSection.rateSlider->setValue( patch->arpeggio_rate );

    lfoSection.rateSlider->setValue( patch->lfo_rate );
    lfoSection.delaySlider->setValue( patch->lfo_delay );
    lfoSection.modeSwitch->setValue( patch->lfo_mode );

    dcoSection.lfoSlider->setValue( patch->dco_lfo );
    dcoSection.pwmSlider->setValue( patch->dco_pwm );
    dcoSection.pwmSwitch->setValue( patch->dco_pwm_mod );
    dcoSection.pulseButton->setValue( patch->dco_pulse_switch );
    dcoSection.sawButton->setValue( patch->dco_saw_switch );
    dcoSection.subButton->setValue( patch->dco_sub_switch );
    dcoSection.subSlider->setValue( patch->dco_sub );
    dcoSection.noiseSlider->setValue( patch->dco_noise );

    hpfSection.frqSlider->setValue( patch->hpf_frq );

    vcfSection.frqSlider->setValue( patch->vcf_frq );
    vcfSection.resSlider->setValue( patch->vcf_res );
    vcfSection.invertSwitch->setValue( patch->vcf_env_invert );
    vcfSection.envSlider->setValue( patch->vcf_env );
    vcfSection.lfoSlider->setValue( patch->vcf_lfo );
    vcfSection.kbdSlider->setValue( patch->vcf_kbd );

    vcaSection.envSwitch->setValue( patch->vca_mode );

    envSection.attackSlider->setValue( patch->env_attack );
    envSection.decaySlider->setValue( patch->env_decay );
    envSection.releaseSlider->setValue( patch->env_sustain );
    envSection.sustainSlider->setValue( patch->env_release );

    chorusSection.oneButton->setValue( patch->chorus_I_switch );
    chorusSection.twoButton->setValue( patch->chorus_II_switch );
    int bank=-1;
    int pat=-1;
    for (int i=0;i<NUM_PATCHES;i++)
	{
	if (&patches[i]==patch)
	   {
	   bank = i/10;
	   pat = i%10;
	   }
	}
    if (pat!=-1)
	{
        patchSection.bankLed->setValue((double)bank);
        patchSection.patchLed->setValue((double)pat);
	}
    gtk_juno_led_set_state( GTK_JUNO_LED(patchSection.usedLed),
			    patches[ patchSection.bankLed->getValue() * 10 + patchSection.patchLed->getValue() ].used );
}

void loadCurrentPatch()
{
    loadPatch( &patches[ patchSection.bankLed->getValue() * 10 +
				patchSection.patchLed->getValue() ] );
}

void saveCurrentPatch()
{
    savePatch( &patches[ patchSection.bankLed->getValue() * 10 +
		       patchSection.patchLed->getValue() ] );
}

static void powerSwitched(GtkWidget *widget, gpointer data)
{
    int value = gtk_juno_powerswitch_get_value( powerswitch );
    gtk_juno_led_set_state( GTK_JUNO_LED( powerled ), value );

    if ( value ) // power on
    {
	Scheduler::resume();
    }

    else // power off
    {
	//exit(0);
	Scheduler::suspend();
    }
}

//FIXME move this to juno_wrappers and deal with all these hacks...(KARL)
static gint buttonPressed( GtkWidget      *widget, 
			   gpointer        data )
{
    if ( data == chorusSection.offButton )
    {
	chorusSection.oneButton->set( FALSE );
	chorusSection.twoButton->set( FALSE );
    }

    else if ( data == patchSection.saveButton )
    {
	saveCurrentPatch();
    }

    else if ( data == patchSection.loadButton )
    {
	loadCurrentPatch();
    }

    else
    {
	bool found = false;
	for( int i = 0; i < 10; i++ )
	{
	    if ( data == patchSection.patchButtons[i] )
	    {
		found = true;
		patchSection.patchLed->setValue( i );
		loadCurrentPatch();
		break;
	    }
	}

	if (!found)
	    ((JunoButton*)data)->buttonPressed();
    }

    return( TRUE );
}

static gint keyTransposeReleased(GtkWidget *widget, gpointer data)
{
    ((JunoButton*)data)->set(0);
    if (keyboard->getKeyTranspose() != 0)
	    ((JunoButton*)data)->setLed(1);
    
    return TRUE;
}

static void patchChanged( GtkWidget      *widget, 
			  gpointer        data )
{
    gtk_juno_led_set_state( GTK_JUNO_LED(patchSection.usedLed),
			    patches[ patchSection.bankLed->getValue() * 10 + patchSection.patchLed->getValue() ].used );
}

void init()
{
    powerswitch = GTK_JUNO_POWERSWITCH( gtk_juno_powerswitch_new() );
    gtk_widget_show( GTK_WIDGET( powerswitch ) );
    powerled = gtk_juno_led_new();
    gtk_widget_show( powerled );
    tuneKnob = new JunoKnob("master_tune");
    tuneKnob->setValue( .5 );

    gtk_juno_led_set_state( GTK_JUNO_LED(powerled), 1 );

    gtk_signal_connect( GTK_OBJECT( powerswitch ), 
			"value_changed",
			GTK_SIGNAL_FUNC( powerSwitched ), 
			NULL );
    
    initPerformanceSection();
    initTransposeHoldSection();
    initArpeggioSection();
    initLfoSection();
    initDcoSection();
    initHpfSection();
    initVcfSection();
    initVcaSection();
    initEnvSection();
    initChorusSection();
    initPatchSection();
}

void initPerformanceSection( )
{
    performanceSection.dcoSlider  = new JunoSlider( "bender_dco" );
    performanceSection.vcfSlider  = new JunoSlider( "bender_vcf" );

    performanceSection.octaveTransposeSwitch = new JunoSwitch( GTK_JUNO_SWITCH_3WAY, "octave_transpose");

    performanceSection.lfoTrig = new JunoLfoTrigger( "lfo_trigger" );
    performanceSection.pitchBender = new JunoBender("bender");
}

void initTransposeHoldSection( )
{
    //FIXME: the transpose button should be momentary
    transposeHoldSection.transposeButton = new JunoButton( GTK_JUNO_BUTTON_CREAM, "transpose_switch" );
    transposeHoldSection.holdButton      = new JunoButton( GTK_JUNO_BUTTON_YELLOW, "hold_switch" );

    gtk_signal_connect( GTK_OBJECT( transposeHoldSection.transposeButton->widget ), 
			"pressed",
			GTK_SIGNAL_FUNC( buttonPressed ), 
			(gpointer)transposeHoldSection.transposeButton);

    gtk_signal_connect( GTK_OBJECT( transposeHoldSection.holdButton->widget ), 
			"pressed",
			GTK_SIGNAL_FUNC( buttonPressed ), 
			(gpointer)transposeHoldSection.holdButton);

    gtk_signal_connect( GTK_OBJECT( transposeHoldSection.transposeButton->widget ), 
			"released",
			GTK_SIGNAL_FUNC( keyTransposeReleased ), 
			(gpointer)transposeHoldSection.transposeButton);
}

void initArpeggioSection( )
{
    arpeggioSection.rateSlider  = new JunoSlider( "arpeggio_rate" );
    arpeggioSection.onOffButton = new JunoButton( GTK_JUNO_BUTTON_ORANGE, "arpeggio_switch" );

    arpeggioSection.modeSwitch  = new JunoSwitch( GTK_JUNO_SWITCH_3WAY, "arpeggio_mode");
    arpeggioSection.rangeSwitch = new JunoSwitch( GTK_JUNO_SWITCH_3WAY, "arpeggio_range" );

    gtk_signal_connect( GTK_OBJECT( arpeggioSection.onOffButton->widget ), 
			"pressed",
			GTK_SIGNAL_FUNC( buttonPressed ), 
			arpeggioSection.onOffButton );
}

void initLfoSection( )
{
    lfoSection.rateSlider  = new JunoSlider( "lfo_rate" );
    lfoSection.delaySlider = new JunoSlider( "lfo_delay" );
    lfoSection.modeSwitch  = new JunoSwitch( GTK_JUNO_SWITCH_2WAY, "lfo_mode" );

}

void initDcoSection( )
{
    dcoSection.lfoSlider    = new JunoSlider( "dco_lfo" );
    dcoSection.pwmSlider    = new JunoSlider( "dco_pwm" );

    dcoSection.subSlider    = new JunoSlider( "dco_sub" );

    dcoSection.noiseSlider  = new JunoSlider( "dco_noise" );

    dcoSection.pulseButton  = new JunoButton( GTK_JUNO_BUTTON_CREAM, "dco_pulse_switch" );
    dcoSection.sawButton    = new JunoButton( GTK_JUNO_BUTTON_YELLOW, "dco_saw_switch");
    dcoSection.subButton    = new JunoButton( GTK_JUNO_BUTTON_ORANGE, "dco_sub_switch" );

    dcoSection.pwmSwitch    = new JunoSwitch( GTK_JUNO_SWITCH_3WAY, "dco_pwm_mod" );

    gtk_signal_connect( GTK_OBJECT( dcoSection.pulseButton->widget ),
			"pressed",
			GTK_SIGNAL_FUNC( buttonPressed ), 
			(gpointer)dcoSection.pulseButton);

    gtk_signal_connect( GTK_OBJECT( dcoSection.sawButton->widget ), 
			"pressed",
			GTK_SIGNAL_FUNC( buttonPressed ), 
			(gpointer)dcoSection.sawButton);

    gtk_signal_connect( GTK_OBJECT( dcoSection.subButton->widget ), 
			"pressed",
			GTK_SIGNAL_FUNC( buttonPressed ), 
			(gpointer)dcoSection.subButton);
}

void initHpfSection( )
{
    hpfSection.frqSlider = new JunoSlider( "hpf_frq" );
}

void initVcfSection( )
{
    vcfSection.frqSlider    = new JunoSlider( "vcf_frq" );
    vcfSection.resSlider    = new JunoSlider( "vcf_res" );
    vcfSection.envSlider    = new JunoSlider( "vcf_env" );
    vcfSection.lfoSlider    = new JunoSlider( "vcf_lfo" );
    vcfSection.kbdSlider    = new JunoSlider( "vcf_kbd" );
    vcfSection.invertSwitch = new JunoSwitch( GTK_JUNO_SWITCH_2WAY, "vcf_env_invert");
}

void initVcaSection( )
{
    vcaSection.envSwitch = new JunoSwitch( GTK_JUNO_SWITCH_2WAY, "vca_mode" );
    vcaSection.volumeSlider = new JunoSlider( "volume" );
}

void initEnvSection( )
{
    envSection.attackSlider  = new JunoSlider( "env_attack" );
    envSection.decaySlider   = new JunoSlider( "env_decay" );
    envSection.sustainSlider = new JunoSlider( "env_sustain" );
    envSection.releaseSlider = new JunoSlider( "env_release" );
}

void initChorusSection( )
{
    chorusSection.offButton = new JunoButton( GTK_JUNO_BUTTON_CREAM, "chorus_off_switch" );
    chorusSection.oneButton = new JunoButton( GTK_JUNO_BUTTON_YELLOW, "chorus_I_switch" );
    chorusSection.twoButton = new JunoButton( GTK_JUNO_BUTTON_ORANGE, "chorus_II_switch" );

    gtk_signal_connect( GTK_OBJECT( chorusSection.offButton->widget ), 
			"pressed",
			GTK_SIGNAL_FUNC( buttonPressed ), 
			chorusSection.offButton );

    gtk_signal_connect( GTK_OBJECT( chorusSection.oneButton->widget ), 
			"pressed",
			GTK_SIGNAL_FUNC( buttonPressed ), 
			chorusSection.oneButton );

    gtk_signal_connect( GTK_OBJECT( chorusSection.twoButton->widget ), 
			"pressed",
			GTK_SIGNAL_FUNC( buttonPressed ), 
			chorusSection.twoButton );
}

void initPatchSection( )
{
    patchSection.bankLed = new JunoAlphaLed( "bank" );
    patchSection.patchLed = new JunoAlphaLed( "patch" );
    for( int i = 0; i < 10; i++)
    {
	String tmp;
	tmp.sprintf( "patch_%d", i );

	patchSection.patchButtons[i] = new JunoButton( GTK_JUNO_BUTTON_CREAM, 
						       (const char *)tmp, false );
	gtk_signal_connect( GTK_OBJECT( patchSection.patchButtons[i]->widget ),
			    "pressed",
			    GTK_SIGNAL_FUNC( buttonPressed ), 
			    patchSection.patchButtons[i] );
    }

    patchSection.usedLed = gtk_juno_led_new();
    gtk_widget_show( patchSection.usedLed );

    patchSection.saveButton = new JunoButton( GTK_JUNO_BUTTON_YELLOW, "patch_save", false );
    patchSection.loadButton = new JunoButton( GTK_JUNO_BUTTON_ORANGE, "patch_load", false );

    gtk_signal_connect( GTK_OBJECT( patchSection.bankLed->widget ),
			"value_changed",
			GTK_SIGNAL_FUNC( patchChanged ), NULL );

    gtk_signal_connect( GTK_OBJECT( patchSection.patchLed->widget ),
			"value_changed",
			GTK_SIGNAL_FUNC( patchChanged ), NULL );

    gtk_signal_connect( GTK_OBJECT( patchSection.saveButton->widget ),
			"pressed",
			GTK_SIGNAL_FUNC( buttonPressed ), 
			patchSection.saveButton );


    gtk_signal_connect( GTK_OBJECT( patchSection.loadButton->widget ),
			"pressed",
			GTK_SIGNAL_FUNC( buttonPressed ), 
			patchSection.loadButton );
}

void layout( GtkFixed* f )
{
    gtk_fixed_put( f, GTK_WIDGET(powerswitch), 35, 51 );
    gtk_fixed_put( f, powerled, 39, 43 );

    gtk_fixed_put( f, keyboard->widget, 129, 111 ); 

    gtk_fixed_put( f, tuneKnob->widget, 77, 50 );
    layoutPerformanceSection(f);
    layoutTransposeHoldSection(f);
    layoutArpeggioSection(f);
    layoutLfoSection(f);
    layoutDcoSection(f);
    layoutHpfSection(f);
    layoutVcfSection(f);
    layoutVcaSection(f);
    layoutEnvSection(f);
    layoutChorusSection(f);
    layoutPatchSection(f);
}

void layoutPerformanceSection( GtkFixed* f )
{
    gtk_fixed_put( f, performanceSection.dcoSlider->widget, 22, 127 );
    gtk_fixed_put( f, performanceSection.vcfSlider->widget, 41, 127 );
    gtk_fixed_put( f, performanceSection.octaveTransposeSwitch->widget, 70, 142 );
    gtk_fixed_put( f, performanceSection.lfoTrig->widget, 75, 182 );
    gtk_fixed_put( f, performanceSection.pitchBender->widget, 66, 206);
}

void layoutTransposeHoldSection( GtkFixed* f )
{
    gtk_fixed_put( f, transposeHoldSection.transposeButton->widget, 125, 52 );
    gtk_fixed_put( f, transposeHoldSection.holdButton->widget, 152, 52 );
    gtk_fixed_put( f, transposeHoldSection.transposeButton->led, 130, 43 );
    gtk_fixed_put( f, transposeHoldSection.holdButton->led, 157, 43 );
}

void layoutArpeggioSection( GtkFixed* f )
{
    gtk_fixed_put( f, arpeggioSection.onOffButton->widget, 184, 52 );
    gtk_fixed_put( f, arpeggioSection.rateSlider->widget, 260, 33 );
    gtk_fixed_put( f, arpeggioSection.onOffButton->led, 189, 43 );
    gtk_fixed_put( f, arpeggioSection.modeSwitch->widget, 205, 48 );
    gtk_fixed_put( f, arpeggioSection.rangeSwitch->widget, 242, 48 );
}

void layoutLfoSection( GtkFixed* f )
{
    gtk_fixed_put( f, lfoSection.rateSlider->widget, 281, 33 );
    gtk_fixed_put( f, lfoSection.delaySlider->widget, 299, 33 );
    gtk_fixed_put( f, lfoSection.modeSwitch->widget, 314, 48);
}

void layoutDcoSection( GtkFixed* f )
{
    gtk_fixed_put( f, dcoSection.lfoSlider->widget, 346, 33 );
    gtk_fixed_put( f, dcoSection.pwmSlider->widget, 364, 33 );
    gtk_fixed_put( f, dcoSection.pwmSwitch->widget, 379, 48 );
    gtk_fixed_put( f, dcoSection.pulseButton->widget, 407, 52 );
    gtk_fixed_put( f, dcoSection.sawButton->widget,    423, 52 );
    gtk_fixed_put( f, dcoSection.subButton->widget,    439, 52 );
    gtk_fixed_put( f, dcoSection.pulseButton->led, 412, 43 );
    gtk_fixed_put( f, dcoSection.sawButton->led,    428, 43 );
    gtk_fixed_put( f, dcoSection.subButton->led,    444, 43 );
    gtk_fixed_put( f, dcoSection.subSlider->widget, 460, 33 );
    gtk_fixed_put( f, dcoSection.noiseSlider->widget, 478, 33 );
}

void layoutHpfSection( GtkFixed* f )
{
    gtk_fixed_put( f, hpfSection.frqSlider->widget, 502, 33 );
}

void layoutVcfSection( GtkFixed* f )
{
    gtk_fixed_put( f, vcfSection.frqSlider->widget, 526, 33 );
    gtk_fixed_put( f, vcfSection.resSlider->widget, 543, 33 );
    gtk_fixed_put( f, vcfSection.invertSwitch->widget, 565, 48 );
    gtk_fixed_put( f, vcfSection.envSlider->widget, 581, 33 );
    gtk_fixed_put( f, vcfSection.lfoSlider->widget, 600, 33 );
    gtk_fixed_put( f, vcfSection.kbdSlider->widget, 618, 33 );
}

void layoutVcaSection( GtkFixed* f )
{
    gtk_fixed_put( f, vcaSection.envSwitch->widget, 640, 48 );
    gtk_fixed_put( f, vcaSection.volumeSlider->widget, 668, 33 );
}

void layoutEnvSection( GtkFixed* f )
{
    gtk_fixed_put( f, envSection.attackSlider->widget, 689, 33 );
    gtk_fixed_put( f, envSection.decaySlider->widget, 707, 33 );
    gtk_fixed_put( f, envSection.sustainSlider->widget, 726, 33 );
    gtk_fixed_put( f, envSection.releaseSlider->widget, 743, 33 );
}

void layoutChorusSection( GtkFixed* f )
{
    gtk_fixed_put( f, chorusSection.offButton->widget, 765, 52 );
    gtk_fixed_put( f, chorusSection.oneButton->widget, 781, 52 );
    gtk_fixed_put( f, chorusSection.twoButton->widget, 797, 52 );
    gtk_fixed_put( f, chorusSection.oneButton->led, 786, 43 );
    gtk_fixed_put( f, chorusSection.twoButton->led, 802, 43 );
}

void layoutPatchSection( GtkFixed* f )
{
    gtk_fixed_put( f, patchSection.bankLed->widget, 829, 29 );
    gtk_fixed_put( f, patchSection.patchLed->widget, 841, 29 );
    gtk_fixed_put( f, patchSection.usedLed, 861, 34 );
    gtk_fixed_put( f, patchSection.saveButton->widget, 874, 29 );
    gtk_fixed_put( f, patchSection.loadButton->widget, 893, 29 );

    for( int i = 0; i < 5; i++ )
    {
	gtk_fixed_put( f, patchSection.patchButtons[i]->widget, 829 + i*16, 55 );
	gtk_fixed_put( f, patchSection.patchButtons[i+5]->widget, 829 +i*16, 73 );
    }
}

