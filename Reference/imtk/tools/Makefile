# IMTK Tools Toplevel Makefile
#
# $Id: Makefile,v 1.6 2002/05/01 20:43:04 clwyatt Exp $
#

# edit this for your particular configuration
include ../makeinclude

TOOLS = rvfinfo lsf2oogl waltzer dicom2rvf raw2rvf analyze2rvf

all: 
	for i in $(TOOLS); do ( if [ -d $$i ]; then cd $$i ; $(MAKE) all; fi; ) ; done
	
clean: 
	for i in $(TOOLS); do ( if [ -d $$i ]; then cd $$i ; $(MAKE) $@; fi; ) ; done

realclean: 
	for i in $(TOOLS); do ( if [ -d $$i ]; then cd $$i ; $(MAKE) $@; fi; ) ; done
	
install:
	for i in $(TOOLS); do ( if [ -d $$i ]; then cd $$i ; $(MAKE) $@; fi; ) ; done
