// File: rvfinfo.cc
// Abstract: print rvf file header information
//
// Created: 11/29/99 by: Chris L. Wyatt
//
//                

#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

/*
* main routine
*/

int main(int argv, char **argc)
{
	char infilename[100];
        ifstream  infile;
        long sx, sy, sz, bitdepth;
	float dx, dy, dz, xo, yo, zo;
	unsigned char b1, b2, b3, b4;

	//parse command line
	if(argv == 2){
		strcpy(infilename, *(argc+1));
	}
	else{
		cerr << "rvfinfo ERROR: no file name given" << endl;
		cerr << "Usage: rvfinfo filename" << endl << endl;
		exit(EXIT_FAILURE);
	}

	infile.open( infilename );

	if(!infile){
		cerr << "rvfinfo ERROR: could not open file " << infilename << " for reading" << endl;
		exit(EXIT_FAILURE);
	}

	infile.read((char *)(&b1), 1);
	infile.read((char *)(&b2), 1);
	infile.read((char *)(&b3), 1);
	infile.read((char *)(&b4), 1);

	if( !((b1 == 100) && (b2 == 118) && (b3 == 118) && (b4 == 114)) ){
		cerr << "rvfinfo ERROR: file " << infilename << " does not appear to be a raw volume file" << endl;
		exit(EXIT_FAILURE);

	}

        infile.seekg(12);
        infile.read((char *)(&sx), 4);
        infile.read((char *)(&sy), 4);
        infile.read((char *)(&sz), 4);
	infile.read((char *)(&bitdepth), 4);

	infile.seekg(52);
        infile.read((char *)(&xo), sizeof(float));
        infile.read((char *)(&dx), sizeof(float));
        infile.read((char *)(&yo), sizeof(float));
        infile.read((char *)(&dy), sizeof(float));
        infile.read((char *)(&zo), sizeof(float));
        infile.read((char *)(&dz), sizeof(float));

	clog << "Raw Volume File        : " << infilename << endl;
	clog << "Volume size            : " << 2*sx*sy*sz << " bytes" << endl;
	clog << "       dimensions      : " << sx << " x " << sy << " x " << sz << endl;
	clog << "       dimensions (mm) : " << sx*dx << " x " << sy*dy << " x " << sz*dz << endl;
	clog << "       origin          : " << xo << ", " << yo << ", " << zo << endl;

}


