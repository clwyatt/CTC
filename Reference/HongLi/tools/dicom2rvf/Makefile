# dicom2rvf tool Makefile
#
# $Id: Makefile,v 1.2 2003/10/13 20:15:19 hongli Exp $
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

LDFLAGS_ = $(LDFLAGS) -L. -L$(TOPLEVEL)/src/common

all: $(PROGRAM) 

$(PROGRAM): $(OBJECTS)
	@echo  $(CXX) -o $@ $(OBJECTS) $(LDFLAGS_) -limtk -lctn $(LDLIBS)
	$(CXX) -o $@ $(OBJECTS) $(LDFLAGS_) -limtk -lctn $(LDLIBS)
	
clean: 
	$(RM) *.o core

realclean: 
	$(RM) *.o core *% *~ $(PROGRAM) 

install:
	$(INSTALL) $(PROGRAM) $(BINPATH)
