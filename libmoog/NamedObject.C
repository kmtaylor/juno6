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
#include <stdlib.h>
#include <string.h>

#include "NamedObject.h"

NamedObject::NamedObject()
{
    name = NULL;
}

NamedObject::NamedObject( const char* n )
{
    name = NULL;
    setName( n );
}

NamedObject::~NamedObject()
{
    if ( name )
	delete( name );
}

void
NamedObject::setName( const char* n )
{
    if ( name )
	delete( name );

    if ( n == NULL )
	name = NULL;
    else
    {
	int len = strlen(n);
	name = new char[len + 1];
	memcpy(name, n, len);
	name[len] = 0;
    }
}

