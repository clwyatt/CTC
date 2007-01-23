#######################################################################################
# File: Makefile
# Abstract: makefile for capd project
# Created: 2002-06-24
# (C) 2002 by Wake Forest University School of Medicine
#                             and
#         Virgina Polytechnic Institute and State University
#
# Last Revision ($Revision: 1.2 $) by $Author: hongli $ on $Date: 2003/10/13 20:01:58 $
#######################################################################################

include ../makeinclude


all: COMMON CAPD SEGMENT

COMMON:
	cd common; $(MAKE) all
	
CAPD:
	cd capd; $(MAKE) all
	
SEGMENT:
	cd segment; $(MAKE) all
	
clean: 
	$(RM) *.o core
	cd common; $(MAKE) clean
	cd capd; $(MAKE) clean
	cd segment; $(MAKE) clean

realclean: 
	$(RM) *.o core *% *~ make.* $(PROGRAM)
	cd common; $(MAKE) realclean
	cd capd; $(MAKE) realclean
	cd segment; $(MAKE) realclean

install:
	cd common; $(MAKE) install
	cd capd; $(MAKE) install
	cd segment; $(MAKE) install
