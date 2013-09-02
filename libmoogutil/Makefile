include ../Makefile.include

DEFINES=-D_REENTRANT -D_GNU_SOURCE -DSTRING_WITHOUT_NUMBER -DLOCAL_RCSID -DUSE_FASTMEMCPY
CFLAGS=$(PIC) $(PROFILE) $(OPTIMIZE) $(DEBUG) -Wall $(INCLUDE_DIRS) $(DEFINES)
CPPFLAGS=$(CFLAGS)

LIB_OBJS=debug.o String.o sio.o ini.o hash.o text_util.o aclib.o

LIBNAME=libmoogutil.so
LIBSTATIC=libmoogutil.a
LIBMAJOR=1
LIBMINOR=0
LIBFULLNAME=$(LIBNAME).$(LIBMAJOR).$(LIBMINOR)
LIBDIR=./
LIB_LIBS=

DYN_FLAGS=-shared -Wl,-soname,"$(LIBNAME).$(LIBMAJOR)"

all: $(LIBFULLNAME)

.PHONY: objs

objs: $(LIB_OBJS)

$(LIBFULLNAME): $(LIB_OBJS)
	g++ $(DYN_FLAGS) -o $@ $(LIB_OBJS) $(LIB_LIBS)
	ln -f -s  $(LIBFULLNAME) $(LIBNAME)
	ln -f -s  $(LIBFULLNAME) $(LIBNAME).$(LIBMAJOR)

clean:
	rm -f *.o $(LIBNAME)* core
