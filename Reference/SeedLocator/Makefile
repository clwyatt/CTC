# GDM3D app Makefile
# Chris L. Wyatt
# 

CC=gcc    
CCC=g++	

IMTK = ../../sandbox/imtk/src
FLTK = /home/clwyatt/fltk
LIBZ = ../../thirdparty/zlib
DICOM = ../../thirdparty/DCMTK_new

PROGRAM = seedloc

SOURCES.cc = \
configFile.cc \
mask.cc \
distance.cc \
seedloc.cc
	     
SOURCES.hh = configFile.hh mask.hh distance.hh

SOURCES = \
	$(SOURCES.hh) \
	$(SOURCES.cc)

OBJECTS = \
	$(SOURCES.c:%.c=%.o) \
	$(SOURCES.cc:%.cc=%.o)

# Compiler flags.
CPPFLAGS = -g -O2 -DNDEBUG -Wall -I../../sandbox/imtk/src  -I$(DICOM)/include -I$(LIBZ)/include -DHAVE_ZLIB -DNULL=0 -D_REENTRANT
# CPPFLAGS = -g -I../../sandbox/imtk/src  -DNULL=0 -D_REENTRANT
LDFLAGS = -L../../sandbox/imtk/src -L$(DICOM)/lib -L$(LIBZ)/lib
LDLIBS = -limtk -lstdc++ -lm -lpthread -ldcm -lz

all: $(PROGRAM)
	
$(PROGRAM): $(OBJECTS)
	$(LINK.cc) -o $@ $(OBJECTS) $(LDFLAGS) $(LDLIBS)
	
.SUFFIXES: .cc .o .fl .hh

.cc:
	$(CCC) $(CPPFLAGS) $< $(LDFLAGS) $(LDLIBS) -o $@

.cc.o:
	$(CCC) $(CPPFLAGS) $< -c

.fl.cc .fl.hh:
	fluid -c $<

.fl:
	fluid -c $<
	$(CCC) $(CPPFLAGS) $@.cc $(LDFLAGS) $(LDLIBS) -o $@
		
clean: 
	$(RM) *.o core

realclean: 
	$(RM) *.o core *% *~ $(PROGRAM) 

install:
	$(INSTALL) $(PROGRAM) $(BINPREFIX)
