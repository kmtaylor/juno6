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
#include <util/String.h>
#include "juno_voice.h"
#include "juno_control.h"
#include "juno_lfo.h"
#include <math.h>

extern Rand *noise;
extern JunoLfo *lfo;
extern Attenuator *pwmLfo;

#define JUNO_CONTROL_RATE 32

extern Settings *settings;
extern void setScope( int voice );

void JunoVoice_envChanged(MoogObject *o, double data, long userData)
{
    ((JunoVoice*)o)->envChanged(data);
}

void JunoVoice_lfoChanged(MoogObject *o, double data, long userData)
{
    ((JunoVoice*)o)->lfoChanged(data);
}

void JunoVoice_kbdChanged(MoogObject *o, double data, long userData)
{
    ((JunoVoice*)o)->kbdChanged(data);
}

void JunoVoice_benderChanged(MoogObject *o, double data, long userData)
{
    ((JunoVoice*)o)->benderChanged(data);
}

void JunoVoice_benderdcoChanged(MoogObject *o, double data, long userData)
{
    ((JunoVoice*)o)->benderdcoChanged(data);
}

void JunoVoice_bendervcfChanged(MoogObject *o, double data, long userData)
{
    ((JunoVoice*)o)->bendervcfChanged(data);
}

void JunoVoice_subSwitchChanged(MoogObject *o, double data, long userData)
{
    ((JunoVoice*)o)->subSwitchChanged(data);
}

void JunoVoice_dcolfoChanged(MoogObject *o, double data, long userData)
{
    ((JunoVoice*)o)->dcolfoChanged(data);
}

void JunoVoice_pwmModeChanged(MoogObject *o, double data, long userData)
{
    ((JunoVoice*)o)->pwmModeChanged(data);
}

void JunoVoice_vcffrqChanged(MoogObject *o, double data, long userData)
{
    ((JunoVoice*)o)->vcffrqChanged(data);
}

void JunoVoice_vcfresChanged(MoogObject *o, double data, long userData)
{
    ((JunoVoice*)o)->vcfresChanged(data);
}

void JunoVoice_vcfenvinvertChanged(MoogObject *o, double data, long userData)
{
    ((JunoVoice*)o)->vcfenvinvertChanged(data);
}

void JunoVoice_vcfenvChanged(MoogObject *o, double data, long userData)
{
    ((JunoVoice*)o)->vcfenvChanged(data);
}

void JunoVoice_vcflfoChanged(MoogObject *o, double data, long userData)
{
    ((JunoVoice*)o)->vcflfoChanged(data);
}

void JunoVoice_vcfkbdChanged(MoogObject *o, double data, long userData)
{
    ((JunoVoice*)o)->vcfkbdChanged(data);
}


void JunoVoice_vcamodeChanged(MoogObject *o, double data, long userData)
{
    ((JunoVoice*)o)->vcamodeChanged(data);
}

