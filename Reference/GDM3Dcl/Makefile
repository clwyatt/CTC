# GDM3D app Makefile
# Chris L. Wyatt
# 

CC=gcc    
CCC=g++	

PROGRAM = gdm3dcl2

SOURCES.cc = \
buildTube.cc \
levelSetEvolve.cc \
tetradecomp.cc \
filter.cc \
configFile.cc \
gdm3dcl.cc
	     
SOURCES.hh = LF3Dfull.hh buildTube.hh configFile.hh levelSetEvolve.hh tetradecomp.hh filter.hh

SOURCES = \
	$(SOURCES.hh) \
	$(SOURCES.cc)

OBJECTS = \
	$(SOURCES.c:%.c=%.o) \
	$(SOURCES.cc:%.cc=%.o)

# Compiler flags.
CPPFLAGS = -gstabs+ -O2 -DNDEBUG -I../../sandbox/imtk/src  -DNULL=0 -D_REENTRANT
# CPPFLAGS = -g -I../../sandbox/imtk/src  -DNULL=0 -D_REENTRANT
LDFLAGS = -L../../sandbox/imtk/src
LDLIBS = -limtk -lstdc++ -lm -lpthread

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
