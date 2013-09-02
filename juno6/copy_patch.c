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
#include <errno.h>
#include <stdlib.h>

int
main( int argc, char** argv )
{
    char* srcFile;
    int srcNo;
    char* destFile;
    int destNo;

    juno_patch* srcPatches;
    juno_patch* destPatches;

    if ( argc < 4 )
    {
	fprintf( stderr, "Usage: %s srcFile srcNo destFile destNo\n", argv[0] );
	exit(1);
    }

    srcFile  = argv[1];
    srcNo    = atoi(argv[2]);
    destFile = argv[3];
    destNo   = atoi(argv[4]);

    srcPatches  = juno_patchset_new();
    destPatches = juno_patchset_new();

    if ( load_patches( srcFile, srcPatches ) < 0 )
    {
	exit(1);
    }

    if ( load_patches( destFile, destPatches ) < 0 )
    {
	exit(1);
    }

    destPatches[ destNo ] = srcPatches[ srcNo ];

    save_patches( destFile, destPatches );

    juno_patchset_delete( srcPatches );
    juno_patchset_delete( destPatches );

    return( 0 );
}
