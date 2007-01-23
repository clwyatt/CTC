# lsf2oogl tool Makefile
#
# $Id: Makefile,v 1.1.1.1 2002/12/06 21:49:26 hongli Exp $
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

LDFLAGS_ = $(LDFLAGS) -L. -L$(TOPLEVEL)/src/capd -L$(TOPLEVEL)/src/common

all: $(PROGRAM)

$(PROGRAM): $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) $(LDFLAGS_) -limtk -lctn $(LDLIBS)
	
clean: 
	$(RM) *.o core

realclean: 
	$(RM) *.o core *% *~ $(PROGRAM) 

install:
	$(INSTALL) $(PROGRAM) $(BINPATH)
