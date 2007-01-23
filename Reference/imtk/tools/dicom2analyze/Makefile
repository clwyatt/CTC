# dicom2rvf tool Makefile
#
# $Id: Makefile,v 1.1 2002/07/31 20:21:25 clwyatt Exp $
#

# edit this for your particular configuration
include ../../makeinclude

PROGRAM = dicom2analyze

SOURCES.cc = dicom2analyze.cc

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
