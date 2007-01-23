# IMTK Toplevel Makefile
#
# $Id: Makefile,v 1.5 2001/11/15 21:41:25 clwyatt Exp $
#

# edit this for your particular configuration
include makeinclude    

# the following should not require any editing

all: IMTK_BASE IMTK_TESTS IMTK_TOOLS IMTK_DOCS

IMTK_BASE:
	cd src; $(MAKE) all
	
IMTK_TESTS:
	cd tests; $(MAKE) all
	
IMTK_TOOLS:
	cd tools; $(MAKE) all
	
IMTK_DOCS:
	cd src; doxygen Doxyfile
#cd src; doc++ -d ../doc/api *.hh
	
clean: 
	cd src; $(MAKE) clean
	cd tests; $(MAKE) clean
	cd tools; $(MAKE) clean
	cd doc; $(MAKE) clean

realclean: 
	cd src; $(MAKE) realclean
	cd tests; $(MAKE) realclean
	cd tools; $(MAKE) realclean
	cd doc; $(MAKE) realclean

tar:
	-tar -cvf backup.tar *
	-gzip backup.tar

install:
	cd src; $(MAKE) install
	cd tests; $(MAKE) install
	cd tools; $(MAKE) install
	cd doc; $(MAKE) install
