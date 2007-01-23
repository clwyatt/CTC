# waltzer tool Makefile
#
# $Id: Makefile,v 1.5 2002/07/31 20:24:29 clwyatt Exp $
#

# edit this for your particular configuration
include ../../makeinclude

PROGRAM = waltzer

SOURCES.cc = waltzer.cc waltzcube.cc

SOURCES.hh = 

SOURCES = \
	$(SOURCES.hh) \
	$(SOURCES.cc)

OBJECTS = \
	$(SOURCES.c:%.c=%.o) \
	$(SOURCES.cc:%.cc=%.o)

all: $(PROGRAM)

$(PROGRAM): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS) $(LDFLAGS) -limtk $(LDLIBS)
	
clean: 
	$(RM) *.o core

realclean: 
	$(RM) *.o core *% *~ $(PROGRAM) 

install:
	$(INSTALL) $(PROGRAM) $(BINPREFIX)
