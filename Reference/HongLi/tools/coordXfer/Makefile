# 
#Hong Li Wfubmc
# 
# 
include ../../makeinclude
CC=gcc    
CCC=g++	

PROGRAM = coordXfer

SOURCES.cc = coordXfer.cc 

SOURCES = \
	$(SOURCES.hh) \
	$(SOURCES.cc)

OBJECTS = \
	$(SOURCES.c:%.c=%.o) \
	$(SOURCES.cc:%.cc=%.o)

# Compiler flags.
LDFLAGS_ = $(LDFLAGS) -L. -L$(TOPLEVEL)/src/capd -L$(TOPLEVEL)/src/common

all: $(PROGRAM) 
	
$(PROGRAM): $(OBJECTS)
	$(CXXLINK) -o $@ $(OBJECTS)  $(LDFLAGS_)  -limtk -lctn $(LDLIBS)
	
.SUFFIXES: .cc .o .hh

.cc.o:
	$(CCC) $(CXXFLAGS) $< -c

clean: 
	$(RM) *.o core

realclean: 
	$(RM) *.o core *% *~ $(PROGRAM) 

install:
	$(INSTALL) $(PROGRAM) $(BINPATH)
