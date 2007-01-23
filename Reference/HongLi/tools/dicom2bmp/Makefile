# dicom2bmp tool Makefile
#
# $Id: Makefile,v 1.2 2003/10/13 20:03:49 hongli Exp $
#

# edit this for your particular configuration
include ../../makeinclude

PROGRAM = dicom2bmp

SOURCES.cc = dicom2bmp.cc

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
