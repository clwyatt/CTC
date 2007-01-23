# IMTK_BASE Makefile
# 
# $Id: Makefile,v 1.7 2001/11/16 21:46:33 clwyatt Exp $
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
mesh_test \
curve_test \
minheap_test \
gridhash_test \
raggedarray_test \
specfun_test \
dataio_test \
bezier_test \
dicom_test

#kdtree_test \

SOURCES.cc = \
ndarray_test.cc \
subscript_test.cc \
volume_test.cc \
image_test.cc \
vector_test.cc \
thread_test.cc  \
rbtree_test.cc \
mesh_test.cc \
curve_test.cc \
minheap_test.cc \
gridhash_test.cc \
raggedarray_test.cc \
specfun_test.cc \
dataio_test.cc \
bezier_test.cc

#kdtree_test.cc \


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
	$(INSTALL) $(TESTS) $(BINPREFIX)
