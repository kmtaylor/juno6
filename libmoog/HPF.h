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
#ifndef _LIBMOOG_HPF_H
#define _LIBMOOG_HPF_H

#include "IIR2.h"

class HPF : public IIR2
{
    friend void HPF_frqChanged(MoogObject *o, double data, long userData);
    friend void HPF_QChanged(MoogObject *o, double data, long userData);

    double frq;
    double Q;

    void frqChanged(double data) { frq = data; updateCoef(); }
    void QChanged(double data) { Q = data; updateCoef(); };

    void updateCoef();

 public:
    HPF();
    const char * getClassName() { return "HPF"; }
};


#endif /* _LIBMOOG_HPF_H */
