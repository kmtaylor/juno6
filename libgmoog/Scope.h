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
 * $Revision: 1.3.4.2 $$Date: 2000/02/25 20:40:43 $
 */
#ifndef SCOPE_H
#define SCOPE_H

#include <moog/MoogObject.h>
#include "scope2.h"

class Scope : public MoogObject
{
public:
    GtkScope* widget;
    GtkWidget* mainWindow;
    int showing;

    int scale;
    int scalepos;

    Scope();
    void sampleGo();
    void popup();
    void popdown();
    void toggle();
    void setScale( int scale );

    const char *getClassName() { return "Scope"; }
};

#endif /* SCOPE_H */
