// File: dicom2rvf.cc
// Abstract: convert dicom files to sigle rvf
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

/*
* main routine
*/
int main(int argv, char **argc)
{
	char infilename[256];
	char outfilename[256];
	int xfrom, yfrom, zfrom, zto, yto, xto;

	//parse command line
	if(argv == 9){
		strncpy(infilename, *(argc+1), 256);
		strncpy(outfilename, *(argc+2), 256);
		xfrom  = atoi(argc[3]);
		xto  = xfrom + atoi(argc[4]);
		if(xfrom >= xto) 
			cerr << "cliprvf ERROR: xdim error" << endl;
		yfrom  = atoi(argc[5]);
		yto  = yfrom + atoi(argc[6]);
		if(yfrom >= yto) 
			cerr << "cliprvf ERROR: ydim error" << endl;

		zfrom  = atoi(argc[7]);
		zto  = zfrom + atoi(argc[8]);
		if(zfrom >= zto) 
			cerr << "cliprvf ERROR: zdim error" << endl;
		 
	}
	else{
		cerr << "cliprvf ERROR: one or more file names not given" << endl;
		cerr << "Usage: cliprvf rvf.in rvf.out x, xsize, y, ysize, z, zsize" << endl << endl;
		exit(EXIT_FAILURE);
	}

	Volume<unsigned short> data;
	int error;
	bool mask = false;
	unsigned short temp;
        unsigned int i, j, k;

	// attempt dicom read
	readRVF(std::string(infilename), data, mask,  error);
	Volume<unsigned short> dataout;
	IMTK::Subscript<3> bounds;
	bounds[0] = xto-xfrom;
	bounds[1] = yto-yfrom;
	bounds[2] = zto-zfrom;
	 dataout.setBounds(bounds);

	// write out the origin and voxel dimensions	

	float xo, yo, zo;
	float dx, dy, dz;

	data.getOrigin(xo, yo, zo);
	data.getVoxelSpacing(dx, dy, dz);

	dataout.setOrigin(xo + xfrom*dx, yo + yfrom*dy, zo + zfrom*dz);
	dataout.setVoxelSpacing(dx, dy, dz);
	
	for(k=0; k < bounds[2]; k++)
		for(j=0; j < bounds[1]; j++)
			for(i=0; i < bounds[0]; i++){
				temp = data.getVoxel(i + xfrom, j+yfrom, k+zfrom);
				dataout.setVoxel(i, j, k, temp);
			}
	

	if(error) exit(EXIT_FAILURE);

	// attempt to write rvf
	writeRVF(std::string(outfilename), dataout, error);

	if(error) exit(EXIT_FAILURE);

	return 0;
}


