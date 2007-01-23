// File: analyze2rvf.cc
// Abstract: convert analyze db file to rvf
//
// Created: 05/01/02 by: Chris L. Wyatt
//
//                

#include <iostream>
#include <fstream>
#include <cstdlib>

#include "Volume.hh"
#include "DataIO.hh"

using namespace IMTK;

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
		cerr << "analyze2rvf ERROR: one or more file names not given" << endl;
		cerr << "Usage: analyze2rvf analyze_db_name rvf.file" << endl << endl;
		exit(EXIT_FAILURE);
	}

	Volume<unsigned short> data;
	int error;

	// attempt analyze read
	readAnalyze(std::string(infilename), data, error);

	if(error) exit(EXIT_FAILURE);

	// attempt to write rvf
	writeRVF(std::string(outfilename), data, error);

	if(error) exit(EXIT_FAILURE);

	return 0;
}


