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
 * $Revision: 1.11.4.1 $$Date: 2000/02/24 00:44:04 $
 */
#ifndef SCHEDULER_H
#define SCHEDULER_H

#ifndef DEFAULT_SAMPLE_CONTROL_RATIO
#define DEFAULT_SAMPLE_CONTROL_RATIO 32
#endif

#include <util/list.h>
//#include <windows.h>
#include <pthread.h>

class GoObject;
class DSPOutput;

void *runSynth(void *);

#define CPS(x) (double)(x)/Scheduler::nyquistFreq

class Scheduler
{
    friend void *runSynth(void *);

    static struct list_head controlRateList;
    static struct list_head sampleRateList;

    static DSPOutput *dsp;
#ifdef POSIX
    static pthread_t tickThread;
#else
    static HANDLE tickThread;
#endif

    static void      safeListOp(struct list_head *node,
				struct list_head *list, bool add);
    static void      run();
    
    static int	     set_realtime_priority();

 public:
    static void      Init();
    static void      DeInit();

    static int       sampleRate;
    static int       sampleControlRatio;
    static double    controlRate;
    static double    nyquistFreq;
    static int       suspended;

    static void      setSampleRate(int actual);
    static void      setSampleControlRatio(int);

    static void      scheduleControlRate(GoObject *, bool);
    static void      scheduleSampleRate(GoObject *, bool);

    static void      start(DSPOutput *);
    static void      stop();

    static void      suspend();
    static void      resume();
};


#endif /* SCHEDULER_H */
