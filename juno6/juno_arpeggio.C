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

#include "juno_arpeggio.h"
#include "juno_control.h"


static int oct_x[] = { 1, 2, 4 };
static int NUM_VOICES = 0;
//void JunoArpeggio_Changed(MoogObject *o, double data, long userData)
//{
//    ((JunoArpeggio*)o)->Changed(data);
//}

void JunoArpeggio_gateChanged(MoogObject *o, double data, long userData)
{
    ((JunoArpeggio*)o)->gateChanged(userData, data);
}

void JunoArpeggio_onoffChanged(MoogObject *o, double data, long userData)
{
    ((JunoArpeggio*)o)->onoffChanged(data);
}

void JunoArpeggio_modeChanged(MoogObject *o, double data, long userData)
{
    ((JunoArpeggio*)o)->modeChanged(data);
}

void JunoArpeggio_rangeChanged(MoogObject *o, double data, long userData)
{
    ((JunoArpeggio*)o)->rangeChanged(data);
}

void JunoArpeggio_rateChanged(MoogObject *o, double data, long userData)
{
    ((JunoArpeggio*)o)->rateChanged(data);
}


JunoArpeggio::JunoArpeggio(JunoControl *jc,int voices)
{
printf("creating arpeggio for %d voices\n",voices);
NUM_VOICES = voices;
    //addInput("", JunoArpeggio_Changed, 0, 1);
    //PATCH(jc, "", this, "");

    INIT_LIST_HEAD(&notes);
   #ifndef POSIX
    InitializeCriticalSection(&listMutex);
   #else
    pthread_mutex_init(&listMutex, NULL);
   #endif
    pos = 0.0;
    velocity = .001;
    currentNote = NULL;
    currentOctave = 0;
    mode = JUNO_ARPEGGIO_MODE_DOWN;
    numOctaves = 3;
    inPitches = new double *[NUM_VOICES];
    for (int i=0;i<NUM_VOICES;i++)
    {
    char *tmppitch = new char[15]; 
    sprintf(tmppitch,"voice%d_pitch",i);
    addInput(tmppitch);
    }
 for (int i=0;i<NUM_VOICES;i++)
    {
    char *tmppitch = new char[15]; 
    sprintf(tmppitch,"voice%d_pitch",i);
    PATCH(jc, tmppitch, this, tmppitch);
    }
 for (int i=0;i<NUM_VOICES;i++)
    {
    char *tmpgate = new char[14];
    sprintf(tmpgate,"voice%d_gate",i); 
    addInput(tmpgate, JunoArpeggio_gateChanged, i, 1);
    }
     for (int i=0;i<NUM_VOICES;i++)
    {  
    char *tmpgate = new char[14];
    sprintf(tmpgate,"voice%d_gate",i); 
    PATCH(jc, tmpgate, this, tmpgate);
    }
 for (int i=0;i<NUM_VOICES;i++)
    {
    char *tmppitch = new char[15];  
    sprintf(tmppitch,"voice%d_pitch",i);
    addOutput(tmppitch, false);
    }
 for (int i=0;i<NUM_VOICES;i++)
    {
    char *tmpgate = new char[14];
    sprintf(tmpgate,"voice%d_gate",i); 
    addOutput(tmpgate, false);
    }
    
   
    /*addInput("voice1_pitch");
    addInput("voice2_pitch");
    addInput("voice3_pitch");
    addInput("voice4_pitch");
    addInput("voice5_pitch");
    PATCH(jc, "voice0_pitch", this, "voice0_pitch");
    PATCH(jc, "voice1_pitch", this, "voice1_pitch");
    PATCH(jc, "voice2_pitch", this, "voice2_pitch");
    PATCH(jc, "voice3_pitch", this, "voice3_pitch");
    PATCH(jc, "voice4_pitch", this, "voice4_pitch");
    PATCH(jc, "voice5_pitch", this, "voice5_pitch");

    addInput("voice0_gate", JunoArpeggio_gateChanged, 0, 1);
    addInput("voice1_gate", JunoArpeggio_gateChanged, 1, 1);
    addInput("voice2_gate", JunoArpeggio_gateChanged, 2, 1);
    addInput("voice3_gate", JunoArpeggio_gateChanged, 3, 1);
    addInput("voice4_gate", JunoArpeggio_gateChanged, 4, 1);
    addInput("voice5_gate", JunoArpeggio_gateChanged, 5, 1);
    PATCH(jc, "voice0_gate", this, "voice0_gate");
    PATCH(jc, "voice1_gate", this, "voice1_gate");
    PATCH(jc, "voice2_gate", this, "voice2_gate");
    PATCH(jc, "voice3_gate", this, "voice3_gate");
    PATCH(jc, "voice4_gate", this, "voice4_gate");
    PATCH(jc, "voice5_gate", this, "voice5_gate");

    addOutput("voice0_pitch", false);
    addOutput("voice1_pitch", false);
    addOutput("voice2_pitch", false);
    addOutput("voice3_pitch", false);
    addOutput("voice4_pitch", false);
    addOutput("voice5_pitch", false);
    addOutput("voice0_gate", false);
    addOutput("voice1_gate", false);
    addOutput("voice2_gate", false);
    addOutput("voice3_gate", false);
    addOutput("voice4_gate", false);
    addOutput("voice5_gate", false);*/

    addInput("arpeggio_switch", JunoArpeggio_onoffChanged, 0, 1);
    addInput("arpeggio_mode", JunoArpeggio_modeChanged, 0, 1);
    addInput("arpeggio_range", JunoArpeggio_rangeChanged, 0, 1);
    addInput("arpeggio_rate", JunoArpeggio_rateChanged, 0, 1);
    PATCH(jc, "arpeggio_switch", this, "arpeggio_switch");
    PATCH(jc, "arpeggio_mode", this, "arpeggio_mode");
    PATCH(jc, "arpeggio_range", this, "arpeggio_range");
    PATCH(jc, "arpeggio_rate", this, "arpeggio_rate");

    for (int i = 0; i < NUM_VOICES; i++)
	inPitches[i] = inputs[i].data;
}

