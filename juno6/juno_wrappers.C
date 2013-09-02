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
#include "juno_control.h"
#include "juno_wrappers.h"

//FIXME: globals from juno_gui.C
extern Settings* settings;
extern JunoControl* junoControl;
extern MidiInput* midiInput;
extern JunoKeyboard* keyboard;

/* JunoKeyboard */
void JunoKeyboard_midiGateChanged(MoogObject *o, double data, long userData)
{
    ((JunoKeyboard*)o)->midiGateChanged(userData, data);
}

void JunoKeyboard_gtkKeyPressed(GtkWidget *keyboard, guint voice, guint key, gpointer data)
{
    ((JunoKeyboard*)data)->gtkKeyPressed(voice, key);
}

void JunoKeyboard_gtkKeyReleased(GtkWidget *keyboard, guint voice, gpointer data)
{
    ((JunoKeyboard*)data)->gtkKeyReleased(voice);
}

void JunoKeyboard_octaveTransposeChanged(MoogObject *o, double data, long)
{
    ((JunoKeyboard*)o)->octaveTransposeChanged(data);
}

void JunoKeyboard_keyTransposeChanged(MoogObject *o, double data, long)
{
    ((JunoKeyboard*)o)->keyTransposeChanged(data);
}

void JunoKeyboard_masterTuneChanged(MoogObject *o, double data, long)
{
    ((JunoKeyboard*)o)->masterTuneChanged(data);
}

void JunoKeyboard_holdChanged(MoogObject *o, double data, long)
{
    ((JunoKeyboard*)o)->holdChanged(data);
}

JunoKeyboard::JunoKeyboard(int _numVoices)
{
    numVoices = _numVoices;
    octaveTranspose = -1;
    keyTransposePressed = 0;
    keyTranspose = 0;
    holdPressed = 0;
    savedGateInfo = new int[numVoices];
    pitchOutputs = new Output *[numVoices];
    gateOutputs = new Output *[numVoices];

    memset(savedGateInfo, 0, numVoices * sizeof(int));

    widget = gtk_juno_keyboard_new(numVoices);
    gtk_widget_show( widget );

    gtk_signal_connect(GTK_OBJECT(widget), 
		       "key_pressed", 
		       GTK_SIGNAL_FUNC(JunoKeyboard_gtkKeyPressed),
		       (gpointer)this);
    gtk_signal_connect(GTK_OBJECT(widget), 
		       "key_released", 
		       GTK_SIGNAL_FUNC(JunoKeyboard_gtkKeyReleased),
		       (gpointer)this);

    for( int i = 0; i < numVoices; i++ )
    {
	String tmp1, tmp2;

	tmp1.sprintf( "sig%d", i);
	tmp2.sprintf( "voice%d_pitch", i);
	//puts("add input");	
	if (midiInput)
	{
	//puts("add really");
	    addInput(tmp2, NULL,0,0);
	    PATCH(midiInput, tmp1, this, tmp2);
 	//puts("da isser wieda");
	}
 	//printf("get output %s\n",tmp2);
	pitchOutputs[i] = junoControl->getOutput(tmp2);

	tmp1.sprintf( "amp%d", i );
	tmp2.sprintf( "voice%d_gate", i );
	
	if (midiInput)
	{
	    addInput(tmp2, JunoKeyboard_midiGateChanged, i, 1);
	    PATCH( midiInput, tmp1, this, tmp2 );
	}
	gateOutputs[i] = junoControl->getOutput(tmp2);
    }

    masterTune = 0;
    addInput("octave_transpose", JunoKeyboard_octaveTransposeChanged, 0, 1);
    addInput("key_transpose", JunoKeyboard_keyTransposeChanged, 0, 1);
    addInput("master_tune", JunoKeyboard_masterTuneChanged, 0, 1);
    addInput("hold", JunoKeyboard_holdChanged, 0, 1);

    PATCH(junoControl, "octave_transpose", this, "octave_transpose");
    //FIXME: these names should match
    PATCH(junoControl, "transpose_switch", this, "key_transpose");
    PATCH(junoControl, "master_tune", this, "master_tune");
    PATCH(junoControl, "hold_switch", this, "hold");

}

