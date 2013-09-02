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

#include "juno_control.h"

JunoControl::JunoControl(int voices)
{
for (int i=0;i<voices;i++)
{
    addOutput( (String)"voice"+i+(String)"_pitch", false );
    addOutput( (String)"voice"+i+(String)"_gate", false );

   /* addOutput( "voice1_pitch", false );
    addOutput( "voice1_gate", false );

    addOutput( "voice2_pitch", false );
    addOutput( "voice2_gate", false );

    addOutput( "voice3_pitch", false );
    addOutput( "voice3_gate", false );

    addOutput( "voice4_pitch" , false);
    addOutput( "voice4_gate", false );

    addOutput( "voice5_pitch", false );
    addOutput( "voice5_gate", false );*/
}
    addOutput( "bender", false );
    addOutput( "bender_dco", false );
    addOutput( "bender_vcf", false );
    addOutput( "lfo_trigger" , false );
    addOutput( "volume", false );
    addOutput( "octave_transpose", false );

    addOutput( "master_tune", false );

    addOutput( "transpose_switch", false ); // 0 = normal, non zero = transpose
    addOutput( "hold_switch", false ); // 0 = normal, non zero = hold

    addOutput( "arpeggio_switch", false ); // 0 = off, non zero = on
    addOutput( "arpeggio_mode", false ); // -1 = down, 0 = up and down, 1 = up
    addOutput( "arpeggio_range", false ); // -1 = 3, 0 = 2, 1 = 1
    addOutput( "arpeggio_rate", false ); 

    addOutput( "lfo_rate", false );
    addOutput( "lfo_delay", false );
    addOutput( "lfo_mode", false ); // 0 = auto, non zero = manual

    addOutput( "dco_lfo", false );
    addOutput( "dco_pwm", false );
    addOutput( "dco_pwm_mod", false ); // -1 = env, 0 = manual, 1 = lfo

    addOutput( "dco_pulse_switch", false );
    addOutput( "dco_saw_switch", false );
    addOutput( "dco_sub_switch", false );
    addOutput( "dco_sub", false );
    addOutput( "dco_noise", false );

    addOutput( "hpf_frq", false );

    addOutput( "vcf_frq", false );
    addOutput( "vcf_res", false );

    addOutput( "vcf_env_invert", false ); // 0 = normal, non zero = inverted

    addOutput( "vcf_env", false );
    addOutput( "vcf_lfo", false );
    addOutput( "vcf_kbd", false );

    addOutput( "vca_mode", false ); // 0 == env, 1 == gate 

    addOutput( "env_attack", false );
    addOutput( "env_decay", false );
    addOutput( "env_sustain", false );
    addOutput( "env_release", false );

    addOutput( "chorus_off_switch", false );
    addOutput( "chorus_I_switch", false );
    addOutput( "chorus_II_switch", false );
}
