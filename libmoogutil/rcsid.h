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
#ifndef _COMMON_RCSID_H
#define _COMMON_RCSID_H

/* RCS Id macro (complements of bod@compusol.com.au (Brendan O'Dea)) */
#ifdef lint
# define RCSID(i)
#else /* lint */
# ifdef __GNUC__
#  define ATTRIB_UNUSED __attribute__ ((unused))
# else /* __GNUC__ */
#  define ATTRIB_UNUSED
# endif /* __GNUC__ */
# define RCSID(i) static char const *rcsid ATTRIB_UNUSED = (i)
#endif /* lint */

#endif /* _COMMON_RCSID_H */
