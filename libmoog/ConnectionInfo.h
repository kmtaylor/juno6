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
 * $Revision: 1.15.4.1 $$Date: 2000/02/24 00:44:03 $
 */
#ifndef _CONNECTIONINFO_H
#define _CONNECTIONINFO_H

#include <stdio.h>
#include <util/debug.h>

class Input;
class Output;
class MoogObject;

class ConnectionInfo
{
 public:

    MoogObject *from;
    Output     *output;
    
    MoogObject *to;
    Input      *input;
    
    ConnectionInfo(MoogObject *f, Output *o, MoogObject *t, Input *i)
	:
	from(f),
	output(o),
	to(t),
	input(i)
	{
	    print();
	}
    
    int operator ==(const ConnectionInfo &other)
	{
	    return (from   == other.from   &&
		    output == other.output &&
		    to     == other.to     &&
		    input  == other.input);
	}
    
    void print();
};

#endif /*_CONNECTIONINFO_H*/

