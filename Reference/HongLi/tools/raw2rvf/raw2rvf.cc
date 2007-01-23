// File: raw2rvf.cc
// Abstract: 
//
// Author: Chris L. Wyatt 
//
// NOTES: 
//

#include <iostream>

#include "Volume.hh"
#include "DataIO.hh"

//globals to simplify things
char * raw_file;
char * rvf_file;
int xdim, ydim, zdim;
float xo, yo, zo;
float dx, dy, dz;

void usage(){
	cerr << "Usage: raw2rvf -v -o -xdim -ydim -zdim -xo -yo -zo -dx -dy -dz" << endl << endl;
	cerr << "       -v file, the input volume file in raw format" << endl;
	cerr << "       -o output file in rvf format" << endl;
	cerr << "       -xdim number of voxels in the x direction" << endl;
	cerr << "       -ydim number of voxels in the y direction" << endl;
	cerr << "       -zdim number of voxels in the z direction" << endl;
	cerr << "       -xo coordinate of x origin" << endl;
	cerr << "       -yo coordinate of y origin" << endl;
	cerr << "       -zo coordinate of z origin" << endl;
	cerr << "       -dx x sampling resolution" << endl;
	cerr << "       -dy y sampling resolution" << endl;
	cerr << "       -dz z sampling resolution" << endl;
	cerr << "       " << endl << endl;
}

void parseCL(int argc, char **argv)
{
	char * sw;
	int n = 1;
	bool vreq = false, oreq = false;
	bool xdimreq = false, ydimreq = false, zdimreq = false;
	bool xoreq = false, yoreq = false, zoreq = false;
	bool dxreq = false, dyreq = false, dzreq = false;

	while(n < argc){
		sw = *(argv+n);
		if(strncmp(sw, "-v", 3) == 0){
			n+= 1;
			raw_file = *(argv+n);
			vreq = true;
		}
		else if(strncmp(sw, "-o", 3) == 0){
			n+= 1;
			rvf_file = *(argv+n);
			oreq = true;
		}
		else if(strncmp(sw, "-xdim", 6) == 0){
			n+= 1;
			xdim = atoi(*(argv+n));
			xdimreq = true;
		}
		else if(strncmp(sw, "-ydim", 6) == 0){
			n+= 1;
			ydim = atoi(*(argv+n));
			ydimreq = true;
		}
		else if(strncmp(sw, "-zdim", 6) == 0){
			n+= 1;
			zdim = atoi(*(argv+n));
			zdimreq = true;
		}
		else if(strncmp(sw, "-xo", 4) == 0){
			n+= 1;
			xo = atof(*(argv+n));
			xoreq = true;
		}
		else if(strncmp(sw, "-yo", 4) == 0){
			n+= 1;
			yo = atof(*(argv+n));
			yoreq = true;
		}
		else if(strncmp(sw, "-zo", 4) == 0){
			n+= 1;
			zo = atof(*(argv+n));
			zoreq = true;
		}
		else if(strncmp(sw, "-dx", 4) == 0){
			n+= 1;
			dx = atof(*(argv+n));
			dxreq = true;
		}
		else if(strncmp(sw, "-dy", 4) == 0){
			n+= 1;
			dy = atof(*(argv+n));
			dyreq = true;
		}
		else if(strncmp(sw, "-dz", 4) == 0){
			n+= 1;
			dz = atof(*(argv+n));
			dzreq = true;
		}
		else{
			usage();
			exit(EXIT_FAILURE);
		}
		     
		n+= 1;
	}

	if(!vreq){
		cerr << "Error: Volume file required" << endl << endl;
		usage();
		exit(EXIT_FAILURE);
	}
	if(!oreq){
		cerr << "Error: Output file required" << endl << endl;
		usage();
		exit(EXIT_FAILURE);
	}
	if(!xdimreq){
		cerr << "Error: xdim required" << endl << endl;
		usage();
		exit(EXIT_FAILURE);
	}
	if(!ydimreq){
		cerr << "Error: ydim required" << endl << endl;
		usage();
		exit(EXIT_FAILURE);
	}
	if(!zdimreq){
		cerr << "Error: zdim required" << endl << endl;
		usage();
		exit(EXIT_FAILURE);
	}
	if(!xoreq){
		cerr << "Error: xo required" << endl << endl;
		usage();
		exit(EXIT_FAILURE);
	}
	if(!yoreq){
		cerr << "Error: yo required" << endl << endl;
		usage();
		exit(EXIT_FAILURE);
	}
	if(!zoreq){
		cerr << "Error: zo required" << endl << endl;
		usage();
		exit(EXIT_FAILURE);
	}
	if(!dxreq){
		cerr << "Error: dx required" << endl << endl;
		usage();
		exit(EXIT_FAILURE);
	}
	if(!dyreq){
		cerr << "Error: dy required" << endl << endl;
		usage();
		exit(EXIT_FAILURE);
	}
	if(!dzreq){
		cerr << "Error: dz required" << endl << endl;
		usage();
		exit(EXIT_FAILURE);
	}

	// check parameters
	// xdim, ydim and zdim should be positive
	if( xdim <= 0 ){
		cerr << "Error: xdim must be positive" << endl << endl;
		usage();
		exit(EXIT_FAILURE);
	} 
	if( ydim <= 0){
		cerr << "Error: ydim must be positive" << endl << endl;
		usage();
		exit(EXIT_FAILURE);
	}
	if( zdim <= 0 ){
		cerr << "Error: zdim must be positive" << endl << endl;
		usage();
		exit(EXIT_FAILURE);
	}
	
	// dx, dy and dz should be positive
	if( dx <= 0 ){
		cerr << "Error: dx must be positive" << endl << endl;
		usage();
		exit(EXIT_FAILURE);
	} 
	if( dy <= 0){
		cerr << "Error: dy must be positive" << endl << endl;
		usage();
		exit(EXIT_FAILURE);
	}
	if( dz <= 0 ){
		cerr << "Error: dz must be positive" << endl << endl;
		usage();
		exit(EXIT_FAILURE);
	}

}

void read_rawus_file(char *name, IMTK::Volume<unsigned short> &vol)
{
        ifstream  infile;
	IMTK::Subscript<3> bounds;
	unsigned short temp;
        int i, j, k;

	bounds = vol.getBounds();

	infile.open( name );

	if(!infile){
		cerr << "Error opening raw file " << name << endl;
		exit(-1);
	}
	
	for(k=0; k < bounds[2]; k++)
		for(j=0; j < bounds[1]; j++)
			for(i=0; i < bounds[0]; i++){
				infile.read((char *)(&temp), 2);
				vol.setVoxel(i, j, k, temp);
			}

	infile.close();
};

int main(int argc, char **argv) {

	// data objects
	IMTK::Volume<unsigned short> volume;
	
	// parse the command line
	parseCL(argc, argv);

	// set bounds to correct values
	IMTK::Subscript<3> bounds;
	bounds[0] = xdim;
	bounds[1] = ydim;
	bounds[2] = zdim;
	volume.setBounds(bounds);

	// set the origin and spacing to correct values
	volume.setVoxelSpacing(dx, dy, dz);
	volume.setOrigin(xo, yo, zo);
		       
	// load raw volume file
	read_rawus_file(raw_file, volume);
	
	//write cropped output file
	int error;
	IMTK::writeRVF(rvf_file, volume, error);	
	
	// check for write errors
	if(error != 0){
		cerr << "Error: Volume file " << rvf_file << " could not be written." << endl;
		exit(EXIT_FAILURE);
	}
}


