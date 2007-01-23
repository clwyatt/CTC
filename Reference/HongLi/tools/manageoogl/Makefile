# Hong Li
# 

# edit this for your particular configuration
include ../../makeinclude

CC=gcc    
CCC=g++	

PROGRAM = clipoogl clipmoogl getboundoogl mhash searchlog

SOURCES.cc = clipoogl.cc getboundoogl.cc mhash.cc searchlog.cc
SOURCES = \
	$(SOURCES.hh) \
	$(SOURCES.cc)

OBJECTS = \
	head $(SOURCES.c:%.c=%.o) \
	$(SOURCES.cc:%.cc=%.o)

# Compiler flags.
CPPFLAGS = -O2 -DNDEBUG  -DNULL=0 -D_REENTRANT
LDLIBS =  -lstdc++ -lm 

all: $(PROGRAM) 
	

clipmoogl: clipmoogl.o
	$(CCC) -o $@ clipmoogl.o $(LDFLAGS) $(LDLIBS)


clipoogl: clipoogl.o
	$(CCC) -o $@ clipoogl.o $(LDFLAGS) $(LDLIBS)

getboundoogl: getboundoogl.o
	$(CCC) -o $@ getboundoogl.o $(LDFLAGS) $(LDLIBS)
mhash: mhash.o
	$(CCC) -o $@ mhash.o $(LDFLAGS) $(LDLIBS)
searchlog: searchlog.o
	$(CCC) -o $@ searchlog.o $(LDFLAGS) $(LDLIBS)
	
.SUFFIXES: .cc .o 

.cc:
	$(CCC) $(CPPFLAGS) $< $(LDFLAGS) $(LDLIBS) -o $@

.cc.o:
	$(CCC) $(CPPFLAGS) $< -c

clean: 
	$(RM) *.o core

realclean: 
	$(RM) *.o core *% *~ $(PROGRAM) 

install: 
	$(INSTALL) $(PROGRAM) $(BINPATH)

