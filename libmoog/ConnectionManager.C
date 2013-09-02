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
#include "ConnectionManager.h"
#include "MoogObject.h"
#include "ConnectionInfo.h"
#include <util/debug.h>

int ConnectionManager::reconnect  = 0;

#define ASSERT_CONTEXTS(x,y,z)/* \
do {\
    if ((x)->contextTag != (y)->contextTag)\
    {\
	debug(DEBUG_APPERROR, \
		"cannot do connect on objects from different contexts");\
	return z;\
    }\
} while (0)
*/

ConnectionInfo *
ConnectionManager::connect(MoogObject *from, const char* oname, 
			   MoogObject *to, const char* iname)
{
    ASSERT_CONTEXTS(from, to, NULL);

//    printf("ConnectionManager::connect from:%s::%s(%s) to:%s::%s(%s)",
//	   from->getName(), from->getClassName(), oname, to->getName(), to->getClassName(), iname);
    
    /* we can assume the same context for both 'from' and 'to' so use 'to' */
    if (to->contextTag)
	return to->contextTag->connectImpl(from, oname, to, iname);
    else
	return llConnect(from, oname, to, iname);

   
}

ConnectionInfo *
ConnectionManager::connect(MoogObject *from, int onum, MoogObject *to, int inum)
{
    ASSERT_CONTEXTS(from, to, NULL);
    debug( DEBUG_STATUS,"ConnectionManager::connect from:%s::%s(%d) to:%s::%s(%d)",
	   from->getName(), from->getClassName(), onum, to->getName(), to->getClassName(), inum);
    
    /* we can assume the same context for both 'from' and 'to' so use 'to' */
    if (to->contextTag)
	return to->contextTag->connectImpl(from, onum, to, inum);
    else
	return llConnect(from, onum, to, inum);
}

ConnectionInfo *
ConnectionManager::llConnect(MoogObject *from, const char* oname,
			     MoogObject *to, const char* iname)
{
    /* if the to->inum is already connected, force a disconnect first
     * set the reconnect flag in case someone needs to know...
     */
  // puts("conn:getinput");
    Input* input = to->getInput(iname);
    if (input!=NULL)
    {
//   puts("not null");
    if (input->getConnection())
    {
  //  puts("disconnect before reconnect");
	reconnect = 1;
	disconnect(to->getInput(iname)->getConnection());
	reconnect = 0;
    }
 //  puts("new connectioninfo");

    ConnectionInfo *retval = new ConnectionInfo(from,
						from->getOutput(oname),
     						to,
    						to->getInput(iname));
//    printf("connect %s\n",from->getClassName());
     from->connectFrom(retval);
  //  printf("to %s\n",to->getClassName());
    to->connectTo(retval);
//    puts("ready");
    return retval;
    }
//puts("was null, no connection was made");
return NULL;
}

ConnectionInfo *
ConnectionManager::llConnect(MoogObject *from,int onum,MoogObject *to,int inum)
{
    /* if the to->inum is already connected, force a disconnect first
     * set the reconnect flag in case someone needs to know...
     */
    if (to->getInput(inum)->getConnection())
    {
	reconnect = 1;
	disconnect(to->getInput(inum)->getConnection());
	reconnect = 0;
    }

    ConnectionInfo *retval = new ConnectionInfo(from,
						from->getOutput(onum),
						to,
						to->getInput(inum));
    from->connectFrom(retval);
    to->connectTo(retval);

    return retval;
}

void 
ConnectionManager::disconnect(ConnectionInfo *connection)
{
    ASSERT_CONTEXTS(connection->from, connection->to,);

    debug(DEBUG_STATUS, "ConnectionManager::disconnect()");

    if (connection->to->contextTag)
	connection->to->contextTag->disconnectImpl(connection);
    else
	llDisconnect(connection);
}

void 
ConnectionManager::llDisconnect(ConnectionInfo *connection)
{
    connection->from->disconnectFrom(connection);
    connection->to->disconnectTo(connection);

    delete connection;
}
