# IMTK_BASE Makefile
# 
# $Id: Makefile,v 1.3 2003/10/13 19:58:00 hongli Exp $
#

# edit this for your particular configuration
include ../../makeinclude

SOURCES.cc = \
Object.cc \
Semaphore.cc \
Thread.cc \
Vertex.cc \
Vertex2D.cc \
Triangle.cc \
Line.cc \
Mesh.cc \
Curve.cc \
SpecFun.cc \
AnalyzeHdr.cc\
DataIO.cc \
BezierCurve.cc \
Dicom.cc\

SOURCES.hh = \
Data.hh \
NDArray.hh \
Object.hh \
Semaphore.hh \
Thread.hh \
Subscript.hh \
Volume.hh \
Image.hh \
Vector.hh \
Vertex.hh \
Vertex2D.hh \
Triangle.hh \
Line.hh \
RBTree.hh \
Mesh.hh \
Curve.hh \
KDTree.hh \
MinHeap.hh \
GridHash.hh \
RaggedArray.hh \
SpecFun.hh \
DataIO.hh \
BezierPoint.hh \
BezierCurve.hh \
BezierCurveMemento.hh \
Dicom.hh

SOURCES = \
	$(SOURCES.hh) \
	$(SOURCES.cc)

OBJECTS = \
	$(SOURCES.c:%.c=%.o) \
	$(SOURCES.cc:%.cc=%.o)

all: $(IMTK_BASE_STATIC_LIB)

$(IMTK_BASE_STATIC_LIB): $(OBJECTS)
	@echo $(LIBCOMMAND) $(IMTK_BASE_STATIC_LIB)
	@$(LIBCOMMAND) $(IMTK_BASE_STATIC_LIB) $(OBJECTS)
	@$(RANLIB) $(IMTK_BASE_STATIC_LIB)
	
clean: 
	$(RM) *.o core

realclean: 
	$(RM) *.o core *% *~ $(IMTK_BASE_SHARED_LIB) $(IMTK_BASE_STATIC_LIB)

install:
	$(INSTALL) $(SOURCES.hh) $(INCLUDEPATH)
#	$(INSTALL) $(IMTK_BASE_SHARED_LIB) $(LIBPATH)
	$(INSTALL) $(IMTK_BASE_STATIC_LIB) $(LIBPATH)
