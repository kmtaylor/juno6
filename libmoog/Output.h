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
 * $Revision: 1.18.4.1 $$Date: 2000/02/24 00:44:04 $
 */
#ifndef _OUTPUT_H
#define _OUTPUT_H

#include <util/SimpleArray.h>
#include "ConnectionList.h"
#include "NamedObject.h"
#include "Input.h"
#include "ConnectionInfo.h"

class Output : public NamedObject
{
    ConnectionList  connections;
    bool            hasEventInputs;
    bool            continuousOutput;

 public:     
    Output(const char * name, bool );

    void            connect(ConnectionInfo*);
    void            disconnect(ConnectionInfo*);

    double         *getData()        { return &data; }

    double          data;
    
    ConnectionList *getConnections() { return &connections; }

    void setData( double _data )
    {
	data = _data;

	if ( hasEventInputs )
	{
	    ConnectionInfo *info;
	    
	    //FIXME: is this thread safe???
	    connections.resetIterator();
	    while ((info = connections.getNextConnection()))
	    {
		if (!continuousOutput || 
		    ++info->input->currentTimeout == info->input->timescale)
		{
		    info->input->doCallback( data );
		}
	    }
	}
    }

//#ifdef DEBUG
    void print();
//#endif
};

typedef SimpleArray<Output> OutputArray;

#endif /* _OUTPUT_H */
