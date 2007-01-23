/********************************************************
File: waltzer.cc
Abstract: My implementation of waltzing cubes
Created: 01/10/2001
Author: Chris L. Wyatt

Last Revision ($Revision: 1.6 $) by $Author: clwyatt $ on $Date: 2002/07/31 20:24:57 $
*********************************************************/
#include <iostream>
#include <fstream>
#include <string>
#include <list>

#include "Mesh.hh"
#include "Volume.hh"
#include "Vector.hh"
#include "DataIO.hh"

using namespace std;
using namespace IMTK;


void waltzcube(Volume<unsigned short> & V, Vector<unsigned int> seed, float threshold, Mesh & mesh);

void load_seed_file(const string &filename, list< Subscript<3> > &seeds)
{
	ifstream ifs;

	ifs.open( filename.c_str() );

	if(!ifs){
		cerr << "Error opening seed file " << filename << endl;
		exit(EXIT_FAILURE);
	}

	int Pn, Px, Py, Pz;
	IMTK::Subscript<3> tempseed;
	while( ifs.good() ){
		ifs >> Pn >> Px >> Py >> Pz;

		clog << "Seed " << Pn << " at " << Px << ", " << Py << ", " << Pz << endl;
		tempseed[0] = Px;
		tempseed[1] = Py;
		tempseed[2] = Pz;

		if( ifs.good() ) seeds.push_front(tempseed);
	}

}

/*
* main routine
*/

int main(int argv, char **argc)
{
	char infilename[100];
	char seedfilename[100];
	char outfilename[100];
	list< IMTK::Subscript<3> > seeds;
	float threshold;
	Mesh m(false);
	Volume<unsigned short> V;
	
	if(argv != 5){
		cerr << "Usage: waltzer infile.rvf seedfile threshold outfile.oogl \n" << endl;
		exit(1);
	}

	//parse command line, no checking!
	strncpy(infilename, *(argc+1), 100);
	strncpy(seedfilename, *(argc+2), 100);
	threshold = atof(*(argc+3)) + 0.0000001; // numerical purterbation to make sure isovalue not exactly on boundary
	strncpy(outfilename, *(argc+4), 100);
	
	clog << "Parsing seed file." << endl;
	load_seed_file(string(seedfilename), seeds);

	//read file
	int error;
	readRVF(infilename, V, true, error);
	if(error != 0){
		cerr << "Error reading RVF file." << endl;
		exit(EXIT_FAILURE);
	}

	Subscript<3> bounds = V.getBounds();
	float xo, yo, zo, dx, dy, dz;
	V.getOrigin(xo, yo, zo);
	V.getVoxelSpacing(dx, dy, dz);
	m.setBounds(bounds[0], bounds[1], bounds[2]);
	m.setVoxelSize(dx, dy, dz);
	m.setOrigin(xo, yo, zo);

	//Compute surface
	list< Subscript<3> >::iterator it = seeds.begin();
	while(it != seeds.end() ){

		clog << (*it)[0] << " " <<  (*it)[1] << " " << (*it)[2] << endl;
		Vector<unsigned int> seed(3);
		seed[0] = (*it)[0];
		seed[1] = (*it)[1];
		seed[2] = (*it)[2];
		waltzcube(V, seed, threshold, m);

		it++;
	}

	//write surface
	clog << "Writing surface." << endl;
	m.ooglwrite(outfilename);
}

