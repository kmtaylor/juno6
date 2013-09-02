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
#ifndef _JUNO_WRAPPERS_H
#define _JUNO_WRAPPERS_H

#include <moog/moog.h>
#include <gmoog/juno_widgets.h>

/***********************/

class JunoKeyboard : public MoogObject
{
    friend void JunoKeyboard_midiGateChanged(MoogObject *, double, long);
    friend void JunoKeyboard_gtkKeyPressed(GtkWidget *, guint, guint,gpointer);
    friend void JunoKeyboard_gtkKeyReleased(GtkWidget *, guint, gpointer);
    friend void JunoKeyboard_octaveTransposeChanged(MoogObject *, double,long);
    friend void JunoKeyboard_keyTransposeChanged(MoogObject *, double, long);
    friend void JunoKeyboard_masterTuneChanged(MoogObject *, double, long);
    friend void JunoKeyboard_holdChanged(MoogObject *, double, long);

    int octaveTranspose;
    int keyTransposePressed;
    int keyTranspose;
    int holdPressed;
    int numVoices;
    double masterTune;

    int *savedGateInfo;
    Output **pitchOutputs;
    Output **gateOutputs;

    void midiGateChanged(int, double);
    void gtkKeyPressed(unsigned int, unsigned int);
    void gtkKeyReleased(unsigned int);
    void octaveTransposeChanged(double);
    void keyTransposeChanged(double);
    void masterTuneChanged(double);
    void holdChanged(double);

    void transposeVoices(double);

 public:
    GtkWidget* widget;

    JunoKeyboard(int);
    ~JunoKeyboard();

    int getKeyTranspose() { return keyTranspose; }

    const char* getClassName(){ return( "JunoKeyboard" ); }
};

/***********************/

class JunoSlider : public MoogObject
{
    friend void JunoSlider_midiValueChanged(MoogObject *, double, long);

    Output* output;
    void midiValueChanged(double );
 public:
    void setValue(double );
    double getValue();
    GtkWidget* widget;
    GtkAdjustment* adj;

    JunoSlider( const char* name );
    void updateOutput();
    const char* getClassName(){ return( "JunoSlider" ); }
};

/***********************/

class JunoButton : public MoogObject
{
    friend void JunoButton_midiValueChanged(MoogObject *, double, long);

    int state;
    Output* output;

    void midiValueChanged(double);

 public:
    GtkWidget* widget;
    GtkWidget* led;

    JunoButton( GtkJunoButtonType type, const char* name, bool useLed = true );

    void buttonPressed();
    void set( int _state );
    void setLed(int);
    void setValue( double);
    double getValue();
    const char* getClassName(){ return( "JunoButton" ); }
};

/***********************/

class JunoSwitch : public MoogObject
{
    friend void JunoSwitch_midiValueChanged(MoogObject *, double, long);

    Output *output;

    void midiValueChanged(double);

 public:
    GtkWidget* widget;
    void setValue( double);
    double getValue();
    JunoSwitch(GtkJunoSwitchType type, const char* name );
    void updateOutput();
    const char* getClassName(){ return( "JunoSwitch" ); }
};

/***********************/

class JunoKnob : public MoogObject
{
    friend void JunoKnob_midiValueChanged(MoogObject *, double, long);

    Output *output;

 public:
    void midiValueChanged(double);
    GtkWidget *widget;
    void setValue( double);
    double getValue();    
    JunoKnob(const char *name);
    void updateOutput();
    const char *getClassName() { return "JunoKnob"; }
};

/***********************/

class JunoAlphaLed : public MoogObject
{
    friend void JunoAlphaLed_midiValueChanged(MoogObject *, double, long);

    Output *output;

    void midiValueChanged(double);

 public:
    GtkWidget *widget;
    void setValue( double);
    JunoAlphaLed( const char*);
    void updateOutput();
    const char *getClassName() { return "JunoAlphaLed"; }
    int getValue();
};

/***********************/
class JunoBender : public MoogObject
{
    friend void JunoBender_midiValueChanged(MoogObject *, double, long);

    Output *output;

    void midiValueChanged(double);

 public:
    GtkWidget *widget;
    
    JunoBender(const char *name);
    void updateOutput();
    const char *getClassName() { return "JunoBender"; }
};

/***********************/
class JunoLfoTrigger : public MoogObject
{
    friend void JunoLfoTrigger_midiValueChanged(MoogObject *, double, long);

    Output *output;

    void midiValueChanged(double);

 public:
    GtkWidget *widget;
    
    JunoLfoTrigger(const char *name);
    void updateOutput();
    const char *getClassName() { return "JunoLfoTrigger"; }
};

#endif /* _JUNO_WRAPPERS_H */
