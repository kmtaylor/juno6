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
 * Copyright (c) 1998 Cobite, Inc. All Rights Reserved.
 * @author Karl LaRocca
 * @created Fri Nov  6 14:48:04 1998
 * @version $Revision: 1.3.4.1 $$Date: 2000/02/24 00:44:09 $
 */
#ifndef _TEXT_UTIL_H
#define _TEXT_UTIL_H

#ifdef __cplusplus
extern "C"
{
#endif

char*       chop( char* src );
char*       digits( char* src );
char*       lower_case( char* src );
char*       reverse( char* src );
char*       trim( char* src );
char*       upper_case( char* src );

const char* cents2money( long cents );
long        money2cents( const char* money );

#ifdef __cplusplus
}
#endif

#endif /* _TEXT_UTIL_H */
