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
#ifndef _JUNO_JUNO_VOICE_H
#define _JUNO_JUNO_VOICE_H

#include <moog/moog.h>
class JunoControl;

class JunoVoice : public MoogObject
{
    friend void JunoVoice_envChanged(MoogObject *, double, long);
    friend void JunoVoice_lfoChanged(MoogObject *, double, long);
    friend void JunoVoice_kbdChanged(MoogObject *, double, long);
    friend void JunoVoice_benderChanged(MoogObject *, double, long);
    friend void JunoVoice_benderdcoChanged(MoogObject *, double, long);
    friend void JunoVoice_bendervcfChanged(MoogObject *, double, long);
    friend void JunoVoice_subSwitchChanged(MoogObject *, double, long);
    friend void JunoVoice_dcolfoChanged(MoogObject *, double, long);
    friend void JunoVoice_pwmModeChanged(MoogObject *, double, long);
    friend void JunoVoice_vcfenvinvertChanged(MoogObject *, double, long);
    friend void JunoVoice_vcffrqChanged(MoogObject *, double, long);
    friend void JunoVoice_vcfresChanged(MoogObject *, double, long);
    friend void JunoVoice_vcfkbdChanged(MoogObject *, double, long);
    friend void JunoVoice_vcflfoChanged(MoogObject *, double, long);
    friend void JunoVoice_vcfenvChanged(MoogObject *, double, long);
    friend void JunoVoice_vcamodeChanged(MoogObject *, double, long);

    JunoControl *jc;
    int voiceNum;

    /* basic elements */
    //PulseWave pulse;
    //SawWave saw;
    JunoPulse pulse;
    JunoSaw saw;
 public:
    //PulseWave sub;
    JunoPulse sub;
 private:
    ResonantLowPass vcf;
    ADSR adsr;
    ADSR gateAdsr;

    Balance balance;

    /* keep track of certain connections so they can be disconnected */
    ConnectionInfo *subOscConnection;
    ConnectionInfo *pwmAttConnection;

    /* additional internals */
    Attenuator voiceVol;
    Mixer subMix;
    Attenuator pwmAttenuator;

    /* vcf calculation variables */
    double env;
    double lfo;
    double kbd;
    double bender;
    double benderdco;
    double bendervcf;
    double dcolfo;
    double vcffrq;
    double vcfenvinvert;
    double vcfenv;    
    double vcflfo;
    double vcfkbd;
    
    int vcamode;

    void envChanged(double);
    void lfoChanged(double);
    void kbdChanged(double);
    void benderChanged(double);
    void benderdcoChanged(double);
    void bendervcfChanged(double);
    void subSwitchChanged(double);
    void dcolfoChanged(double);
    void pwmModeChanged(double);
    void vcffrqChanged(double);
    void vcfresChanged(double);
    void vcfenvinvertChanged(double);
    void vcfenvChanged(double);
    void vcflfoChanged(double);
    void vcfkbdChanged(double);
    void vcamodeChanged(double);

    void updateVcf();
    void updateFrq();

    MoogObject *voiceSrc;

 public:
    JunoVoice(JunoControl *, int);
    ~JunoVoice();

    void attachVoice(MoogObject *);

    const char *getClassName() { return "JunoVoice"; }
};

#endif /* _JUNO_JUNO_VOICE_H */









