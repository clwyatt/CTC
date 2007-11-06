#!/usr/bin/env python

import sys
import getopt
import string
import xml.parsers.expat

class XMLPropertyFile:
    "A class to parse simple configuration file"
    
    def __init__(self, id=0):
        self.id = id
        self.element = ""
        self.data = ""
        self.watchID = False
        self.store = False

    def start_element(self, name, attrs):        #print 'Start element:', name, attrs
        if name == "arrayjob":
            if self.id == string.atoi(attrs['id']):
                self.watchID = True

        if (self.watchID == True) & (name == self.element):
                self.store = True

    def end_element(self, name):
      if name == "arrayjob":
          self.watchID = False
      if (self.store == True) & (name == self.element):
          self.store = False

    def char_data(self, data):
        if self.store:
            self.data = data

class Usage(Exception):
    def __init__(self, msg):
        self.msg = msg

def main(argv=None):

    pf = XMLPropertyFile()

    if argv is None:
        argv = sys.argv
    try:
        try:
            opts, args = getopt.getopt(argv[1:], "f:i:e:h",["help"])
        except getopt.error, msg:
             raise Usage(msg)
        for o, a in opts:
            if o == "-f":
                filename = a
            if o == "-i":
                pf.id = string.atoi(a)
            if o == "-e":
                pf.element = a
            if o in ("-h", "--help"):
                raise Usage("help")

    except Usage, err:
        print >>sys.stderr, err.msg
        print >>sys.stderr, "for help use --help"
        return 2

    p = xml.parsers.expat.ParserCreate()

    p.StartElementHandler = pf.start_element
    p.EndElementHandler = pf.end_element
    p.CharacterDataHandler = pf.char_data

    f = open(filename)
    p.Parse(f.read(), 1)

    print pf.data

if __name__ == "__main__":
    sys.exit(main())
