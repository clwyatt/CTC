#######################################################################################
# File: Makefile
# Abstract:  segmentation
# Created: 2002-06-24
# (C) 2002 by Wake Forest University School of Medicine
#                             and
#         Virgina Polytechnic Institute and State University
#
# Last Revision ($Revision: 1.5 $) by $Author: hongli $ on $Date: 2003/10/13 20:01:21 $
#######################################################################################

include ../../makeinclude

SOURCES = \
	$(SOURCES.hh) \
	$(SOURCES.cc)\
	$(SOURCES.c) \
	$(SOURCES.h) 

OBJECTS = $(SOURCES.cc:%.cc=%.o)
#	$(SOURCES.c:%.c=%.o) 

LDFLAGS_ = $(LDFLAGS) -L. -L$(TOPLEVEL)/src/capd -L$(TOPLEVEL)/src/common
LDLIBS_ = -lcapd -limtk -lctn $(LDLIBS)

PROGRAMS = segment getseeds

all: segment getseeds 

segment: segment.o $(OBJECTS)
	@echo 	$(CXXLINK) -o segment  segment.o $(OBJECTS) $(LDFLAGS_) $(LDLIBS_)
	$(CXXLINK) -o segment  segment.o $(OBJECTS) $(LDFLAGS_) $(LDLIBS_)

getseeds: getseeds.o $(OBJECTS)
	@echo 	$(CXXLINK) -o getseeds getseeds.o $(OBJECTS) $(LDFLAGS_) $(LDLIBS_)
	$(CXXLINK) -o getseeds  getseeds.o $(OBJECTS) $(LDFLAGS_) $(LDLIBS_)

clean: 
	$(RM) *.o core

realclean: 
	$(RM) *.o core *% *~ make.* segment getseeds


install:
	$(INSTALL) $(PROGRAMS) $(BINPATH)
