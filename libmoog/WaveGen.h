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
 * $Revision: 1.8.4.1 $$Date: 2000/02/24 00:44:04 $
 */
#ifndef WAVEGEN_H
#define WAVEGEN_H

#include "DataBlock.h"

DataBlock* wg_line( int size, double start, double end );
DataBlock* wg_lines( int size, int nsegs, double initial, ... );
DataBlock* wg_sin( int size, int ncoefs, ... );
DataBlock* wg_cos( int size, int ncoefs, ... );
DataBlock* wg_noise( int size );
DataBlock* wg_saw( int size );
DataBlock* wg_tri( int size );
DataBlock* wg_pow( int size, double min, double max );
DataBlock* wg_bell( int size, double sharpness );

#endif /* WAVEGEN_H */
