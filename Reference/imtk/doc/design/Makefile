# IMTK_DESIGN_DOC Makefile
# 
# $Id: Makefile,v 1.2 2001/11/15 21:43:26 clwyatt Exp $
#

# edit this for your particular configuration
include ../../makeinclude

SGML = imtkdesign.sgm

all: HTML RTF

HTML:
	for i in $(SGML); do ( jade -t sgml -d $(HOME)/jade/docbook/html/docbook.dsl $$i ) ; done	
	
RTF:
	for i in $(SGML); do ( jade -t rtf -d $(HOME)/jade/docbook/print/docbook.dsl $$i ) ; done

clean:
	$(RM) *.htm *.rtf
			
realclean: clean 
	
install:

