include ../Makefile.include

INCLUDE_DIRS=-I..
DEFINES=-D_REENTRANT -D_GNU_SOURCE -DUSE_FASTMEMCPY -DPOSIX
CFLAGS=$(PIC) $(PROFILE) $(OPTIMIZE) $(DEBUG) -Wall $(INCLUDE_DIRS) $(DEFINES)
CPPFLAGS=$(CFLAGS)


LIB_OBJS=ADSR.o\
	Attenuator.o\
	Balance.o\
	Chorus.o\
	ConnectionInfo.o\
	ConnectionList.o\
	ConnectionManager.o\
	DSPDevice.o\
	DSPOutput.o\
	DataBlock.o\
	Delay.o\
	GoObject.o\
	HPF.o\
	IIR2.o\
	Input.o\
	JunoPulse.o\
	JunoSaw.o\
	MidiDevice.o\
	MidiInputLinux.o\
	Mixer.o\
	MoogObject.o\
	NamedObject.o\
	Oscillator.o\
	Output.o\
	Rand.o\
	ResonantLowPass.o\
	Rms.o\
	Scheduler.o\
	Settings.o\
	WaveGen.o\
	bilinear.o\
	pitch.o

LIBNAME=libmoog.so
LIBSTATIC=libmoog.a
LIBMAJOR=2
LIBMINOR=0
LIBFULLNAME=$(LIBNAME).$(LIBMAJOR).$(LIBMINOR)
LIBDIR=./
LIB_LIBS=-L../util -lmoogutil -lpthread

DYN_FLAGS=-shared -Wl,-soname,"$(LIBNAME).$(LIBMAJOR)"

all: $(LIBFULLNAME)

objs: $(LIB_OBJS)

$(LIBFULLNAME): objs
	g++ $(DYN_FLAGS) -o $@ $(LIB_OBJS) $(LIB_LIBS)
	ln -f -s  $(LIBFULLNAME) $(LIBNAME)
	ln -f -s  $(LIBFULLNAME) $(LIBNAME).$(LIBMAJOR)

static: $(LIBSTATIC)

$(LIBSTATIC): $(LIB_OBJS)
	rm -f $(LIBSTATIC)
	ar r $(LIBSTATIC) $(LIB_OBJS)

install:
	cp $(LIBFULLNAME) $(LIBDIR)
	ln -f -s $(LIBDIR)/$(LIBFULLNAME) $(LIBDIR)/$(LIBNAME)

clean:
	rm -f *.o core .depend $(LIBNAME)* Makefile.bak

depend: .depend

.depend: Makefile
	touch .depend
	makedepend -f .depend $(INCLUDE_DIRS) -I /usr/include/linux *.C *.c


.PHONY: objs

#include .depend
