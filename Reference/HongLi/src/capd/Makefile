#######################################################################################
# File: Makefile
# Abstract:  capd objects
# Created: 2002-06-24
# (C) 2002 by Wake Forest University School of Medicine
#                             and
#         Virgina Polytechnic Institute and State University
#
# Last Revision ($Revision: 1.7 $) by $Author: hongli $ on $Date: 2003/10/13 19:56:51 $
#######################################################################################

include ../../makeinclude

SOURCES.cc = \
Mesh_Features.cc\
Vertex_with_Features.cc\
Triangle_with_Features.cc\
mvVec3f.cc\
mvLesion.cc\
curvature.cc\
Colon.cc\
hotelling.cc\
CAPD_alg.cc\
nrerror.cc\
Cube.cc\
MarchingCube.cc\
mHash.cc


SOURCES.hh = \
nrmatrix.hh\
nrinvert.hh\
nrvector.hh\
nrsvdcmp.hh\
nrsvbksb.hh\
Vertex_with_Features.hh\
Triangle_with_Features.hh\
nrfree_matrix.hh\
nrfree_vector.hh\
nrjacobi.hh\
Volume_ext.hh\
hotelling.hh\
curvature.hh \
mvLesion.hh\
mvVec3i.hh\
mvVec3f.hh\
Cube.hh\
MarchingCube.hh\
mHash.hh

SOURCES.h = \
nrerror.h\
CAPD_alg.h\
Colon.hh

SOURCES = \
	$(SOURCES.hh) \
	$(SOURCES.cc)\
	$(SOURCES.c) \
	$(SOURCES.h) \

OBJECTS = $(SOURCES.cc:%.cc=%.o) \
	$(SOURCES.c:%.c=%.o) 
	

LIB = libcapd.a
LDFLAGS_ = $(LDFLAGS) -L. -L$(TOPLEVEL)/src/capd -L$(TOPLEVEL)/src/common

all: libcapd.a 

libcapd.a: $(OBJECTS)
	@echo $(LIBCOMMAND) libcapd.a $(OBJECTS)
	@$(LIBCOMMAND) libcapd.a $(OBJECTS)
	@$(RANLIB) libcapd.a


clean: 
	$(RM) *.o core

realclean: 
	$(RM) *.o core *% *~ make.* libcapd.a 


install:
	$(INSTALL) $(LIB)	$(LIBPATH)
	$(INSTALL) $(SOURCES.hh) $(SOURCES.h) $(INCLUDEPATH)