JunoKeyboard::~JunoKeyboard()
{
    delete[] savedGateInfo;
    delete[] pitchOutputs;
    delete[] gateOutputs;
}

void JunoKeyboard::midiGateChanged(int voiceNum, double data)
{
    gdk_threads_enter();
    if ( data > 0 )
	gtk_juno_keyboard_note_on( GTK_JUNO_KEYBOARD( widget ), 
				   pitch_to_note( Scheduler::nyquistFreq * *inputs[ voiceNum * 2 ].data ) - 36);
       else
	gtk_juno_keyboard_note_off( GTK_JUNO_KEYBOARD( widget ), 
				    pitch_to_note( Scheduler::nyquistFreq * *inputs[ voiceNum * 2 ].data ) - 36);
    gdk_threads_leave();
}

void JunoKeyboard::gtkKeyPressed(unsigned int voice, unsigned int key)
{
    //printf("----------------key %u was pressed on voice %u\n", key, voice);

    if (keyTransposePressed)
    {
	transposeVoices(pow(2, ((double)(key % 12) - keyTranspose)/12));
	keyTranspose = key % 12;
    }
    else
    {
	double val = CPS(midi_notes[key + 36 - octaveTranspose * 12 + keyTranspose])*pow( 2, masterTune/12 );
	pitchOutputs[voice]->setData(val);
	gateOutputs[voice]->setData(1.0);

	savedGateInfo[voice] = 1;
    }
}

void JunoKeyboard::gtkKeyReleased(unsigned int voice)
{
    //printf("----------------voice %u was released\n", voice);

    if (!holdPressed)
	gateOutputs[voice]->setData(0.0);

    savedGateInfo[voice] = 0;
}


void JunoKeyboard::octaveTransposeChanged(double data)
{
    //printf("octave transpose changed to %f\n", data);
    
    int octaves = (int)octaveTranspose - (int)data;
    octaveTranspose = (int)data;

    transposeVoices(pow(2, (double)octaves));
}

void JunoKeyboard::keyTransposeChanged(double data)
{
    //printf("key transpose changed to %f\n", data);
    keyTransposePressed = (data) ? 1 : 0;
}


void JunoKeyboard::masterTuneChanged(double data)
{
    data = data * 2 - 1;
    double adjustment = pow( 2, (data - masterTune)/12.0);     
    masterTune = data;

    //printf("master tune changed to %f adjustment=%f\n", data, adjustment);
    transposeVoices( adjustment );   
}

void JunoKeyboard::holdChanged(double data)
{
    //printf("hold changed to %f\n", data);
    if (!(holdPressed = (data) ? 1 : 0))
    {
	for (int i = 0; i < numVoices; i++)
	    if (!savedGateInfo[i])
		gateOutputs[i]->setData(0.0);
    }
}

void JunoKeyboard::transposeVoices(double tune)
{
    //printf( "TRANSPOSE BY %f\n", tune );
    for (int i = 0; i < numVoices; i++)
	pitchOutputs[i]->setData(pitchOutputs[i]->data * tune);
}

/* ********* JunoSlider ********** */

void JunoSlider_midiValueChanged(MoogObject *o, double data, long userData)
{
    //printf("JunoSlider_midiValueChanged %f\n", data);
    ((JunoSlider*)o)->midiValueChanged(data);
}

void gtkSliderValueChanged( GtkWidget *widget, 
			    gpointer data )
{
    ((JunoSlider*)data)->updateOutput();
}

JunoSlider::JunoSlider( const char* name )
{
    adj = (GtkAdjustment*)gtk_adjustment_new( 0.0, 0.0, 1.0, 0.01, 0.01, 0.01 );
    widget = gtk_juno_slider_new( adj );
    gtk_widget_show( widget );

    gtk_signal_connect( GTK_OBJECT( adj ), 
			"value_changed",
			GTK_SIGNAL_FUNC( gtkSliderValueChanged ), 
			(gpointer)this );

    output = junoControl->getOutput( name );

    if (midiInput != NULL )
    {
	String midiChannel = settings->getString("juno6-midi", name);
	if ( midiChannel.getLength() > 0 )
	{
	    addInput( "in", JunoSlider_midiValueChanged, 0, 1);
	    //printf( "---------JunoSlider\n%s\n", name );
	    PATCH( midiInput, midiChannel, this, "in" );
	}
    }
}

