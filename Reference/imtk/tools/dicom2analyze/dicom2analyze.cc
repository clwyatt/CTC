// File: dicom2analyze.cc
// Abstract: convert dicom files to sigle analyze
//
// Created: 02/26/02 by: Chris L. Wyatt
//
//                

#include <iostream>
#include <fstream>
#include <cstdlib>

#include "Volume.hh"
#include "DataIO.hh"

using namespace IMTK;
using namespace std;

/*
* main routine
*/
int main(int argv, char **argc)
{
	char infilename[256];
	char outfilename[256];

	//parse command line
	if(argv == 3){
		strncpy(infilename, *(argc+1), 256);
		strncpy(outfilename, *(argc+2), 256);
	}
	else{
		cerr << "dicom2analyze ERROR: one or more file names not given" << endl;
		cerr << "Usage: dicom2analyze dicom.file analyze_base_name" << endl << endl;
		exit(EXIT_FAILURE);
	}

	Volume<unsigned short> data;
	int error;

	// attempt dicom read
	readDicomFromList(std::string(infilename), data, error);

	if(error) exit(EXIT_FAILURE);

	// attempt to write analyze
	writeANALYZE(std::string(outfilename), data, error);

	if(error) exit(EXIT_FAILURE);

	return 0;
}


