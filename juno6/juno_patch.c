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
#include <sys/stat.h>
#include <util/sio.h>
#include <util/debug.h>
#include <stdlib.h>
#include <string.h>

#include "juno_patch.h"

void
init_patch( juno_patch* patch )
{
    memset( patch, 0, sizeof( juno_patch ) );
    patch->version = 1;
    patch->octave_transpose = 2;
}

juno_patch* juno_patchset_new()
{
    int i;
    juno_patch* retval = (juno_patch*)malloc(NUM_PATCHES*sizeof(juno_patch));
    for( i = 0; i < NUM_PATCHES; i++ )
    {
	init_patch(&retval[i]);
    }
    return( retval );
}

void juno_patchset_delete( juno_patch* patches )
{
    free( patches );
}

int save_patches(const char *filename, juno_patch* patches)
{
   
    unsigned int nbytes;
    FILE *out;
    
    out=fopen(filename, "wb");

    if (out==NULL)
    {
	debug(DEBUG_SYSERROR, "can't open patch file %s for write", filename);
	return -1;
    }

    nbytes = sizeof(juno_patch) * NUM_PATCHES;
    if (fwrite(patches,1, nbytes,out) != nbytes)
    {
	debug(DEBUG_SYSERROR, "short write on patch file %s", filename);
	fclose(out);
	return -1;
    }

    fclose(out);
    return 0;
}

int load_patches(const char *filename, juno_patch* patches)
{
    
    unsigned int nbytes;
    int i;
    unsigned int ret;
    FILE *in;
    in = fopen(filename, "rb");
    if (in==NULL)
    {
	for( i = 0; i < NUM_PATCHES; i++ )
	{
	    init_patch( &patches[i] );
	}

	debug(DEBUG_SYSERROR, "can't open patch file %s for read", filename);
	return -1;
    }

    nbytes = sizeof(juno_patch) * NUM_PATCHES;
    memset(patches, 0, sizeof(juno_patch) * NUM_PATCHES);
    
    ret = fread(patches,1,nbytes,in);
   // printf("%d,%d\n",ret,nbytes);
   if (ret!=nbytes)
    {
	memset(patches, 0, sizeof(juno_patch) * NUM_PATCHES);
	
	debug(DEBUG_SYSERROR, "short read on patch file %s", filename);
	fclose(in);
	return -1;
    }

   fclose(in);
    return 0;
}
