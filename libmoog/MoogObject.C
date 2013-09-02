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
#include <stdarg.h>
#include <string.h>
#include <util/debug.h>

#include "MoogObject.h"
#include "ConnectionInfo.h"
#include "ConnectionList.h"
#include "ConnectionManager.h"

MoogObject::MoogObject()
{
#ifdef LIBMOOG_DEBUG
    debug_flag = false;
#endif

    contextTag = NULL;

    /* create a default unique name for every object */
    name = new char[strlen("MoogObject") + 10];
    sprintf(name, "MoogObject_%d", goHandle);
}

void MoogObject::addPorts(const char *name, ...)
{
    va_list va;
    int io;
    moog_callback_t callback;
    long userData = 0;
    int  timescale = 0;
   // printf("adding port %s\n",name);
    va_start(va, name);
   // puts("start ready");
    while(name)
    {
 	//puts("get io");
	io = va_arg(va, int);
        
	if (io == OUTPUT)
	{
	
	    bool continuousOutput = va_arg(va, int);
        
	    addOutput(name, continuousOutput);
	}
	else
	{   
          
	    callback = va_arg( va, moog_callback_t);
             
	    if ( callback != NULL )
	    {   
            
		userData = va_arg( va, long );
		
		timescale = va_arg( va, int );
	    }
           // printf("add input %s to %s\n",name,getName());
	    addInput(name, callback, userData, timescale);
	}

	name = va_arg(va, char *);
    }
    va_end(va);
}

Output* MoogObject::addOutput(const char *name, bool continuousOutput)
{
 //printf("add output %s to %s %s\n",name,getName(),getClassName());

    debug(DEBUG_STATUS, "addOutput: '%s' to %s::%s.", name, getName(), getClassName());

    Output* retval = new Output( name, continuousOutput );
    outputs.appendElement( retval );
    return( retval );
}

Input* MoogObject::addInput(const char *name, moog_callback_t callback, long userData, int timescale)
{
//printf("add input %s to %s %s\n",name,getName(),getClassName());

    debug(DEBUG_STATUS, "addInput: '%s' to %s::%s.", 
	  name, getName(), getClassName());

    Input* retval = new Input( name, callback, this, userData, timescale );
    inputs.appendElement( retval );
    return( retval );
}

ConnectionManager *MoogObject::getContextTag()
{
    return contextTag;
}

void MoogObject::setContextTag(ConnectionManager *t)
{
    contextTag = t;
}

void MoogObject::connectFrom(ConnectionInfo *info)
{
    info->output->connect(info);
}

void MoogObject::connectTo(ConnectionInfo *info)
{
    info->input->connect(info);
}

void MoogObject::disconnectFrom(ConnectionInfo *info)
{
   info->output->disconnect(info);
}

void MoogObject::disconnectTo(ConnectionInfo *info)
{
    info->input->disconnect(info);
}

void MoogObject::set( int inputNum, double value )
{
    inputs[inputNum].setStaticValue( value );
} 

void MoogObject::set( const char* inputName, double value )
{
    inputs[ getInputNum( inputName ) ].setStaticValue( value );
} 

// These next two may want to be faster than a linear scan at some point
int MoogObject::getInputNum( const char* name )
{
    for( int i = 0; i < inputs.getSize(); i++ )
    {
	if ( strcmp( inputs[ i ].getName(), name ) == 0 )
	    return( i );
    }

    debug( DEBUG_APPERROR, "%s(%s): no input named '%s'", getName(), getClassName(), name );
    return( -1 );
}

int MoogObject::getOutputNum( const char* name )
{
    for( int i = 0; i < outputs.getSize(); i++ )
    {
	if ( strcmp( outputs[ i ].getName(), name ) == 0 )
	    return( i );
    }

    //FIXME: this is more than 'STATUS' but we need it to shut up right now...
    debug( DEBUG_STATUS, "%s(%s): no output named '%s'", 
	   getName(), getClassName(), name );
    return( -1 );
}

Input* MoogObject::getInput( const char* name )
{
    int num = getInputNum( name );
    if ( num == -1 )
	return( NULL );

    return( &inputs[ num ] );
}

Output* MoogObject::getOutput( const char* name )
{
    int num = getOutputNum( name );
    if ( num == -1 )
	return( NULL );

    return( &outputs[ num ] );
}

