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
#ifndef _SIO_H
#define _SIO_H

/* include for typedefs */
#ifdef WIN32
#define ssize_t long
#define size_t  unsigned long
#include <winsock2.h>
#include <io.h>
#else
#include <unistd.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif
/* these are W.R.Stevens' famous io routines to read or write bytes to fd */
ssize_t readn(int, void *, size_t);
ssize_t writen(int, const void *, size_t);

#ifdef __cplusplus
}
#endif

#endif /* _SIO_H */
