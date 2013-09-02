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
 * $Revision: 1.6.4.1 $$Date: 2000/02/24 00:43:48 $
 */
#ifndef JUNO_PATCH_H
#define JUNO_PATCH_H

typedef struct 
{
    int    version;
    int    used;
    double bender_dco;
    double bender_vcf;
    double lfo_trigger;
    double volume;
    double octave_transpose;
    double arpeggio_switch;
    double arpeggio_mode;
    double arpeggio_range;
    double arpeggio_rate;
    double lfo_rate;
    double lfo_delay;
    double lfo_mode;
    double dco_lfo;
    double dco_pwm;
    double dco_pwm_mod;
    double dco_pulse_switch;
    double dco_saw_switch;
    double dco_sub_switch;
    double dco_sub;
    double dco_noise;
    double hpf_frq;
    double vcf_frq;
    double vcf_res;
    double vcf_env_invert;
    double vcf_env;
    double vcf_lfo;
    double vcf_kbd;
    double vca_mode;
    double env_attack;
    double env_decay;
    double env_sustain;
    double env_release;
    double chorus_I_switch;
    double chorus_II_switch;
} juno_patch;

#define NUM_PATCHES 100
#ifdef __cplusplus
extern "C"
{
#endif
juno_patch* juno_patchset_new();
void juno_patchset_delete();
int save_patches( const char *filename, juno_patch* patches );
int load_patches( const char *filename, juno_patch* patches );
#ifdef __cplusplus
}
#endif
#endif /* JUNO_PATCH_H */



