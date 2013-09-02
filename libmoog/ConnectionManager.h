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
#ifndef _ConnectionManager_H
#define _ConnectionManager_H

class ConnectionInfo;
class MoogObject;

#define PATCH( a, b, c, d ) ConnectionManager::connect( a, b, c, d )

class ConnectionManager
{
 protected:
    static int              reconnect;

 public:
    ConnectionManager() { reconnect = 0; }

    /* the general interface: calls a hook connect/disc. or the base impl */
    static ConnectionInfo *connect(MoogObject *f, int o, MoogObject *t, int i);
    static ConnectionInfo *connect(MoogObject *f, const char*, MoogObject *t, const char*);
    static void            disconnect(ConnectionInfo *);

 protected:
    /* called by connect/disconnect or by a ConnectionManager impl.
     * ll is for low-level. this does the actual patching of inputs 
     * to outputs and allocates/deallocates the ConnectionInfo class
     */
    static ConnectionInfo *llConnect(MoogObject *, int, MoogObject *, int);
    static ConnectionInfo *llConnect(MoogObject *, const char*, MoogObject *, const char*);
    static void            llDisconnect(ConnectionInfo *);

    virtual ConnectionInfo *connectImpl(MoogObject *from, 
					int onum, 
					MoogObject *to, 
					int inum) = 0;
    virtual ConnectionInfo *connectImpl(MoogObject *from, 
					const char* oname, 
					MoogObject *to, 
					const char* iname) = 0;
    virtual void    disconnectImpl(ConnectionInfo *info) = 0;

    static int isReconnecting() { return reconnect; }
};

#endif /* _ConnectionManager_H */

