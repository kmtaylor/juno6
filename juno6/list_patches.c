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
 * $Revision: 1.1.4.1 $$Date: 2000/02/24 00:43:48 $
 */
#include "juno_patch.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int
main( int argc, char** argv )
{
    int i;
    juno_patch* patches;

    if ( argc < 2 )
    {
	fprintf( stderr, "Usage: %s patchfile\n", argv[0] );
	exit(1);
    }

    patches = juno_patchset_new();
    if ( load_patches( argv[1], patches ) < 0 )
    {
	exit(1);
    }
    
    for( i = 0; i < NUM_PATCHES; i++ )
    {
	printf( "%02d %s\t", i, patches[ i ].used ? "used" : "free" );
	if ( i % 10 == 10 )
	    printf( "\n" );
    }

    juno_patchset_delete( patches );

    return( 0 );
}
