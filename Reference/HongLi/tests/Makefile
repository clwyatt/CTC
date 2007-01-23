# IMTK_BASE Makefile
# 
# $Id: Makefile,v 1.2 2003/10/13 20:03:40 hongli Exp $
#

# edit this for your particular configuration
include ../makeinclude

TESTS = \
subscript_test \
ndarray_test \
volume_test \
image_test \
vector_test \
thread_test \
rbtree_test \
curve_test \
minheap_test \
raggedarray_test \
specfun_test \
dataio_test \
bezier_test \
dicom_test 
#gridhash_test \
#mesh_test

#kdtree_test \

SOURCES.cc = \
ndarray_test.cc \
subscript_test.cc \
volume_test.cc \
image_test.cc \
vector_test.cc \
thread_test.cc  \
rbtree_test.cc \
curve_test.cc \
minheap_test.cc \
raggedarray_test.cc \
specfun_test.cc \
dataio_test.cc \
bezier_test.cc 
#gridhash_test.cc \
#mesh_test.cc 

#kdtree_test.cc \

LDFLAGS_ = $(LDFLAGS) -L. -L$(TOPLEVEL)/src/capd -L$(TOPLEVEL)/src/common
LDLIBS_ = -lcapd -limtk -lctn -lpthread $(LDLIBS)


.cc:
	echo Compiling and linking $@
	$(CXX) $(CXXFLAGS) $<  $(LDFLAGS_) $(LDLIBS_) -o $@



SOURCES.hh = 

SOURCES = \
	$(SOURCES.hh) \
	$(SOURCES.cc)

OBJECTS = \
	$(SOURCES.c:%.c=%.o) \
	$(SOURCES.cc:%.cc=%.o)

all: $(TESTS)

$(TESTS):

clean: 
	$(RM) *.o core

realclean: 
	$(RM) *.o core *% *~ $(TESTS) 

install:
	$(INSTALL) $(TESTS) $(BINPATH)