void JunoSlider::setValue(double data)
{
    gtk_adjustment_set_value( adj, data );
}

double JunoSlider::getValue()
{
    return( adj->value );
}

void JunoSlider::midiValueChanged(double data)
{
    gdk_threads_enter();
    gtk_adjustment_set_value( adj, data );
    gdk_threads_leave();
}

void
JunoSlider::updateOutput()
{
    output->setData(adj->value);
}

/* ************* JunoButton *************8 */

//FIXME move gtk callback here instead of juno_gui.C

//void JunoButton_callback(GtkWidget *widget, 
//			 gpointer data )
//{
//    ((JunoButton*)data)->buttonPressed();
//}

void JunoButton_midiValueChanged(MoogObject *o, double data, long userData)
{
    ((JunoButton*)o)->midiValueChanged(data);
}

JunoButton::JunoButton( GtkJunoButtonType type, 
			const char* name, bool useLed )
{
    widget = gtk_juno_button_new( type );
    gtk_widget_show( widget );

    led = NULL;
    if ( useLed )
    {
	led = gtk_juno_led_new();
	gtk_widget_show( led );
    }

    output = junoControl->getOutput( name );

    state = 0;

    if ( midiInput != NULL)
    {
	String midiChannel = settings->getString("juno6-midi", name);
	if ( midiChannel.getLength() > 0 )
	{
	    addInput( "in", JunoButton_midiValueChanged, 0, 1 );
	    //printf( "---------JunoButton\n%s\n", name );
	    PATCH(midiInput, midiChannel, this, "in");
	}
    }
}

void JunoButton::set( int _state )
{
    state = _state;
    if ( led != NULL )
	gtk_juno_led_set_state( GTK_JUNO_LED( led ), state );
    
    output->setData(state);
}

void JunoButton::setLed( int _state )
{
    if ( led != NULL )
	gtk_juno_led_set_state( GTK_JUNO_LED( led ), _state );
}

void JunoButton::setValue(double data)
{
    state = (int)data;
    gtk_juno_led_set_state( GTK_JUNO_LED( led ), state );
    output->setData(state);
}

double JunoButton::getValue()
{
    return( state );
}

void JunoButton::buttonPressed()
{
    set( !state );
}

void JunoButton::midiValueChanged(double data)
{
    gdk_threads_enter();
    
    if ( data > 0 )
	gtk_juno_button_do_press( GTK_JUNO_BUTTON( widget ) );
    else
	gtk_juno_button_do_release( GTK_JUNO_BUTTON( widget ) );
    
    gdk_threads_leave();
}

/* *********** JunoSwitch *********** */

void JunoSwitch_midiValueChanged(MoogObject *o, double data, long userData)
{
    ((JunoSwitch *)o)->midiValueChanged(data);
}

static void JunoSwitch_callback( GtkWidget *widget, 
				 gpointer data )
{
    ((JunoSwitch*)data)->updateOutput();
}

JunoSwitch::JunoSwitch( GtkJunoSwitchType type, 
			const char* name )
{
    widget = gtk_juno_switch_new( type );
    gtk_widget_show( widget );

    gtk_signal_connect( GTK_OBJECT( widget ), 
			"switched",
			GTK_SIGNAL_FUNC( JunoSwitch_callback ), 
			(gpointer)this );

    output = junoControl->getOutput( name );

    if ( midiInput != NULL )
    {
	String midiChannel = settings->getString("juno6-midi", name);
	if ( midiChannel.getLength() > 0 )
	{
	    addInput( "in", JunoSwitch_midiValueChanged, 0, 1);
	    //printf( "---------JunoSwitch\n%s\n", name );
	    PATCH(midiInput, midiChannel, this, "in");
	}
    }
}

