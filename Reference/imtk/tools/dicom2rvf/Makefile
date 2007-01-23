# dicom2rvf tool Makefile
#
# $Id: Makefile,v 1.1 2002/02/26 20:42:15 clwyatt Exp $
#

# edit this for your particular configuration
include ../../makeinclude

PROGRAM = dicom2rvf

SOURCES.cc = dicom2rvf.cc

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
