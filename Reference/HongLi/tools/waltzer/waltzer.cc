/********************************************************
File: waltzer.cc
Abstract: My implementation of waltzing cubes
Created: 01/10/2001
Author: Chris L. Wyatt

Last Revision ($Revision: 1.1.1.1 $) by $Author: hongli $ on $Date: 2002/12/06 21:49:26 $
*********************************************************/
#include <iostream>
#include <fstream>
    
#include "Mesh.hh"
#include "Volume.hh"
#include "Vector.hh"
#include "DataIO.hh"

using namespace IMTK;

void waltzcube(Volume<unsigned short> & V, Vector<unsigned int> seed, float threshold, Mesh & mesh);

/*
* main routine
*/

int main(int argv, char **argc)
{
	char infilename[100];
	char outfilename[100];
	Vector<unsigned int> seed(3);
	float threshold;
	Mesh m(true);
	Volume<unsigned short> V;
	
	if(argv != 7){
		cerr << "Usage: waltzer infile.rvf seedx seedy seedz threshold outfile.oogl \n" << endl;
		exit(1);
	}

	//parse command line, no checking!
	strncpy(infilename, *(argc+1), 100);
	seed[0] = (unsigned int) atoi(*(argc+2));
	seed[1] = (unsigned int) atoi(*(argc+3));
	seed[2] = (unsigned int) atoi(*(argc+4));
	threshold = atof(*(argc+5)) + 0.0000001; // numerical purterbation to make sure isovalue not exactly on boundary
	strncpy(outfilename, *(argc+6), 100);
	
	//read file
	int error;
	readRVF(infilename, V, true, error);
	if(error != 0){
		cerr << "Error reading RVF file." << endl;
		exit(EXIT_FAILURE);
	}

	//Compute surface
	waltzcube(V, seed, threshold, m);

	V.dump("waltzer.raw");

	//write surface
	m.ooglwrite(outfilename);
}

