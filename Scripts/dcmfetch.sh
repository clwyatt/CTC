#!/usr/bin/ksh

# fetch a DICOM series from a SCP to a local directory
# requires DCM4CHE to be installed 
# Author: C. L. Wyatt
# Usage: see below

usage(){
    print dcmfetch - a script to fetch a DICOM series 
    print "Usage: dcmfetch -p -s [-d]"
    print "  -p   Patient ID/MRN of the study/series"
    print "  -s   Series Number"
    print "  -d   local directory (default ./tmp)"
}

# define global paths
if [ "x$DCM4CHE_HOME" = "x" ]
    print "ERROR: DCM4CHE_HOME environment variable not set."
    exit 1
fi
DCMBIN=${DCM4CHE_HOME}/bin
 
#define defaults
LOCALDIR=${PWD}/tmp

# parse the arguments
if [ $# -lt 1 ] ; then
	usage
	exit 1
fi
while getopts "p:s:d:h" arg
do
	case $arg in
	    p)
	    	MRN=$OPTARG
		;;
	    s)
	    	SERIES=$OPTARG
		;;
	    d)
	    	LOCALDIR=$OPTARG
		;;
	    h|*)
	    	usage
		exit 1
		;;
	esac
done

# build the local AE title
AETITLE=AE$(hostname)

# setup and run the local SCP
${DCMBIN}/dcmrcv ${AETITLE}:11112 -dest $LOCALDIR >& /dev/null &
PID=$!
sleep 2

# do a QR to move the specified files to the local SCP
${DCMBIN}/dcmqr BSLSCP@brain:11112 -qPatientID=$MRN -qSeriesNumber=$SERIES -S -dest ${AETITLE} >& /dev/null

# shutdown the local SCP
kill -HUP $PID

# cleanup and exit
