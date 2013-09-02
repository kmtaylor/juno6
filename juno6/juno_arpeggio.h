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
#ifndef _JUNO6_JUNO_ARPEGGIO_H
#define _JUNO6_JUNO_ARPEGGIO_H

#include <moog/MoogObject.h>
#include <util/list.h>
#ifdef POSIX
#include <pthread.h>
#else
#include <windows.h>
#endif
enum {
    JUNO_ARPEGGIO_MODE_UP,
    JUNO_ARPEGGIO_MODE_DOWN,
    JUNO_ARPEGGIO_MODE_UP_DOWN
};

struct juno_arpeggio_note
{
    int deleted;
    int voice;
    struct list_head node;
};

class JunoControl;

class JunoArpeggio : public MoogObject 
{
    //friend void JunoArpeggio_Changed(MoogObject *, double, long);
    friend void JunoArpeggio_gateChanged(MoogObject *, double, long);
    friend void JunoArpeggio_onoffChanged(MoogObject *, double, long);
    friend void JunoArpeggio_modeChanged(MoogObject *, double, long);
    friend void JunoArpeggio_rangeChanged(MoogObject *, double, long);
    friend void JunoArpeggio_rateChanged(MoogObject *, double, long);

    //void Changed(double);
    void gateChanged(int, double);
    void onoffChanged(double);
    void modeChanged(double);
    void rangeChanged(double);
    void rateChanged(double);

    struct list_head notes;
    #ifndef POSIX
    CRITICAL_SECTION listMutex;
    #else
    pthread_mutex_t listMutex;
    #endif
    double velocity;
    int mode;
    int numOctaves;

    double pos;
    struct juno_arpeggio_note *currentNote;
    int currentOctave;
    int currentDir;

    double **inPitches;

    void startInternal();
    void stopInternal();
    void moveNextNote();

 public:
    JunoArpeggio(JunoControl *,int);

    void connectTo(ConnectionInfo *info);
    void disconnectTo(ConnectionInfo *info);

    void controlGo();

    const char *getClassName() { return "JunoArpeggio"; }
};

#endif /*  _JUNO6_JUNO_ARPEGGIO_H */

