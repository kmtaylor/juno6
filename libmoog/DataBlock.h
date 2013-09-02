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
 * $Revision: 1.13.4.2 $$Date: 2000/02/24 00:44:03 $
 */
#ifndef _DATABLOCK_H
#define _DATABLOCK_H

#include <stdio.h>                 //for definition of FILE *

class DataBlock
{
    friend class DataManager;
    char *name;

    void init( const char*n, const char* file, size_t size );

 public:

    /* these are public because we don't want to be
     * using accessor functions in tight loops.
     * this class is just a convenience wrapper/mem manager
     * for an array
     */

    double *data;
    size_t  length;

    DataBlock( const char *name );
    DataBlock( const char *name, unsigned int size );
    DataBlock( DataBlock* src );
    ~DataBlock();

     const char* getName();
    void        normalize();
    void        invert();
};

#endif /* _DATABLOCK_H */

