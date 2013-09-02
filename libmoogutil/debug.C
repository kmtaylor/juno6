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
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>

#include "debug.h"
#include "rcsid.h"

//RCSID("$Id: debug.c,v 1.11.2.1 2000/02/24 00:44:09 larocca Exp $");

int debuglvl = ~0;
static FILE *debug_error = NULL;

void vdebug(int dtype, const char *fmt, va_list ap)
{
  int  keep_errno;   
  char msgbuff[1024];

  /* errno could be changed by vsprintf or perror */
  keep_errno = errno;

  if (debuglvl & dtype)
  {
    vsprintf(msgbuff, fmt, ap);
    
    if (dtype == DEBUG_ERROR)
      perror(msgbuff);
    else
      fprintf((debug_error) ? debug_error : stderr, "%s\n", msgbuff);

    fflush((debug_error) ? debug_error : stderr);
  }

  errno = keep_errno;
}

#ifndef NODEBUG

void debug(int dtype, const char *fmt, ...)
{

  va_list ap;
  va_start(ap, fmt);
  vdebug(dtype, fmt, ap);
  va_end(ap);
}
#endif
void hexdump(const char *ptr, int size, const char *fmt, ...) 
{
  static char hexbuff[49];
  static char printbuff[17];
  int count = 0;
  va_list ap;

  if ( !debuglvl & DEBUG_STATUS )
    return;

  va_start(ap, fmt);
  
  vfprintf((debug_error) ? debug_error : stderr, fmt, ap);
  fprintf((debug_error) ? debug_error : stderr, "\n");

  memset(hexbuff, 0, 49);
  memset(printbuff, 0, 17);
  
  while (size--) 
  {
    sprintf(hexbuff + (count*3), "%02x ", (int)*((unsigned char *)ptr));

    if (isprint(*ptr))
      printbuff[count] = *ptr;
    else
      printbuff[count] = '.';

    ptr++;

    if ( count++ == 15 ) 
    {
      count = 0;
      fprintf((debug_error) ? debug_error : stderr, 
	      "%s %s\n", hexbuff, printbuff);
      memset(hexbuff, 0, 49);
      memset(printbuff, 0, 17);
    }
  }

  if ( count > 0 ) {
    while ( count % 16 != 0 ) {
      sprintf(hexbuff + (count * 3), "xx ");
      printbuff[count++] = '.';
    }
    fprintf((debug_error) ? debug_error : stderr,  "%s %s\n", hexbuff, printbuff);
  }

  va_end(ap);
}

void
to_hex( char* dest, const char* src, size_t n )
{
   while ( n-- ) 
   {
      sprintf( dest, "%02x ", (int)*((unsigned char *)src));
      dest += 3;
      src++;
   }

   *dest = 0;
}

void debug_set_error_file(FILE *f)
{
    debug_error = f;
}