void JunoSwitch::midiValueChanged(double data)
{
    if (data)
    {
	gdk_threads_enter();
	gtk_juno_switch_toggle(GTK_JUNO_SWITCH(widget));
	gdk_threads_leave();
    }
}

void JunoSwitch::setValue(double data)
{
    gtk_juno_switch_set_pos(GTK_JUNO_SWITCH(widget), (GtkJunoSwitchPos)data);
}

double JunoSwitch::getValue()
{
    return( gtk_juno_switch_get_pos(GTK_JUNO_SWITCH(widget)));
}

void JunoSwitch::updateOutput()
{
    GtkJunoSwitch *juno_switch = GTK_JUNO_SWITCH(widget);
    GtkJunoSwitchPos pos = gtk_juno_switch_get_pos(juno_switch);
    GtkJunoSwitchType type = gtk_juno_switch_get_switch_type(juno_switch);

    switch(pos)
    {
    case GTK_JUNO_SWITCH_POS_UP:
	output->setData((type == GTK_JUNO_SWITCH_2WAY) ? 0 : -1);
	break;

    case GTK_JUNO_SWITCH_POS_MID:
	output->setData(0);
	break;

    case GTK_JUNO_SWITCH_POS_DOWN:
	output->setData(1);
    }
}

/* ************ JunoKnob ********* */

void JunoKnob_midiValueChanged(MoogObject *o, double data, long userData)
{
    ((JunoKnob*)o)->midiValueChanged(data);
}

static void JunoKnob_callback(GtkWidget *widget, gpointer data)
{
    ((JunoKnob *)data)->updateOutput();
}

JunoKnob::JunoKnob(const char *name)
{
    widget = gtk_juno_knob_new();
    gtk_widget_show( widget );

    gtk_signal_connect( GTK_OBJECT( widget ), 
			"value_changed",
			GTK_SIGNAL_FUNC( JunoKnob_callback ), 
			(gpointer)this );

    output = junoControl->getOutput(name);

    if (midiInput != NULL)
    {
	String midiChannel = settings->getString("juno6-midi", name);
	if ( midiChannel.getLength() > 0 )
	{
	    addInput( "in", JunoKnob_midiValueChanged, 0, 1);
	    //printf( "---------JunoKnob\n%s\n", name );
	    PATCH(midiInput, midiChannel, this, "in");
	}
    }
}

void JunoKnob::midiValueChanged(double data)
{
    gdk_threads_enter();
    gtk_juno_knob_set_value(GTK_JUNO_KNOB(widget), data);
    gdk_threads_leave();
}

void JunoKnob::setValue(double data)
{
    gtk_juno_knob_set_value(GTK_JUNO_KNOB(widget), data);
}

double JunoKnob::getValue()
{
    return(gtk_juno_knob_get_value(GTK_JUNO_KNOB(widget)));
}

void JunoKnob::updateOutput()
{
    GtkJunoKnob *juno_knob = GTK_JUNO_KNOB(widget);
    //printf("JunoKnob value is %f\n", gtk_juno_knob_get_value(juno_knob));
    if ( output )
	output->setData(gtk_juno_knob_get_value(juno_knob));
}

/************* JunoAlphaLed ********* */
void JunoAlphaLed_midiValueChanged(MoogObject *o, double data, long userData)
{
    ((JunoAlphaLed*)o)->midiValueChanged(data);
}

static void JunoAlphaLed_callback(GtkWidget *widget, gpointer data)
{
    ((JunoAlphaLed *)data)->updateOutput();
}

JunoAlphaLed::JunoAlphaLed( const char* name )
{
    widget = gtk_alpha_led_new();
    gtk_widget_show( widget );

    gtk_signal_connect( GTK_OBJECT( widget ), 
			"value_changed",
			GTK_SIGNAL_FUNC( JunoAlphaLed_callback ), 
			(gpointer)this );

    output = junoControl->getOutput(name);

    if (midiInput != NULL)
    {
	String midiChannel = settings->getString("juno6-midi", name);
	if ( midiChannel.getLength() > 0 )
	{
	    addInput( "in", JunoAlphaLed_midiValueChanged, 0, 1);
	    //printf( "---------JunoalphaLed\n%s\n", name );
	    PATCH(midiInput, midiChannel, this, "in");
	}
    }
}