JunoVoice::JunoVoice(JunoControl *_jc, int _voiceNum)
    : subMix(4)
{
    jc = _jc;
    voiceNum = _voiceNum;

    vcamode = 0;
    addInput("lfo", JunoVoice_lfoChanged, 0, JUNO_CONTROL_RATE);
    addInput("env", JunoVoice_envChanged, 0, JUNO_CONTROL_RATE);
    addInput("kbd", JunoVoice_kbdChanged, 0, 1);
    addInput("bender", JunoVoice_benderChanged, 0, 1);
    addInput("bender_dco", JunoVoice_benderdcoChanged, 0, 1);
    addInput("bender_vcf", JunoVoice_bendervcfChanged, 0, 1);
    addInput("dco_lfo", JunoVoice_dcolfoChanged, 0, 1);
    addInput("dco_pwm_mod", JunoVoice_pwmModeChanged, 0, 1);
    addInput("dco_sub_switch", JunoVoice_subSwitchChanged, 0, 1);
    addInput("vcf_frq", JunoVoice_vcffrqChanged, 0, 1);
    addInput("vcf_res", JunoVoice_vcfresChanged, 0, 1);
    addInput("vcf_env_invert", JunoVoice_vcfenvinvertChanged, 0, 1);
    addInput("vcf_env", JunoVoice_vcfenvChanged, 0, 1);
    addInput("vcf_lfo", JunoVoice_vcflfoChanged, 0, 1);
    addInput("vcf_kbd", JunoVoice_vcfkbdChanged, 0, 1);
    addInput("vca_mode", JunoVoice_vcamodeChanged, 0, 1);
    attachVoice(jc);
    PATCH(jc, "bender", this, "bender");
    PATCH(jc, "bender_dco", this, "bender_dco");
    PATCH(jc, "bender_vcf", this, "bender_vcf");
    PATCH(jc, "dco_lfo", this, "dco_lfo");
    PATCH(jc, "dco_pwm", &pwmAttenuator, "amp");
    PATCH(&pwmAttenuator, "sig", &pulse, "width");
    PATCH(jc, "dco_pwm_mod", this, "dco_pwm_mod");
    PATCH(jc, "dco_pulse_switch", &pulse, "amp");
    PATCH(jc, "dco_saw_switch", &saw, "amp");
    pwmAttConnection = PATCH(pwmLfo, "sig", &pwmAttenuator, "sig");
    pwmAttenuator.set("sig", 1.0);

    sub.set("amp", 0);
    
    /* we will listen for dco_sub_switch changes and handle them */
    subOscConnection = NULL;
    PATCH(jc, "dco_sub_switch", this, "dco_sub_switch");

    /* hook all oscillators to internal mix */
    PATCH(&pulse, "sig", &subMix, "sig0");
    PATCH(&saw, "sig", &subMix, "sig1");
    PATCH(&sub, "sig", &subMix, "sig2");
    /* keep the sub in sync with the other two */
    PATCH( &sub, "sync", &pulse, "sync" );
    PATCH( &sub, "sync", &saw,   "sync" );

    /* patch the noice generator into the subMix */
    PATCH(noise, "sig", &subMix, "sig3");
    PATCH(jc, "dco_noise", &subMix, "amp3");
    
    /* the juno appears to have the pulse set with low starting, so -.25 */
    subMix.set("amp0", -.5); //pulse
    subMix.set("amp1", .5);  //saw
    subMix.set("amp2", -.67); //sub
    /* connect the vcf */
    PATCH(&subMix, "sig", &vcf, "sig");
    PATCH(&subMix, "sig", &balance, "pow");

    /* we need to do a calculation for the input of the vcf using lots of
     * data input... including:
     */
    PATCH(&adsr, "sig", this, "env");
    PATCH(::lfo, "sig", this, "lfo");
    //kbd is patched in attachVoice

    PATCH(jc, "vcf_frq", this, "vcf_frq");
    PATCH(jc, "vcf_res", this, "vcf_res");

    PATCH(jc, "vcf_env_invert", this, "vcf_env_invert");

    PATCH(jc, "vcf_env", this, "vcf_env");
    PATCH(jc, "vcf_lfo", this, "vcf_lfo");
    PATCH(jc, "vcf_kbd", this, "vcf_kbd"); 
    //FIXME what about the filter gain??? hardcode to .1...
    vcf.set("gain", .1);

    /* connect the ADSR */
    PATCH(jc, "env_attack", &adsr, "a");
    PATCH(jc, "env_decay", &adsr, "d");
    PATCH(jc, "env_sustain", &adsr, "s");
    PATCH(jc, "env_release", &adsr, "r");

    /* set the gate ADSR */
    gateAdsr.set("a", .000000001);
    gateAdsr.set("d", 0.0);
    gateAdsr.set("s", 1.0);
    gateAdsr.set("r", .000000001);

    /* send the vcf into the main volume control for voice */
    PATCH(&vcf, "sig", &balance, "sig");
    PATCH(&balance, "sig", &voiceVol, "sig");
    /* amp of voice out is either gated or from ADSR, switched by vca_mode */
    PATCH(jc, "vca_mode", this, "vca_mode");

    /* the output of the voice is the sig output of the voiceVol */
    outputs.appendElement(voiceVol.getOutput("sig"));
}

JunoVoice::~JunoVoice()
{
    /* free the output borrowed from the voiceVol object */
    outputs.removeElement(0);
}

void JunoVoice::attachVoice(MoogObject *src)
{
    voiceSrc = src;
    PATCH(src, (String)"voice"+voiceNum+"_gate", &adsr, "trig");
    PATCH(src, (String)"voice"+voiceNum+"_gate", &gateAdsr, "trig");
    PATCH(src, (String)"voice" + voiceNum + "_pitch", this, "kbd");
}

void JunoVoice::envChanged(double data)
{
    env = data;
    updateVcf();
}

