# lsf2oogl tool Makefile
#
# $Id: Makefile,v 1.5 2002/07/31 20:25:52 clwyatt Exp $
#

# edit this for your particular configuration
include ../../makeinclude

PROGRAM = lsf2oogl

SOURCES.cc = lsf2oogl.cc waltzcube.cc tetradecomp.cc

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
