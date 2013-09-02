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
#ifndef _DEBUG_H
#define _DEBUG_H

#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>

#define DEBUG_SYSERROR  1  /* same as DEBUG_ERROR, but here for clarity */
#define DEBUG_ERROR     1
#define DEBUG_STATUS    2
#define DEBUG_TCP       4
#define DEBUG_SIGNALS   8
#define DEBUG_APPERROR  16
#define DEBUG_APPMSG1   32
#define DEBUG_APPMSG2   64
#define DEBUG_APPMSG3   128
#define DEBUG_APPMSG4   256
#define DEBUG_APPMSG5   512
#define DEBUG_LIBERROR  1024
#define DEBUG_LIBSTATUS 2048

#ifdef __cplusplus
extern "C" 
{
#endif

#ifdef NODEBUG
#define debug(a,b,...)
#else
void debug( int dtype, const char *fmt, ... );
#endif
void hexdump( const char *ptr, int size, const char *fmt, ... );
void vdebug(int dtype, const char *fmt, va_list);
void to_hex( char* dest, const char* src, size_t n );
void debug_set_error_file(FILE *);

#ifdef __cplusplus
}
#endif

extern int debuglvl;

#endif /* DEBUG_H */
