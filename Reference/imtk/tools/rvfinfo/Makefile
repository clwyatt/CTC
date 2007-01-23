# lsf2oogl tool Makefile
#
# $Id: Makefile,v 1.1 2001/09/07 19:30:10 clwyatt Exp $
#

# edit this for your particular configuration
include ../../makeinclude

PROGRAM = rvfinfo

SOURCES.cc = rvfinfo.cc

SOURCES.hh = 

SOURCES = \
	$(SOURCES.hh) \
	$(SOURCES.cc)

OBJECTS = \
	$(SOURCES.c:%.c=%.o) \
	$(SOURCES.cc:%.cc=%.o)

all: $(PROGRAM)

$(PROGRAM): $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) $(LDFLAGS) -limtk $(LDLIBS)
	
clean: 
	$(RM) *.o core

realclean: 
	$(RM) *.o core *% *~ $(PROGRAM) 

install:
	$(INSTALL) $(PROGRAM) $(BINPREFIX)
