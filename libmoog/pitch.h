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
 * $Revision: 1.6.4.1 $$Date: 2000/02/24 00:44:04 $
 */
#ifndef PITCH_H
#define PITCH_H

extern double midi_notes[];

#include <math.h>

inline int pitch_to_note(double pitch)
{
    return (int)rint(log(pitch/midi_notes[0])/log(2.0)*12);
}

#endif /* PITCH_H */
