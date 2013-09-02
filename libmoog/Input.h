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
 * $Revision: 1.18.4.1 $$Date: 2000/02/24 00:44:03 $
 */
#ifndef _INPUT_H
#define _INPUT_H

class ConnectionInfo;
class MoogObject;

#include <util/SimpleArray.h>
#include "NamedObject.h"

typedef void (*moog_callback_t)(MoogObject*, double, long);

class Input : public NamedObject
{
    friend class MoogObject;
    friend class Output;

    ConnectionInfo *connection;
    double          staticValue;
    moog_callback_t callback;
    MoogObject*     registrant;
    long            userData;
    int             timescale;
    int             currentTimeout;

    void doCallback( double data )
    {
	if ( callback != NULL )
	{
	    callback( registrant, data, userData );
	    currentTimeout = 0;
	}
    }

 public:
    double         *data;

    Input(const char *, moog_callback_t, MoogObject*, long, int );

    void            setStaticValue(double);

    void            connect(ConnectionInfo *);
    void            disconnect(ConnectionInfo *);

    ConnectionInfo *getConnection() { return connection; } 

    
#ifdef DEBUG
    void print();
#endif

};

typedef SimpleArray<Input> InputArray;

#endif /* _INPUT_H */
