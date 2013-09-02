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
#include "Input.h"
#include "ConnectionInfo.h"
#include "Output.h"

Input::Input(const char *n, moog_callback_t _callback, MoogObject* _registrant, long _userData , int _timescale ) : NamedObject(n)
{
    callback    = _callback;
    registrant  = _registrant;
    userData    = _userData;
    timescale   = _timescale;
    currentTimeout = 0;
    connection  = NULL;
    staticValue = 0;
    data        = &staticValue;
}

void Input::connect(ConnectionInfo *info)
{
    data       = info->output->getData();
    connection = info;

    doCallback(*data);
}

void Input::disconnect(ConnectionInfo * /* unused */)
{
    connection = NULL;
    data       = &staticValue;

    doCallback(staticValue);
}

void Input::setStaticValue(double val)
{
    staticValue = val;
    doCallback(val);
}

#ifdef DEBUG
void Input::print()
{
    debug(DEBUG_STATUS, "%s", name);
}
#endif
