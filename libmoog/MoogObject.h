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
 * $Revision: 1.37.4.1 $$Date: 2000/02/24 00:44:03 $
 */
#ifndef _MOOGOBJECT_H
#define _MOOGOBJECT_H

#include "GoObject.h"
#include "Input.h"
#include "Output.h"
#include "NamedObject.h"
#include "ConnectionManager.h"

/* some defines for addPorts */
#define INPUT  1
#define OUTPUT 0

class Chorus;
class ConnectionInfo;
class ConnectionList;
class Control;
class MoogObject;

typedef SimpleArray<Control> ControlArray;

#define LIBMOOG_DEBUG
#ifdef LIBMOOG_DEBUG
#define MOOG_DEBUG( fmt, args... ) if ( debug_flag ){ fprintf( stderr, "%s: ", getClassName() ); fprintf( stderr, fmt, ## args ); fprintf( stderr, "\n" );}
#else
#define MOOG_DEBUG( args... )
#endif

class MoogObject : public GoObject, NamedObject 
{
    //FIXME: we really shouldn't need these
    // but it looks like every composite class will need to be here
    friend class Chorus;
    friend class Container;
    friend class ConnectionManager;
    friend class SamplePool;
    friend class Delay;
    friend class Balance;
    friend class Reverb;

 protected:
    InputArray       inputs;
    OutputArray      outputs;

    ConnectionManager *contextTag;

    /* allow runtime configuration of objects */
    void             addPorts(const char *, ...);
    Output*          addOutput(const char *, bool);
/*    Input*           addInput(const char *name, int isCtrl){
	puts("should never be called");
	printf("from %s\n",getClassName());
	return NULL;
	}*/
    Input*           addInput(const char *name, moog_callback_t callback = NULL, long userData = 0, int timescale = 0);

    /* Every connection  has two MoogObjects involved: from->output  to->input
     * These MoogObject base impl. functions take these two objects then call
     * the appropriate function in the From object or To object.
     */
    virtual void     connectFrom(ConnectionInfo *);
    virtual void     connectTo(ConnectionInfo *);
    virtual void     disconnectFrom(ConnectionInfo *);
    virtual void     disconnectTo(ConnectionInfo *);
    
 public:
#ifdef LIBMOOG_DEBUG
    bool debug_flag;
#endif

    MoogObject();
    virtual ~MoogObject(){}

    /* each class should override this for RTTI */
    virtual const char *getClassName() = 0;

    virtual Input   *getInput(int n)        {return &(inputs[n]);}
    virtual Output  *getOutput(int n)       {return &(outputs[n]);} 
    virtual Input   *getInput(const char* n);      
    virtual Output  *getOutput(const char* n);

    int              getNumInputs()         {return inputs.getSize(); }
    int              getNumOutputs()        {return outputs.getSize(); }
    const char      *getInputName(int n)    {return inputs[n].getName();} 
    const char      *getOutputName(int n)   {return outputs[n].getName();}

    /* get/set the context which handles the connect/disc. hooks. */
    ConnectionManager *getContextTag();
    void             setContextTag(ConnectionManager *);

    /* set the static input value of one Input */
    void             set(int inputNum, double value);
    void             set(const char* inputName, double value);

    int              getInputNum( const char* name );
    int              getOutputNum( const char* name );

    virtual void     start() {}
    virtual void     stop()  {}
};

#endif /* _MOOGOBJECT_H */