void JunoVoice::lfoChanged(double data)
{
    lfo = data;
    //printf("lfo is %f\n", lfo);
    updateVcf();
    updateFrq();
}

void JunoVoice::kbdChanged(double data)
{
    // this gets rid of a lot of clicks for me, KARL
    pulse.sync();
    saw.sync();
    sub.sync();

    kbd = data;
    updateVcf();
    updateFrq();

    setScope( voiceNum );
}

void JunoVoice::benderChanged(double data)
{
    //printf("bender changed to %f\n", data);
    bender = data;
    updateVcf();
    updateFrq();
}

void JunoVoice::benderdcoChanged(double data)
{
    //printf("benderdco changed to %f\n", data);
    benderdco = data;
    updateFrq();
}

void JunoVoice::bendervcfChanged(double data)
{
    //printf("bendervcf changed to %f\n", data);
    bendervcf = data;
    updateVcf();
}

void JunoVoice::subSwitchChanged(double data)
{
    if (data)
    {
	if (!subOscConnection)
	    subOscConnection = PATCH(jc, "dco_sub", &sub, "amp");
    }
    else
    {
	if (subOscConnection)
	    ConnectionManager::disconnect(subOscConnection);

	subOscConnection = NULL;
    }
}

void JunoVoice::dcolfoChanged(double data)
{
    //printf("dcolfo changed to %f\n", data);
    dcolfo = data;
    updateFrq();
}

void JunoVoice::vcffrqChanged(double data)
{
    vcffrq = data;
    updateVcf();
}

void JunoVoice::vcfresChanged(double data)
{
    vcf.set("resonance", pow( data / 2, 2.0 ) );
    updateVcf();
}


void JunoVoice::pwmModeChanged(double data)
{
    //printf("pwmMode changed to %f\n", data);

    if (data == 0.0)
    {
	if (pwmAttConnection)
	    ConnectionManager::disconnect(pwmAttConnection);

	pwmAttConnection = NULL;
    }
    else if (data == -1.0)
    {
	pwmAttConnection = PATCH(pwmLfo, "sig", &pwmAttenuator, "sig");
    }
    else 
    {
	pwmAttConnection = PATCH(&adsr, "sig", &pwmAttenuator, "sig");
    }

/*
    if (data == -1.0)
	PATCH(::pwm_attenuator, "sig", &pulse, "width");
    else if (data == 0.0)
	PATCH(jc, "dco_pwm", &pulse, "width");
    else
	PATCH(&adsr, "sig", &pulse, "width");
*/
}

void JunoVoice::vcfenvinvertChanged(double data)
{
    if (!data)
	vcfenvinvert = 1.0;
    else
	vcfenvinvert = -1.0;
}

void JunoVoice::vcfenvChanged(double data)
{
    vcfenv = pow( data, 4.0 );
    updateVcf();
}

void JunoVoice::vcflfoChanged(double data)
{
    vcflfo = pow( data, 4.0 );
    updateVcf();
}

void JunoVoice::vcfkbdChanged(double data)
{
    vcfkbd = data;
    updateVcf();
}

void JunoVoice::vcamodeChanged(double data)
{
    if (data)
    {
	PATCH(&gateAdsr, "sig", &voiceVol, "amp");
	vcamode = 1;
    }
    else
    {
	PATCH(&adsr, "sig", &voiceVol, "amp");
	vcamode = 0;
    }
}

void JunoVoice::updateVcf()
{
    double tmp = vcffrq;

    //printf("tmp is %f\n", tmp);
    //printf("%f %f %f\n", kbd, CPS(33), CPS(12000));
    tmp += (kbd - CPS(33))/0.093424 * vcfkbd;
    //printf("tmp: %f lfo: %f vcflfo: %f\n", tmp, lfo, vcflfo);
    tmp += lfo * vcflfo;
    //printf("tmp is %f\n", tmp);
    tmp += env * vcfenv * vcfenvinvert;
    //printf("tmp is %f\n", tmp);

    tmp = tmp * pow(2, bender * bendervcf);

    tmp = pow( tmp, 4.0 );
    if (tmp >= .999)
	tmp = .999;

    if (tmp <= CPS(20))
	tmp = CPS(20);

    vcf.set("cutoff", tmp );
}

void JunoVoice::updateFrq()
{
    double frq = kbd * pow(2, 2 * lfo * dcolfo / 12) * pow(2, bender * benderdco);

    pulse.set("frq", frq);
    saw.set("frq", frq);
    sub.set("frq", frq/2);
}
