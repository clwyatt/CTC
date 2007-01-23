# dicom2rvf tool Makefile
#
# $Id: Makefile,v 1.1 2002/03/09 19:30:58 clwyatt Exp $
#

# edit this for your particular configuration
include ../../makeinclude

PROGRAM = raw2rvf

SOURCES.cc = raw2rvf.cc

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
