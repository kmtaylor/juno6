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
 * $Revision: 1.4.4.1 $$Date: 2000/02/24 00:43:48 $
 */
#include "juno_chorus.h"

void JunoChorus_offChanged(MoogObject *o, double data, long userData)
{
    ((JunoChorus*)o)->offChanged(data);
}

void JunoChorus_IChanged(MoogObject *o, double data, long userData)
{
    ((JunoChorus*)o)->IChanged(data);
}

void JunoChorus_IIChanged(MoogObject *o, double data, long userData)
{
    ((JunoChorus*)o)->IIChanged(data);
}

JunoChorus::JunoChorus( MoogObject* object, const char* outputName, int phase ) : m(3)
{
    addInput( "off", JunoChorus_offChanged, 0, 1 );
    addInput( "I", JunoChorus_IChanged, 0, 1 );
    addInput( "II", JunoChorus_IIChanged, 0, 1 );

    outputs.appendElement( m.getOutput( "sig" ) );

    PATCH( object, outputName, &c1, "sig" );
    PATCH( object, outputName, &c2, "sig" );

    PATCH( object, outputName, &m, "sig0" );
    PATCH( &c1, "sig", &m, "sig1" );
    PATCH( &c2, "sig", &m, "sig2" );

    c1.set( "amp", phase ? -.025 : .025 );
    c1.set( "zro",  .5 );
    c1.set( "mix", 1 ); // all wet

    c2.set( "amp", phase ? -.025 : 0.025 );
    c2.set( "zro", .45 );
    c2.set( "mix", 1 ); // all wet

    m.set( "amp0", 1 );

    offChanged( 0 );
}

JunoChorus::~JunoChorus()
{
    outputs.removeElement(0);
}

void JunoChorus::offChanged( double value )
{
    oneOn = false;
    twoOn = false;

    m.set( "amp1", 0 );
    m.set( "amp2", 0 );
}

//FIXME, need special case for I && II
void JunoChorus::IChanged( double value )
{
    if ( value > 0 )
    {
	oneOn = true;
	if ( twoOn )
	{
	    c1.set( "frq", CPS( 10 ) );
	    c2.set( "frq", CPS( 10 ) );
	}
	else
	{
	    c1.set( "frq", CPS( .55 ) );
	    c2.set( "frq", CPS( .55 ) );
	    m.set( "amp1", 1 );
	    m.set( "amp2", 1 );
	}
    }
    else
    {
	oneOn = false;
	if ( twoOn )
	{
	    c1.set( "frq", CPS( 1.5 ) );
	    c2.set( "frq", CPS( 1.5 ) );
	}

	else
	{
	    m.set( "amp1", 0 );
	    m.set( "amp2", 0 );
	}
    }
}

//FIXME, need special case for I && II
void JunoChorus::IIChanged( double value )
{
    if ( value > 0 )
    {
	twoOn = true;
	if (oneOn)
	{
	    c1.set( "frq", CPS( 10 ) );
	    c2.set( "frq", CPS( 10 ) );
	}

	else
	{
	    c1.set( "frq", CPS( 1.5 ) );
	    c2.set( "frq", CPS( 1.5 ) );
	    m.set( "amp1", 1 );
	    m.set( "amp2", 1 );
	}
    }
    else
    {
	twoOn = false;
	if ( oneOn )
	{
	    c1.set( "frq", CPS( .55 ) );
	    c2.set( "frq", CPS( .55 ) );
	}

	else
	{
	    m.set( "amp1", 0 );
	    m.set( "amp2", 0 );
	}
    }
}