void JunoArpeggio::connectTo(ConnectionInfo *info)
{
    MoogObject::connectTo(info);
    for (int i = 0; i < NUM_VOICES; i++)
	inPitches[i] = inputs[i].data;
}

void JunoArpeggio::disconnectTo(ConnectionInfo *info)
{
    MoogObject::disconnectTo(info);
    for (int i = 0; i < NUM_VOICES; i++)
	inPitches[i] = inputs[i].data;
}



//void JunoArpeggio::Changed(double data)
//{
//}

extern void turnOnArpeggio(bool);

void JunoArpeggio::onoffChanged(double data)
{
    //FIXME: this is a hack
    if (data)
    {
	turnOnArpeggio(true);
    }
    else
    {
	turnOnArpeggio(false);
    }
}

void JunoArpeggio::modeChanged(double data)
{
    if (data == -1.0)
	mode = JUNO_ARPEGGIO_MODE_UP;
    else if (data == 0.0)
	mode = JUNO_ARPEGGIO_MODE_UP_DOWN;
    else
	mode = JUNO_ARPEGGIO_MODE_DOWN;
}

void JunoArpeggio::rangeChanged(double data)
{
    if (data == -1.0)
	numOctaves = 1;
    else if (data == 0.0)
	numOctaves = 2;
    else 
	numOctaves = 3;
}

void JunoArpeggio::rateChanged(double data)
{
    velocity = data / 100 + .001;
}

void JunoArpeggio::gateChanged(int which, double data)
{
    //printf("gate #%d changed to %f, pitch %f\n", which, data, *inputs[which].data);
    #ifndef POSIX
    EnterCriticalSection(&listMutex);
    #else
    pthread_mutex_lock(&listMutex);
    #endif
    struct list_head *next = notes.next;
    struct juno_arpeggio_note *note = NULL;
    double pitch = *inputs[which].data;
    int added = 0;

    while (next != &notes)
    {
	note = list_entry(next, struct juno_arpeggio_note, node);

	if (data == 0) //release / remove
	{
	    if (note->voice == which)
	    {
		if (note == currentNote)
		{
		    note->deleted = 1;
		}
		else
		{
		    //printf("note %f deleted in gateChanged\n", note->pitch);

		    list_del(next);
		    free(note);
		}
		break;
	    }
	}
	else //press / add
	{
	    if (*inPitches[note->voice] == pitch)
	    {
		if (!note->deleted)
		{
		    //FIXME why does this case happen??
		    //printf("***********AHHHHHAHAHAHAHAHAHAHHHHHHH*****\n");
		}
		else
		{
		    note->deleted = 0;
		}
		
		added = 1;

		break;
	    }
	    else if (*inPitches[note->voice] > pitch)
	    {
		note = (struct juno_arpeggio_note *)malloc(sizeof(*note));
		note->deleted = 0;
		note->voice = which;

		list_add(&note->node, next->prev);

		added = 1;
		break;
	    }
	}

	next = next->next;
    }

    // if the list is empty, or this is highest note add to end of list
    if (data && !added)
    {
	note = (struct juno_arpeggio_note *)malloc(sizeof(*note));
	note->deleted = 0;	
	note->voice = which;

	list_add(&note->node, next->prev);

	added = 1;
    }

    if (currentNote == NULL && added)
	startInternal();
    
    if (list_empty(&notes) || (note->deleted && notes.prev == notes.next))
	stopInternal();

    /* debug purposes only */
    /*
    printf("-------------\n");
    next = notes.next;
    while (next != &notes)
    {
	note = list_entry(next,struct juno_arpeggio_note, node);
	printf("note: pitch=%f voice=%d %s\n", 
	       *inPitches[note->voice], 
	       note->voice,
	       note->deleted ? "(deleted)" : "");
	next = next->next;
    }
    */
    /* end debug */
    #ifndef POSIX
    LeaveCriticalSection(&listMutex);
    #else
    pthread_mutex_unlock(&listMutex);
    #endif
}

