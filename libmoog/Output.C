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

#include "Output.h"
#include "ConnectionInfo.h"

Output::Output(const char *n, bool _continuousOutput) : NamedObject(n)
{
    data   = 0;
    hasEventInputs = false;
    continuousOutput = _continuousOutput;
}

void Output::connect(ConnectionInfo *info)
{
    connections.addConnection(info);
    if ( info->input->callback != NULL )
	hasEventInputs = true;
}

void Output::disconnect(ConnectionInfo *info)
{
    connections.removeConnection(info);

    // note we reuse info variable
    if ( hasEventInputs )
    {
	hasEventInputs = false;
	connections.resetIterator();
	while ((info = connections.getNextConnection()))
	{
	    if ( info->input->callback != NULL )
	    {
		hasEventInputs = true;
		break;
	    }
	}
    }

}

//#ifdef DEBUG
void Output::print()
{
    debug(DEBUG_STATUS, "%s", name);
}
//#endif
