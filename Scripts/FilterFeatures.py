#!/bin/env python

"""FilterFeatures

filter a csv feature training file for features within a distance threshold to polyp

-i specifies the file (default: features.csv)
-o specifies the output file (default: filtered.csv)
-t specifies the threshold

"""
import sys
import getopt
import csv
import string

def main():
    # parse command line options
    try:
        opts, args = getopt.getopt(sys.argv[1:], "hi:o:t:", ["help"])
    except getopt.error, msg:
        print msg
        print "for help use --help"
        sys.exit(2)

    # defaults
    infilename = "features.csv"
    outfilename = "filtered.csv"
    threshold = 5.0
    
    # process options
    for o, a in opts:
        if o in ("-h", "--help"):
            print __doc__
            sys.exit(0)
        if o == "-i":
            infilename = a
        if o == "-o":
            outfilename = a
        if o == "-t":
            threshold = string.atof(a)
            
    reader = csv.reader(file(infilename))
    writer = csv.writer(file(outfilename, "w"))
    for entry in reader:
        if entry[0] == "voxelx":
            continue
        else:
            if string.atof(entry[9]) < threshold:
                writer.writerow(entry[5:7])
    
if __name__ == "__main__":
    main()