void JunoAlphaLed::midiValueChanged(double data)
{
//    gdk_threads_enter();
//    gtk_alpha_led_set_value(GTK_ALPHA_LED(widget), data);
//    gdk_threads_leave();
}

void JunoAlphaLed::setValue(double data)
{
    gtk_alpha_led_set_value(GTK_ALPHA_LED(widget), (int)data);
}

void JunoAlphaLed::updateOutput()
{
//    GtkAlpha_Led *alpha_led = GTK_ALPHA_LED(widget);
//    printf("JunoalphaLed value is %f\n", gtk_alpha_led_get_value(alpha_led));
//    output->setData(gtk_alpha_led_get_value(alpha_led));
}

int JunoAlphaLed::getValue()
{
    return( (int)gtk_alpha_led_get_value( GTK_ALPHA_LED( widget ) ) );
}

/* ************ JunoBender ********* */

void JunoBender_midiValueChanged(MoogObject *o, double data, long userData)
{
    ((JunoBender*)o)->midiValueChanged(data);
}

static void JunoBender_callback(GtkWidget *widget, gpointer data)
{
    ((JunoBender *)data)->updateOutput();
}

JunoBender::JunoBender(const char *name)
{
    widget = gtk_juno_bender_new();
    gtk_widget_show( widget );

    gtk_signal_connect( GTK_OBJECT( widget ), 
			"value_changed",
			GTK_SIGNAL_FUNC( JunoBender_callback ), 
			(gpointer)this );

    output = junoControl->getOutput(name);

    if (midiInput != NULL)
    {
	addInput( "in", JunoBender_midiValueChanged, 0, 1);
	//printf( "---------JunoBender\n%s\n", name );
	PATCH(midiInput, "bend", this, "in");
    }
}

void JunoBender::midiValueChanged(double data)
{
    gdk_threads_enter();
    gtk_juno_bender_set_value(GTK_JUNO_BENDER(widget), data);
    gdk_threads_leave();
}

void JunoBender::updateOutput()
{
    GtkJunoBender *juno_bender = GTK_JUNO_BENDER(widget);
    //printf("JunoBender value is %f\n", gtk_juno_bender_get_value(juno_bender));
    output->setData(gtk_juno_bender_get_value(juno_bender));
}

/* ************ JunoLfoTrigger ********* */
void JunoLfoTrigger_midiValueChanged(MoogObject *o, double data, long userData)
{
    //printf( "MIDI VALUE CHANGED %f\n", data );
    ((JunoLfoTrigger*)o)->midiValueChanged(data);
}

static void JunoLfoTrigger_callback(GtkWidget *widget, gpointer data)
{
    ((JunoLfoTrigger *)data)->updateOutput();
}

JunoLfoTrigger::JunoLfoTrigger(const char *name)
{
    widget = gtk_juno_lfotrig_new();
    gtk_widget_show( widget );

    gtk_signal_connect( GTK_OBJECT( widget ), 
			"value_changed",
			GTK_SIGNAL_FUNC( JunoLfoTrigger_callback ), 
			(gpointer)this );

    output = junoControl->getOutput(name);

    if (midiInput != NULL)
    {
	String midiChannel = settings->getString("juno6-midi", name);
	if ( midiChannel.getLength() > 0 )
	{
	    addInput( "in", JunoLfoTrigger_midiValueChanged, 0, 1);
	    PATCH(midiInput, midiChannel, this, "in");
	}
    }
}

void JunoLfoTrigger::midiValueChanged(double data)
{
    gdk_threads_enter();
    gtk_juno_lfotrig_set_value(GTK_JUNO_LFOTRIG(widget), (int)data);
    gdk_threads_leave();
}

void JunoLfoTrigger::updateOutput()
{
    GtkJunoLfotrig *juno_lfotrig = GTK_JUNO_LFOTRIG(widget);
    output->setData(gtk_juno_lfotrig_get_value(juno_lfotrig));
}




