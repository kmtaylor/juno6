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
#include <util/debug.h>
#include <windows.h>
#include "Scheduler.h"
#include "GoObject.h"
#include "DSPOutput.h"

static void            dataWrittenCallback();
static int             set_realtime_priority();


static volatile int    needListSync = 0;
static list_head      *currentListIter;
static CRITICAL_SECTION beginListOpMutex;
//static pthread_cond_t  listOpCompleteCond =(pthread_cond_t) PTHREAD_COND_INITIALIZER;
//static pthread_cond_t  resumeCondition = (pthread_cond_t) PTHREAD_COND_INITIALIZER;

DSPOutput *Scheduler::dsp = NULL;
HANDLE  Scheduler::tickThread;
DWORD ThreadID;
double     Scheduler::controlRate         = 0;
int        Scheduler::sampleRate          = 0;
int        Scheduler::sampleControlRatio  = DEFAULT_SAMPLE_CONTROL_RATIO;
double     Scheduler::nyquistFreq         = 0;
int        Scheduler::suspended           = 0;

LIST_HEAD(Scheduler::controlRateList);
LIST_HEAD(Scheduler::sampleRateList);

void Scheduler::DeInit()
{
////puts("deinit");
DeleteCriticalSection(&beginListOpMutex);
}
void Scheduler::Init()
{
////puts("init");
InitializeCriticalSection(&beginListOpMutex);
}
void Scheduler::setSampleRate(int actual)
{
    sampleRate          = actual;
    controlRate         = sampleRate / sampleControlRatio;
    nyquistFreq         = sampleRate / 2;
}

void Scheduler::setSampleControlRatio(int r)
{
    sampleControlRatio = r;
    
    /* force some recalculation done in setSampleRate */
    setSampleRate(sampleRate);
}

void Scheduler::safeListOp(list_head *node, list_head *list, bool add)
{
    needListSync++;
////puts("try to enter");
    EnterCriticalSection(&beginListOpMutex);
////puts("ready");

    /* since the synth thread runs with the beginListOpMutex locked, 
     * getting here means that that thread is now waiting on the condition
     * variable, so we are safe to proceed, or this IS the synth thread
     * since the mutex is a recursive lock
     */

    if (add)
    {
	if (node->next != NULL)
	{
	    debug(DEBUG_APPERROR, "Warn: obj already on scheduling list");
	    goto out;
	}

	//debug( DEBUG_STATUS, "Adding %s to scheduling list",
	//      ((MoogObject*)list_entry(node, GoObject, sampleListNode ))->getClassName() );

	list_add(node, list);
    }
    else
    {
	if (node->next == NULL)
	{
	    debug(DEBUG_APPERROR, "Warn: obj not on scheduling list");
	    goto out;
	}

	/*
	 * This is a bit of hack for the case where an object removes
	 * itself from the list during the tick() walk of the linked
	 * list.  If we clear the list_head that is the current iterator
	 * in tick() then segfault we go.  So we shift the iter previous
	 * here, then it jumps passed where we were in the next advance.
	 */
	if (node == currentListIter)
	{
	    currentListIter = currentListIter->prev;
	}

	list_del(node);
	CLEAR_LIST_NODE(node);
    }

 out:
    needListSync--;
//    pthread_cond_signal(&listOpCompleteCond);
//puts("try to leave");
    LeaveCriticalSection(&beginListOpMutex);
//puts("ready");
}

void Scheduler::scheduleControlRate(GoObject *obj, bool schedule)
{
    if ( schedule != obj->isControlScheduled() )
	safeListOp(&obj->controlListNode, &controlRateList, schedule);
}

void Scheduler::scheduleSampleRate(GoObject *obj, bool schedule)
{
    if ( schedule != obj->isSampleScheduled() )
	safeListOp(&obj->sampleListNode, &sampleRateList, schedule);
}
int running = 0;
void Scheduler::start(DSPOutput *_dsp)
{
    dsp = _dsp;
    
    if (dsp && dsp->isOpen())
	dsp->setDataWrittenCallback(dataWrittenCallback);
    else
	dsp = NULL;
running = 1;



tickThread = CreateThread(NULL,0,runSynth,NULL,0,&ThreadID);

//    pthread_create(&tickThread, NULL, (DWORD *()(void*)&runSynth, NULL);
}


void Scheduler::stop()
{
running = 0;
//    pthread_cancel(tickThread);
//    pthread_join(tickThread, NULL);
}

void Scheduler::suspend()
{
//    suspended = 1;
}

void Scheduler::resume()
{
//puts("try to resume");
//EnterCriticalSection(&beginListOpMutex);
//puts("ready");
//    suspended = 0;
//    pthread_cond_signal(&resumeCondition);
//puts("try to leave resume");
//LeaveCriticalSection(&beginListOpMutex);
//puts("ready");
}

DWORD CALLBACK runSynth(void *)
{
    Scheduler::run();
    /* not reached */
    return 0;
}

void Scheduler::run()
{
    int controlCount = 0;
    GoObject  *obj;


    debug(DEBUG_APPMSG1, "[Attempting to set process priority, expect error if not root]");

    if (set_realtime_priority() < 0)
	debug(DEBUG_SYSERROR, "Problem with set_realtime_priority");
  
//    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    while (running)
    {


	if (controlCount == 0)
	{
	    currentListIter = controlRateList.next;
	    
	    while (currentListIter != &controlRateList)
	    {
		obj = list_entry(currentListIter, GoObject, GoObject::controlListNode);
		obj->controlGo();
		currentListIter = currentListIter->next;
	    }
	    
	    controlCount = sampleControlRatio;
	}
	
	controlCount--;
	
	currentListIter = sampleRateList.next;
	
	while (currentListIter != &sampleRateList) 
	{
	    obj = list_entry(currentListIter, GoObject, sampleListNode);


	    obj->sampleGo();
	    currentListIter = currentListIter->next;
	}

	/* this is so we can run even without a DSPOutput object */
//	if (!dsp && needListSync > 0)
//	    pthread_cond_wait(&listOpCompleteCond, &beginListOpMutex);
    }
}

/* NOTE: the dataWrittenCallback will be called from the runSynth thread
 *       from dsp->sampleGo() 
 *       from Scheduler::run()
 *
 * This routine is essentially a convenient point for the tick thread 
 * to block so that the linked lists con be manipulated by another thread.
 */

void dataWrittenCallback()
{
//    if (needListSync > 0)
//	pthread_cond_wait(&listOpCompleteCond, &beginListOpMutex);
}


/* this routine taken from Benno Sonner, sbenno@gardena.net */

int set_realtime_priority()
{
/*    struct sched_param schp;

    memset(&schp, 0, sizeof(schp));
    schp.sched_priority = sched_get_priority_max(SCHED_FIFO);
    
    if (sched_setparam(0,&schp) != 0) 
	return -1;
*/    
    return 0;
}


