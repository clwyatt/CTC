// File: gdm3dcl.cc
// Abstract: Evolution of the 3D GDM using volume data
//           Command Line Version
//
// Author: Chris L. Wyatt 03/02/2001
//
// NOTES:
//

#include <iostream>

#include "Volume.hh"
#include "Vector.hh"
#include "configFile.hh"
#include "LF3Dfull.hh"
#include "buildTube.hh"
#include "levelSetEvolve.hh"
#include "filter.hh"

//globals for file names to simplify things
char * theVolumeFile;
char * theSpecFile;
char * theCmapFile;
char * theOutputFile;
bool buildOnly = false;
bool isDistance = false;

typedef struct{
	float deltaT;
	float epsilon;
	float sigma;
	float F0;
	int maxIterations;
	string seedfile;
	float thresh;
	float innerR;
	float outerR;
} Parameters;

void usage(){
	cerr << "Usage: ls3dcl -v -spec -cmap -outlsf [-bo]" << endl << endl;
	cerr << "       -v file, the input volume file in rvf format" << endl;
	cerr << "       -spec file, the input specification file" << endl;
	cerr << "       -cmap file, the input conformal mapping file in raw format" << endl;
	cerr << "       -outlsf file, is the output level set file" << endl;
	cerr << "       -dt assume the input volume is a distance transform" << endl;
	cerr << "       -bo just build the initial levelset and exit" << endl << endl;
}