void JunoArpeggio::controlGo()
{
    pos += velocity;

    if (pos >= 1.0)
    {
    #ifndef POSIX
    EnterCriticalSection(&listMutex);
    #else
    pthread_mutex_lock(&listMutex);
    #endif

	moveNextNote();

    #ifndef POSIX
    LeaveCriticalSection(&listMutex);
    #else
    pthread_mutex_unlock(&listMutex);
    #endif
    }
}


/* note: this must be called with mutex locked */
void JunoArpeggio::startInternal()
{
    if (list_empty(&notes))
    {
	printf("start called, but nothing to do\n");
	return;
    }

    if (isControlScheduled())
    {
	printf("start called, already running!\n");
	return;
    }

    if (currentNote)
    {
	printf("start called, currentNote != NULL!\n");
	return;
    }
    
    pos = 0.0;

    if (mode != JUNO_ARPEGGIO_MODE_DOWN)
    {
	currentNote = list_entry(notes.next, struct juno_arpeggio_note, node);
	currentDir = 1;
	currentOctave = 0;
    }
    else
    {
	currentNote = list_entry(notes.prev, struct juno_arpeggio_note, node);
	currentDir = -1;
	currentOctave = numOctaves - 1;
    }

    outputs[currentNote->voice].setData(*inPitches[currentNote->voice] * oct_x[currentOctave]);
    outputs[currentNote->voice + NUM_VOICES].setData(1);

    Scheduler::scheduleControlRate(this, true);
}

/* note: this must be called with mutex locked */
void JunoArpeggio::stopInternal()
{
    /* note: this cannot call scheduleControlRate(this, false) because
     * we hold the listMutex, and we need to obtain the scheduler mutex
     * to change the scheduling lists, so it could deadlock
     */
    pos = 1.0;
}

/* note: this must be called with mutex locked */
void JunoArpeggio::moveNextNote()
{
    struct list_head *next;
    struct juno_arpeggio_note *nextNote;

    next = (currentDir == 1) ? currentNote->node.next : currentNote->node.prev;
    if (next == &notes)
    {
	currentOctave += currentDir;

	if (currentOctave >= numOctaves)
	{
	    if (mode == JUNO_ARPEGGIO_MODE_UP)
	    {
		currentOctave = 0;
		next = notes.next;
	    }
	    else /* JUNO_ARPEGGIO_MODE_UP_DOWN */
	    {
		currentOctave--;
		currentDir = -1;

		next = currentNote->node.prev;

		/* check if there is only one note pressed */
		if (next == &notes)
		{
		    /* catch the case where we only have one octave */
		    if (--currentOctave == -1)
		    {
			currentOctave = 0;
		    }

		    next = notes.next;
		}
	    }

	}
	else if (currentOctave == -1)
	{
	    if (mode == JUNO_ARPEGGIO_MODE_DOWN)
	    {
		currentOctave = numOctaves - 1;
		next = notes.prev;
	    }
	    else /* JUNO_ARPEGGIO_MODE_UP_DOWN */
	    {
		currentOctave++;
		currentDir = 1;

		next = currentNote->node.next;

		/* check if there is only one note pressed */
		if (next == &notes)
		{
		    /* catch the case where we only have one octave */
		    if (++currentOctave == numOctaves)
		    {
			currentOctave = 0;
		    }

		    next = notes.next;
		}
	    }
	}
	else
	{
	    next = (currentDir == 1) ? notes.next : notes.prev;
	}
    }

    nextNote = list_entry(next, struct juno_arpeggio_note, node);

    /* gate off */
    outputs[currentNote->voice + NUM_VOICES].setData(0);

    if (currentNote->deleted)
    {
	list_del(&currentNote->node);
	free(currentNote);

	if (list_empty(&notes))
	{
	    Scheduler::scheduleControlRate(this, false);
	    currentNote = NULL;
	    return;
	}
    }

    currentNote = nextNote;

    /* gate on, pitch on  */
    outputs[currentNote->voice].setData(*inPitches[currentNote->voice] * oct_x[currentOctave]);
    outputs[currentNote->voice + NUM_VOICES].setData(1.0);

    //printf("now playing %f %d voice %d\n", 
    //   *inPitches[currentNote->voice], 
    //   currentOctave,
    //   currentNote->voice);
    pos = 0.0;
}
