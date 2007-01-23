# primenumber Makefile
#
# $Id: Makefile,v 1.1.1.1 2002/12/06 21:49:25 hongli Exp $
#

include ../../makeinclude

PROGRAM = primenumber

SOURCES.cc = primenumber.cc

SOURCES.hh = 

SOURCES = \
	$(SOURCES.hh) \
	$(SOURCES.cc)

OBJECTS = \
	$(SOURCES.c:%.c=%.o) \
	$(SOURCES.cc:%.cc=%.o)

all: $(PROGRAM) install

$(PROGRAM): $(OBJECTS)
	@echo  $(CXX) -o $@ $(OBJECTS) $(LDLIBS)
	$(CXX) -o $@ $(OBJECTS) $(LDLIBS)
	
clean: 
	$(RM) *.o core

realclean: 
	$(RM) *.o core *% *~ $(PROGRAM) 

install:
	$(INSTALL) $(PROGRAM) $(BINPATH)