int parseCL(int argc, char **argv)
{
	char * sw;
	int n = 1;
	int vreq = 0, specreq = 0, outlsfreq = 0, sreq = 0;

	while(n < argc){
		sw = *(argv+n);
		if(strncmp(sw, "-v", 3) == 0){
			n+= 1;
			theVolumeFile = *(argv+n);
			vreq = 1;
		}
		else if(strncmp(sw, "-spec", 6) == 0){
			n+= 1;
			theSpecFile = *(argv+n);
			specreq = 1;
		}
		else if(strncmp(sw, "-outlsf", 8) == 0){
			n+= 1;
			theOutputFile = *(argv+n);
			outlsfreq = 1;
		}
		else if(strncmp(sw, "-cmap", 6) == 0){
			n+= 1;
			theCmapFile = *(argv+n);
			sreq = 1;
		}
		else if(strncmp(sw, "-bo", 4) == 0){
			buildOnly = true;
		}
		else if(strncmp(sw, "-dt", 4) == 0){
			isDistance = true;
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
	if((sreq != 1) && (!buildOnly)){
		cerr << "Error: Conformal map file required" << endl << endl;
		usage();
		exit(1);
	}
	if(outlsfreq != 1){
		cerr << "Error: Output level set file required" << endl << endl;
		usage();
		exit(1);
	}
	return 0;

}


// read parameters from specification file
/*example file:
# seed point x, y, z
SEED = 32 32 32
# threshold
THRESHOLD = 150
# time step
TIME_STEP = 0.001
# epsilon
EPSILON = 1.0
# inflation force
INFLATION_FORCE = -1.0
# number of iterations
NUMBER_ITERATIONS = 150
# inner tube radius
INNER_RADIUS = 3.0
# outer tube radius
OUTER_RADIUS = 5.0
*/
void load_spec(char * filename, Parameters &params)
{

	ifstream ifs;
	list<string> output;
	list<string>::iterator it;
	int error;

	ifs.open(filename);

	// read in the seed point file name
	output.clear();
	read_parameter(ifs, string("SEEDFILE"), output, error);
	if(error) goto trap_error;
	if(output.size() != 1){
		cerr << "Error: seed file name should have only 1 value ." << endl;
		exit(-1);
	}
	it = output.begin();
	params.seedfile = *it;

	// read in the threshold
	output.clear();
	read_parameter(ifs, string("THRESHOLD"), output, error);
	if(error) goto trap_error;
	it = output.begin();
	params.thresh = atof((*it).c_str());

	// read in the time step
	output.clear();
	read_parameter(ifs, string("TIME_STEP"), output, error);
	if(error) goto trap_error;
	it = output.begin();
	params.deltaT = atof((*it).c_str());

	// read in the epsilon
	output.clear();
	read_parameter(ifs, string("EPSILON"), output, error);
	if(error) goto trap_error;
	it = output.begin();
	params.epsilon = atof((*it).c_str());

	// read in the inflation force
	output.clear();
	read_parameter(ifs, string("INFLATION_FORCE"), output, error);
	if(error) goto trap_error;
	it = output.begin();
	params.F0 = atof((*it).c_str());

	// read in the number of iterations
	output.clear();
	read_parameter(ifs, string("NUMBER_ITERATIONS"), output, error);
	if(error) goto trap_error;
	it = output.begin();
	params.maxIterations = atoi((*it).c_str());


	// read in the inner tube radius
	output.clear();
	read_parameter(ifs, string("INNER_RADIUS"), output, error);
	if(error) goto trap_error;
	it = output.begin();
	params.innerR = atof((*it).c_str());

	// read in the outer tube radius
	output.clear();
	read_parameter(ifs, string("OUTER_RADIUS"), output, error);
	if(error) goto trap_error;
	it = output.begin();
	params.outerR = atof((*it).c_str());

	ifs.close();

 trap_error:
	if(error){
		cerr << "Error number " << error << " while reading specification file." << endl;
		exit(-1);
	}

	if(params.maxIterations < 0){
		cerr << "Error: Invalid number of iterations." << endl;
		exit(-1);
	}
	if( (params.innerR <= 1) || (params.outerR <= 1)){
		cerr << "Error: tube radus should be greater than 1." << endl;
		exit(-1);
	}
	if(params.innerR +1 > params.outerR){
		cerr << "Error: outer tube radus should be at least 1 greater than inner radius." << endl;
		exit(-1);
	}
}


void load_seed_file(const string &filename, list< IMTK::Subscript<3> > &seeds)
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

void read_rvf_file(char *name, IMTK::Volume<unsigned short> &vol)
{
        ifstream  infile;
	IMTK::Subscript<3> bounds;
	unsigned short temp;
        long sx, sy, sz;
        int i, j, k;

	infile.open( name );

	if(!infile){
		cerr << "Error opening rvf file " << name << endl;
		exit(-1);
	}

        infile.seekg(12);

        infile.read((char *)(&sx), 4);
        infile.read((char *)(&sy), 4);
        infile.read((char *)(&sz), 4);

        bounds[0] = (int)sx;
        bounds[1] = (int)sy;
        bounds[2] = (int)sz;

	clog << "Volume size: " <<  bounds[0] << " x " << bounds[1] << " x " << bounds[2] << endl;

        infile.seekg(532);

	vol.setBounds(bounds);

	for(k=0; k < bounds[2]; k++)
		for(j=0; j < bounds[1]; j++)
			for(i=0; i < bounds[0]; i++){
				infile.read((char *)(&temp), 2);
				vol.setVoxel(i, j, k, temp);
			}

	infile.close();

};

float read_rawf_file(char *name, IMTK::Volume<float> &vol)
{
        ifstream  infile;
	IMTK::Subscript<3> bounds;
	float temp, max;
        int i, j, k;

	bounds = vol.getBounds();

	infile.open( name );

	if(!infile){
		cerr << "Error opening rawf file " << name << endl;
		exit(-1);
	}

	max = -1;
	for(k=0; k < bounds[2]; k++)
		for(j=0; j < bounds[1]; j++)
			for(i=0; i < bounds[0]; i++){
				infile.read((char *)(&temp), 4);
				if(temp > max) max = temp;
				vol.setVoxel(i, j, k, temp);
			}

	infile.close();

	return max;
};

int main(int argc, char **argv) {

	// parameters
	Parameters params;

	// data objects
	IMTK::Volume<unsigned short> volume;
	IMTK::Volume<float> cmap;
	list< IMTK::Subscript<3> > seeds;

	// the level set function
	LF3Dfull levelFunction;
	
	// parse the command line
	parseCL(argc, argv);

	// read and parse spec file
	clog << "Parsing specification file." << endl;
	load_spec(theSpecFile, params);
	levelFunction.setInnerR(params.innerR);
	levelFunction.setOuterR(params.outerR);

	clog << "Parsing seed file." << endl;
	load_seed_file(params.seedfile, seeds);

	// load volume file
	clog << "Reading volume data." << endl;
	read_rvf_file(theVolumeFile, volume);

// 	clog << "Smoothing volume data." << endl;
// 	gaussian_smooth(volume, 0.65);

	// make level set function same size as volume
	levelFunction.setBounds(volume.getBounds());

	clog << "Building Tube." << endl;
	if(isDistance)
		init_region_dist(levelFunction, seeds, params.thresh, volume);
	else
		init_region(levelFunction, seeds, params.thresh, volume);

	if(buildOnly){
		levelFunction.dump(theOutputFile);
		exit(EXIT_SUCCESS);
	}
	clog << "Reading conformal mapping." << endl;
	cmap.setBounds(levelFunction.getBounds());
	float cmapMax = read_rawf_file(theCmapFile, cmap);

	params.deltaT = 0.5/cmapMax;

	clog << "Beginning evolution of 3D GDM equation." << endl;
	levelSetEvolve(levelFunction, cmap, params.maxIterations, params.deltaT, params.epsilon, params.F0);
	clog << "Done." << endl;

	// write out the final level set
	clog << "Writing level set output." << endl;
	levelFunction.dump(theOutputFile);
}




