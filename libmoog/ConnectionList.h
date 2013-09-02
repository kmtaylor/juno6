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
#ifndef _CONNECTION_LIST
#define _CONNECTION_LIST

class ConnectionInfo;

class ConnectionListNode
{
  friend class ConnectionList;

  ConnectionInfo     *connection;
  ConnectionListNode *next;
};

class ConnectionList 
{
  ConnectionListNode *head;
  ConnectionListNode *current;

 public:
  ConnectionList();
  ~ConnectionList();
  void                addConnection(ConnectionInfo *);
  ConnectionInfo     *removeConnection(ConnectionInfo *);

  void                resetIterator() 
  { 
      current = head; 
  }

  ConnectionInfo     *getNextConnection()
  {
      ConnectionInfo *retval = NULL;
      if (current) 
      {
	  retval = current->connection;
	  current = current->next;
      }
      return retval;
  }


  void printList();

};

#endif /*_CONNECTION_LIST*/
