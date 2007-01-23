/********************************************************
File: test_tube.cc
Abstract: Test file for tube construction. 
Created: 11/04/2000
Author: Chris L. Wyatt

Last Revision ($Revision$) by $Author$ on $Date$
*********************************************************/
#include <iostream>
#include <fstream>

#include "buildTube.hh"

using namespace IMTK;

void write_LF3D(char * name,
                 std::LF3Dfull &lf)
{
        ofstream  outfile;
	Subscript<3> bounds;
	int i, xdim, ydim, zdim;
	float *temp;

	bounds = lf.getBounds();
	xdim = bounds[0];
	ydim = bounds[1];
	zdim = bounds[2];

	temp = new float[xdim*ydim*zdim];


	for(i = 0; i < xdim*ydim*zdim; i++){
		if( (lf[i] < 100) && (lf[i] > -100) ) temp[i] = lf[i];
		else temp[i] = 0;
	}

	outfile.open( name );
	outfile.write((char *)temp, 4*xdim*ydim*zdim);
	outfile.close();

	delete [] temp;

};

unsigned short * read_rvf_file(char *name, int *xdim, int *ydim, int *zdim)
{
        ifstream  infile;
	unsigned short * threedp;
        long sx, sy, sz;
        int size;

	infile.open( name );

        infile.seekg(12);

        infile.read((char *)(&sx), 4);
        infile.read((char *)(&sy), 4);
        infile.read((char *)(&sz), 4);

        *ydim = (int)sx;
        *xdim = (int)sy;
        *zdim = (int)sz;

        fprintf(stderr, "Volume size: %ix%ix%i\n", *xdim, *ydim, *zdim);

        size = (*xdim)*(*ydim)*(*zdim);

        infile.seekg(532);
        threedp = new unsigned short[size];
        infile.read((char *)(threedp), 2*size);

	infile.close();

        return threedp;

};

Volume<unsigned short> * read_file(char * name){

	unsigned short * data;
	int i, j, k, xdim, ydim, zdim;
	Subscript<3> bounds;
	Volume<unsigned short> * vol;
	
	data = read_rvf_file(name, &xdim, &ydim, &zdim);
	bounds[0] = xdim;
	bounds[1] = ydim;
	bounds[2] = zdim;
	
	vol = new Volume<unsigned short>(bounds);

	for(i=0; i < xdim; i++)
		for(j=0; j < ydim; j++)
			for(k=0; k < zdim; k++){
				vol->setVoxel(i, j, k, *(data + k*xdim*ydim + i*ydim + j));
			}
	
	return vol;
};




int main(int argc, char **argv) {
	int i, j, k;
	unsigned short d;
	float dx, dy, dz;
	Volume<unsigned short> *V;
	Subscript<3> bounds;
	std::LF3Dfull *lf;
	Vector<int> seed(3);
	
// 	bounds[0] = 128;
// 	bounds[1] = 128;
// 	bounds[2] = 128;
	
// 	V = new Volume<unsigned short>(bounds);

	V = read_file("/project1/imtk/data/SPIE2001/west/west.rvf");

	bounds = V->getBounds();

	lf = new std::LF3Dfull(bounds);
	
// 	for(i=0; i < bounds[0]; i++)
// 		for(j=0; j < bounds[1]; j++)
// 			for(k=0; k < bounds[2]; k++){
// 				dx = (float)(i-64);
// 				dy = (float)(j-64);
// 				dz = (float)(k-64);
// 				d = (unsigned short)sqrt(dx*dx + dy*dy + dz*dz);
// 				V->setVoxel(i, j, k, d);
// 			}
			
// 	seed[0] = 64;
// 	seed[1] = 64;
// 	seed[2] = 64;

	seed[0] = 101;
 	seed[1] = 94;
 	seed[2] = 68;

	init_region(*lf, seed, 150.5, *V);

	cerr << "Writing level set function" << endl;
	
	write_LF3D("temp.raw", *lf);

}




