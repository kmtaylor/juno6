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
#include "ConnectionList.h"
#include "ConnectionInfo.h"

ConnectionList::ConnectionList()
{
    head = NULL;
    current = NULL;
}

ConnectionList::~ConnectionList()
{
    ConnectionListNode *tmp;
    
    while (head)
    {
	tmp = head->next;
	delete head;
	head = tmp;
    }
}

void ConnectionList::addConnection(ConnectionInfo *connection)
{
    ConnectionListNode *newNode = new ConnectionListNode;
    newNode->connection = connection;
    newNode->next = head;
    head = newNode;
    
    printList();
}

ConnectionInfo *ConnectionList::removeConnection(ConnectionInfo *connection)
{
    ConnectionListNode *tmp, *ptr;
    ConnectionInfo     *retval = NULL;

    /* special case if connection is on head node */
    if (head && head->connection == connection)
    {
	tmp = head->next;
	delete head;
	head = tmp;
	retval = connection;
    }

    /* otherwise scan the list*/
    else if (head)
    {
	ptr = head;
	while (ptr->next)
	{
	    if (ptr->next->connection == connection)
	    {
		tmp = ptr->next;
		ptr->next = ptr->next->next;
		delete tmp;
		retval = connection;
		break;
	    }
	    ptr = ptr->next;
	}
    }

    printList();

    return retval;
}

void ConnectionList::printList()
{
    for (ConnectionListNode *ptr = head; ptr; ptr = ptr->next)
	ptr->connection->print();
}
