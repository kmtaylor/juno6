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

#ifdef WIN32
#else
#include <unistd.h>
#endif

#include <errno.h>

#include "sio.h"
#include "rcsid.h"

//RCSID("$Id: sio.c,v 1.4.4.1 2000/02/24 00:44:09 larocca Exp $");

ssize_t readn(int fd, void *buf, size_t len)
{

  size_t nleft,nread;

  nleft = len;

  while (nleft > 0)
  {
    nread = read(fd,buf,nleft);

    /* there is an issue which EINTR which could leave us a bit haywire
     * if we get a signal after having read some bytes. special handling
     * N.B: we *do* return EINTR if no data has been read yet (thanks Karl)
     */
    if (nread < 0)
    {
      if (errno == EINTR && nleft != len)
        continue;
      else
	    return (nread);
    }
    else if (nread == 0)
      break;

    nleft -= nread;

    if (nleft)
      buf = ((char *)buf) + nread;
  }
  return (len - nleft);
}

ssize_t writen(int fd, const void *buf, size_t len)
{
  
  size_t nleft, nwritten;

  nleft = len;

  while (nleft > 0)
  {
    nwritten = write(fd,buf,nleft);

    /* there is an issue with EINTR if we have already written
       a few bytes! return if we have not written any yet */
    if (nwritten < 0 && errno == EINTR)
    {
      if (nleft == len)
	return nwritten;
      
      continue;
    }
    

    if (nwritten <= 0)
      return nwritten;

    nleft -= nwritten;

    if (nleft)
      buf = ((char *)buf) + nwritten;
  }
  
  return (len - nleft);
}

