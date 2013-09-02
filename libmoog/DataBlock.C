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
#include <util/debug.h>
#include <util/sio.h>
#include <math.h>
#include <util/fastmemcpy.h>
#include "DataBlock.h"

#define DATABLOCK_BIN_MAGIC     0xcdedcbab
#define DATABLOCK_STRING_MAGIC  "#mdf"
#define MAGIC_SIZE              4

DataBlock::DataBlock( const char *n )
{
    init( n, NULL, 0 );
}


DataBlock::DataBlock( const char *n, unsigned int size )
{
    init( n, NULL, size );
}

void
DataBlock::init( const char* n, const char* file, size_t size )
{
    name = strdup( n );
    data = NULL;


    if ( size )
    {
	length = size;
	data = new double[ size ];
	memset( data, 0, sizeof( data ) );
    }

    else
    {
	length = 0;
	data = NULL;
    }
}

DataBlock::DataBlock( DataBlock* src )
{
   name = strdup( src->getName() );
   length = src->length;
   data = new double[ length ];
   memcpy( data, src->data, length * sizeof( double ) );
}

DataBlock::~DataBlock()
{
    delete name;

    if ( data ) 
	delete data;
}



const char*
DataBlock::getName()
{
   return( name );
}

void
DataBlock::normalize()
{
    double min = 0.0;
    double max = 0.0;
    double  scale;
    double  shift;
    double* ptr;

    ptr = data;
    for( int i = length; i-- != 0; ptr++ )
    {
	if ( *ptr < min )
	{
	    min = *ptr;
	}

	if ( *ptr > max )
	{
	    max = *ptr;
	}
    }

    scale =  2.0 / ( max - min );
    shift = 1.0 - scale * max;

    ptr = data;
    for( int i = length; i-- != 0; ptr++ )
    {
	*ptr = *ptr * scale + shift;
    }
}

void DataBlock::invert()
{
   double* ptr = data;
   for( int i = length; i-- != 0; ptr++ )
   {
       *ptr *= -1.0;
   }
}
