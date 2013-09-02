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
#ifndef _LIBMOOG_IIR2_H
#define _LIBMOOG_IIR2_H

#include "MoogObject.h"

class IIR2 : public MoogObject
{
    double *in;
    Output *output;

 protected:
    double gain;
    double cx[2];
    double cy[2];
    double x[2];
    double y[2];

 public:
    IIR2();

    void connectTo(ConnectionInfo *);
    void disconnectTo(ConnectionInfo *);
    void sampleGo();

    const char *getClassName() { return "IIR2"; }
};

#endif /* _LIBMOOG_IIR2_H */
