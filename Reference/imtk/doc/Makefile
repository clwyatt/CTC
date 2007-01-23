# IMTK_DOC Makefile
# 
# $Id: Makefile,v 1.2 2001/11/15 21:41:59 clwyatt Exp $
#

# edit this for your particular configuration
include ../makeinclude

all: IMTK_DESIGN

IMTK_DESIGN:
	cd design; $(MAKE) all
	
clean:
	cd api; $(RM) -rf html latex
	cd design; $(MAKE) clean
		
realclean: 
	cd api; $(RM) -rf html latex
	cd design; $(MAKE) realclean
	
install:
