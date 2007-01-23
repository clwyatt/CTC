#######################################################################################
# File: Makefile
# Abstract:  Toplevel makefile for capd project
# Created: 2002-06-24
# (C) 2002 by Wake Forest University School of Medicine
#                             and
#             Virgina Polytechnic and State University
#
# Last Revision ($Revision: 1.3 $) by $Author: hongli $ on $Date: 2003/10/13 20:25:34 $
#######################################################################################

# edit this for your particular configuration
include makeinclude    

# the following should not require any editing
all: CAPD_SRC CAPD_TOOLS CAPD_TESTS # CAPD_DOCS 

CAPD_SRC:
	cd src; $(MAKE) all

CAPD_TOOLS:
	cd tools; $(MAKE) all

CAPD_TESTS:
	cd tests; $(MAKE) all
	
CAPD_DOCS:
	cd src; doxygen Doxyfile
	
clean: 
	cd src; $(MAKE) clean
	cd tests; $(MAKE) clean
	cd tools; $(MAKE) clean
#	cd doc; $(MAKE) clean

realclean: 
	cd src; $(MAKE) realclean
	cd tools; $(MAKE) realclean
	cd tests; $(MAKE) realclean
#	cd doc; $(MAKE) realclean

tar:
	-tar -cvf backup.tar *
	-gzip backup.tar

install: 
	if [ ! -d ${PREFIX} ]; then mkdir ${PREFIX}; fi;
	if [ ! -d ${INCLUDEPATH} ]; then mkdir ${INCLUDEPATH}; fi;
	if [ ! -d ${BINPATH} ]; then mkdir ${BINPATH}; fi;
	if [ ! -d ${LIBPATH} ]; then mkdir ${LIBPATH}; fi;
	if [ ! -d ${DOCPATH} ]; then mkdir ${DOCPATH}; fi;
	cd src; $(MAKE) install
	cd tools; $(MAKE) install
#	cd doc; $(MAKE) install
