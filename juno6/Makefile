include ../Makefile.include

GTK_CFLAGS=$(shell gtk-config --cflags)
INCLUDE_PATH=-I.. -Iumg/base/include $(GTK_CFLAGS)
CPPFLAGS=$(INCLUDE_PATH) $(GTK_CFLAGS) -DPOSIX -Wall $(OPTIMIZE) $(DEBUG) $(PROFILE) -Wno-write-strings
LDFLAGS= -L ../moog -L ../gmoog -L ../util -lgmoog -lmoog -lmoogutil $(shell gtk-config --libs) -lgthread

PROGS=juno6 list_patches copy_patch

OBJS=   juno6.o\
	juno_gui.o \
	juno_synth.o \
	juno_control.o \
	juno_voice.o \
	juno_wrappers.o \
	juno_lfo.o \
	juno_arpeggio.o\
	juno_chorus.o\
	juno_patch.o\
	about.o\
	juno_background.o\
	juno_background_mask.o\

all: $(PROGS)

objs: $(OBJS)

juno6: $(OBJS)

test:
	./juno6

clean:
	rm -f *.o core .depend $(PROGS)

list_patches copy_patch: juno_patch.o

.PHONY: test objs

.depend: Makefile
	touch .depend
	makedepend -f .depend $(INCLUDE_PATH) -I/usr/include/g++-2 *.C *.c

#include .depend
