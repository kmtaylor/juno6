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
 * $Revision: 1.3.4.1 $$Date: 2000/02/24 00:43:48 $
 */
#ifndef JUNO_CHORUS_H
#define JUNO_CHORUS_H

#include <moog/moog.h>

class JunoChorus : public MoogObject
{
 private:
    friend void JunoChorus_offChanged(MoogObject*, double, long);
    friend void JunoChorus_IChanged(MoogObject*, double, long);
    friend void JunoChorus_IIChanged(MoogObject*, double, long);

    void offChanged( double value );
    void IChanged( double value );
    void IIChanged( double value );

    Chorus c1;
    Chorus c2;
    Mixer  m;

    bool oneOn, twoOn;

 public:
    JunoChorus( MoogObject* src, const char* outputName, int phase );
    ~JunoChorus();
    const char* getClassName(){ return( "JunoChorus" ); }
};

#endif /* JUNO_CHORUS_H */
