#!/bin/env python

"""SegmentColons

Segment the colons from a csv database

-d file.csv specifies the file
-m (optional) mrn to process
"""
import sys
import getopt
import csv
import os

def process(db, mrn, study, series):

    if os.getenv("CTC_IMG_DIR") == None:
        return False
    if os.getenv("CTC_DIR") == None:
        print "CTC_DIR not defined."
        return False

    # build directory
    indir = os.getenv("CTC_IMG_DIR")
    indir += "/"  + mrn + "/" + "%06i.STU/" % int(study) + "%06i.SER/000001.ACQ" % int(series)
    outbase = os.getenv("CTC_IMG_DIR")
    outbase += "/"  + mrn + "/" + "%06i.STU/" % int(study) + "%06i.SER/segment" % int(series)
    command = os.getenv("CTC_DIR") + "/bin/SegmentColon "
    command += "%s %s -d 5 " % (indir, outbase)
    print "Issuing command: " + command
    os.system(command)    


    
def main():
    # parse command line options
    try:
        opts, args = getopt.getopt(sys.argv[1:], "hd:o:m:", ["help"])
    except getopt.error, msg:
        print msg
        print "for help use --help"
        sys.exit(2)

    # check the number of args is ok
    if(len(opts) == 0):
        print "Error: incorrect arguments specified\n"
        print __doc__
        sys.exit(0)
        
    # process options
    mrn = None
    for o, a in opts:
        if o in ("-h", "--help"):
            print __doc__
            sys.exit(0)
        if o == "-d":
            infilename = a
        if o == "-o":
            outfilename = a
        if o == "-m":
            mrn = a
            
    reader = csv.reader(file(infilename))
    db = dict()
    for entry in reader:
        if entry[0] == "MRN":
            continue
        if entry[0] in db:
            db[entry[0]].append(entry[1:7])
        else:
            db[entry[0]] = [entry[1:7]]

    done = dict()
    if mrn == None:
        for m in db:
            for l in db[m]:
                if "%s-%s-%s" % (mrn, l[0], l[1]) in done:
                     continue
                process(db, m, l[0], l[1])
                done["%s-%s-%s" % (mrn, l[0], l[1])] = True
    else:
        for l in db[mrn]:
            if "%s-%s-%s" % (mrn, l[0], l[1]) in done:
                continue
            process(db, mrn, l[0], l[1])
            done["%s-%s-%s" % (mrn, l[0], l[1])] = True
            
if __name__ == "__main__":
    main()
