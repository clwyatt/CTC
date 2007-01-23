// File: seedloc.cc
// Abstract: automatic seed coordinate locator
//
// Author: Chris L. Wyatt 1999
//
// NOTES: See SPIE paper "Automatic Segmentation of the Colon" for reference
//

#include <iostream>
#include <list>

#include "Volume.hh"
#include "DataIO.hh"

#include "configFile.hh"
#include "mask.hh"
#include "distance.hh"

using namespace std;

//globals for file names to simplify things
char * volume_file;
char * spec_file;
char * seed_file;

typedef struct{
	float seedMaxThresh;
	float tissueThresh;
} Parameters;

void usage(){
	cerr << "Usage: seedloc -v -spec -sf" << endl << endl;
	cerr << "       -v file, the input volume file in rvf or DICOM DIR format" << endl;
	cerr << "       -spec file, the input specification file" << endl;
	cerr << "       -sf file, the name of the seed file to generate" << endl << endl;
}

int parseCL(int argc, char **argv)
{
	char * sw;
	int n = 1;
	int vreq = 0, specreq = 0, sfreq = 0;

	while(n < argc){
		sw = *(argv+n);
		if(strncmp(sw, "-v", 3) == 0){
			n+= 1;
			volume_file = *(argv+n);
			vreq = 1;
		}
		else if(strncmp(sw, "-spec", 6) == 0){
			n+= 1;
			spec_file = *(argv+n);
			specreq = 1;
		}
		else if(strncmp(sw, "-sf", 4) == 0){
			n+= 1;
			seed_file = *(argv+n);
			sfreq = 1;
		}
		else{
			usage();
			exit(1);
		}
		     
		n+= 1;
	}

	if(vreq != 1){
		cerr << "Error: Volume file required" << endl << endl;
		usage();
		exit(1);
	}
	if(specreq != 1){
		cerr << "Error: Specification file required" << endl << endl;
		usage();
		exit(1);
	}
	if(sfreq != 1){
		cerr << "Error: Seed output file required" << endl << endl;
		usage();
		exit(1);
	}
	return 0;

}


// read parameters from specification file
/*example file:
SEED_MAX_THRESH = 10
TISSUE_THRESH = 220;
*/
void load_spec(char * filename, Parameters &params){

	ifstream ifs;
	list<string> output;
	list<string>::iterator it;
	int error;

	ifs.open(filename);

	// read in the seed max distance threshold
	output.clear();
	read_parameter(ifs, string("SEED_MAX_THRESH"), output, error);
	if(error) goto trap_error;
	it = output.begin();
	params.seedMaxThresh = atof((*it).c_str());

	// read in the tissue threshold
	output.clear();
	read_parameter(ifs, string("TISSUE_THRESH"), output, error);
	if(error) goto trap_error;
	it = output.begin();
	params.tissueThresh = atof((*it).c_str());

 trap_error:

	ifs.close();

	if(error){
		cerr << "Error number " << error << " while reading specification file." << endl;
		exit(-1);
	}

	// check bounds on parameters
}

int main(int argc, char **argv) {

	// parameters
	Parameters params;

	// data objects
	IMTK::Volume<unsigned short> volume;
	IMTK::Volume<unsigned short> region;
	
	// list of seed points
	IMTK::Subscript<3> seedp;
	list< IMTK::Subscript<3> > seedl;

	// parse the command line
	parseCL(argc, argv);

	// load the spec file
	load_spec(spec_file, params);

	// load volume file
	int error;
	if( IMTK::isRVF(volume_file, error) ){
		IMTK::readRVF(volume_file, volume, true, error);
		if(error){
			cerr << "Error " << error << " reading " << volume_file << endl;
			exit(EXIT_FAILURE);
		}
	}
	else{
		cerr << "Volume file not RVF" << endl;
		exit(EXIT_FAILURE);
	}

	IMTK::Subscript<3> bounds = volume.getBounds();

	// set up the region mask and dt as same size
	float dx, dy, dz;
	float xo, yo, zo;
	region.setBounds( bounds );
	volume.getVoxelSpacing(dx, dy, dz);
	region.setVoxelSpacing(dx, dy, dz);
	volume.getOrigin(xo, yo, zo);
	region.setOrigin(xo, yo, zo);

	// mask background and tissue
	maskBg(volume, region, params.tissueThresh);

	// compute the distance transform
	computeDT(region);

	writeRVF(string("distance.rvf"), region, error);
	//region.dump("distance.raw");

	// loop selecting maximum, growing, and masking
	// assume at least one section of lumen is present
	unsigned short seedMax, val;
	int i, j, k, mx, my, mz;
	do{
		// locate maximum distance
		seedMax = region[0];
		mx = 0; my = 0; mz = 0;
		for(i = 0; i < bounds[0]; i++)
			for(j = 0; j < bounds[1]; j++)
				for(k = 0; k < bounds[2]; k++){
					val = region.getVoxel(i,j,k);
					if(val > seedMax){
						seedMax = val;
						mx = i; my = j; mz = k;
					}
				}

		clog << "Seed located: (" << mx << ", " << my << ", " << mz << ") value = " << seedMax << endl;

		if(seedMax > params.seedMaxThresh){
			seedp[0] = mx; seedp[1] = my; seedp[2] = mz;
			seedl.push_front(seedp);
		}

		// mask connected region
		maskRegion(mx, my, mz, region);

	}while(seedMax > params.seedMaxThresh);

	//write out seed file
	ofstream outfile;
	outfile.open(seed_file);
	if(!outfile){
		cerr << "seedloc: ERROR opening seed file " << seed_file << " for writing." << endl;
		exit(EXIT_FAILURE);
	}
	list< IMTK::Subscript<3> >::iterator sit = seedl.begin();
	int num = 0;
	while( sit != seedl.end() ){
		outfile << num << " " << (*sit)[0] << " " << (*sit)[1] << " " << (*sit)[2] << endl;
		num++;
		sit++;
	}
	outfile.close();
}


