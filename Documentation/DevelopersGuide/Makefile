

BASE=CTCDevelopersGuide

help:
	@echo
	@echo "    make                 # to get a dvi file"
	@echo "    make ps              # to get a PostScript file"
	@echo "    make pdf             # to get an Adobe PDF file"
	@echo "    make view            # to view your file using xdvi"
	@echo "    make gv              # to view your file using gv"
	@echo "    make print           # to print to the default printer"
	@echo "    make clean           # to clean up"
	@echo "    make realclean       # to remove all generated files"
	@echo

all: ps gv view pdf print
	@echo $(BASE)

# ------------>----------->---------->--------->-------->------->------>----->---->--->-->->>
# Here is where the real work is done
#
$(BASE).dvi: $(BASE).tex
	latex $(BASE)
	bibtex $(BASE)
	latex $(BASE)
	latex $(BASE)

ps:	$(BASE).dvi
	dvips -o $(BASE).ps $(BASE)

gv:	ps
	ghostview $(BASE) &

view:	$(BASE).dvi
	xdvi $(BASE) &

pdf:	$(BASE).dvi
	dvipdfm $(BASE).dvi

print: $(BASE).dvi
	dvips $(BASE)

clean:
	@rm -f $(BASE).aux $(BASE).toc $(BASE).idx $(BASE).ind $(BASE).ilg $(BASE).log $(BASE).out \
           $(BASE).bbl $(BASE).blg $(BASE).lof $(BASE).lot

realclean:	clean
	@rm -f $(BASE).ps $(BASE).pdf $(BASE).dvi *.~*~
