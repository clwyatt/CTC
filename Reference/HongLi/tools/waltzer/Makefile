# waltzer tool Makefile
#
# $Id: Makefile,v 1.2 2003/10/13 20:03:55 hongli Exp $
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

LDFLAGS_ = $(LDFLAGS) -L. -L$(TOPLEVEL)/src/capd -L$(TOPLEVEL)/src/common

all: $(PROGRAM)

$(PROGRAM): $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) $(LDFLAGS) -limtk -lctn $(LDLIBS)
	
clean: 
	$(RM) *.o core

realclean: 
	$(RM) *.o core *% *~ $(PROGRAM) 

install:
	$(INSTALL) $(PROGRAM) $(BINPATH)
