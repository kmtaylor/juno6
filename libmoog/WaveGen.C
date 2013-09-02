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
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "WaveGen.h"
#include "DataBlock.h"

// a single line
DataBlock* 
wg_line( int size, double start, double end )
{
    return( wg_lines( size, 1, start, size, end ) );
}

DataBlock* 
wg_saw( int size )
{
    return( wg_lines( size, 1, -1.0, size, 1.0 ) );
}

DataBlock* 
wg_tri( int size )
{
    return( wg_lines( size, 2, -1.0, size / 2, 1.0, size / 2, -1.0 ) );
}

// connected line segments
DataBlock* 
wg_lines( int size, int nsegs, double initial, ... )
{
   va_list ap;

   int    lengths[ nsegs ];
   double values[ nsegs ];

   va_start( ap, initial );

   for( int n = 0; n < nsegs; n++ )
   {
      lengths[ n ] = va_arg( ap, int );
      values[ n ] = va_arg( ap, double );
   }

   va_end( ap );

   DataBlock* retval = new DataBlock( "", size );
   int index = 0;
   for( int n = 0; n < nsegs; n++ )
   {
      double rise = ( values[ n ] - initial ) / lengths[ n ];

      for( int i = 0; i < lengths[ n ]; i++ )
      {
	 retval->data[ index++ ] = initial + ( i * rise );
      }

      initial = values[ n ];
   }

   return( retval );
}

// weighted sine partials
DataBlock* 
wg_sin( int size, int ncoefs, ... )
{
    va_list ap;
    int a[ ncoefs ];
    va_start( ap, ncoefs );
    int total_weight = 0;
    
    for( int n = 0; n < ncoefs; n++ )
    {
	a[ n ] = va_arg( ap, int );
	total_weight += a[ n ];
    }
    
    va_end( ap );
    double x, y;
    DataBlock* retval = new DataBlock( "", size );
    
    double size_2 = size / 2;
    for( int i = 0; i < size; i++ )
    {
	x = ( i / size_2 ) * M_PI;
	y = 0;
	
	for( int n = 0; n < ncoefs; n++ )
	{
	    y += a[ n ] * sin( ( n+1 ) * x );
	}
	
	retval->data[ i ] = y / total_weight;
    }
    
    return( retval );
}

// weighted cosine partials
DataBlock*
wg_cos( int size, int ncoefs, ... )
{
   va_list ap;
   int a[ ncoefs ];
   va_start( ap, ncoefs );
   int total_weight = 0;
   for( int n = 0; n < ncoefs; n++ )
   {
      a[ n ] = va_arg( ap, int );
      total_weight += a[ n ];
   }
   va_end( ap );
   double x, y;

   DataBlock* retval = new DataBlock( "", size );

   for( int i = 0; i < size; i++ )
   {
      x = ( i / size ) * 2 * M_PI;
      y = 0;

      for( int n = 0; n < ncoefs; n++ )
      {
	 y += a[ n ] * cos( ( n+1 ) * x );
      }

      retval->data[ i ] = y / total_weight;
   }

   return( retval );
}

// white noise
DataBlock* 
wg_noise( int size )
{
    DataBlock* retval = new DataBlock( "", size );
    for( int i = 0; i < size; i++ )
    {
	retval->data[ i ] = ( 2.0 * rand() / RAND_MAX ) - 1.0;
    }
    return( retval );
}

DataBlock* 
wg_pow( int size, double min, double max )
{
    DataBlock* retval = new DataBlock( "", size );
    for( int i = 0; i < size; i++ )
    {
	retval->data[ i ] = pow( (double)i / size, 2 ) * ( max - min ) + min;
    }
    return( retval );
}

DataBlock* 
wg_bell( int size, double sharpness )
{
    DataBlock* retval = new DataBlock( "", size );
    double x;
    for( int i = 0; i < size; i++ )
    {
	x = ( (double)i / size * sharpness * 2 ) - sharpness;
	retval->data[ i ] = 1 / ( 1 + x * x );
    }
    return( retval );
}

