// File: rvfswab.cc
// Abstract: convert rvf from little to big endian
//
// Created: 11/29/99 by: Chris L. Wyatt
//
//                

#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

void swapshort(short * val)
{
  char swapped[2];
  swapped[0] = *((char*)val+1);
  swapped[1] = *((char*)val);

  *val = *((short *) swapped);
};

void swaplong(long * val)
{
  char swapped[4];
  swapped[0] = *((char*)val+3);
  swapped[1] = *((char*)val+2);
  swapped[2] = *((char*)val+1);
  swapped[3] = *((char*)val);

  *val = *((long *) swapped);
};
void swapfloat(float * val)
{
  char swapped[4];
  swapped[0] = *((char*)val+3);
  swapped[1] = *((char*)val+2);
  swapped[2] = *((char*)val+1);
  swapped[3] = *((char*)val);

  *val = *((float *) swapped);
};

/*
* main routine
*/

int main(int argv, char **argc)
{
	char infilename[100];
	char outfilename[100];
        ifstream  infile;
	ofstream  outfile;
        long sx, sy, sz, bitdepth;
	float dx, dy, dz, xo, yo, zo;
	unsigned char b1, b2, b3, b4;

	//parse command line
	if(argv == 3){
		strcpy(infilename, *(argc+1));
		strcpy(outfilename, *(argc+2));
	}
	else{
		cerr << "rvfswab ERROR: no file name(s) given" << endl;
		cerr << "Usage: rvfswab inputfilename outputfilename" << endl << endl;
		exit(EXIT_FAILURE);
	}

	infile.open( infilename );
	if(!infile){
		cerr << "rvfinfo ERROR: could not open file " << infilename << " for reading" << endl;
		exit(EXIT_FAILURE);
	}

	outfile.open( infilename );
	if(!outfile){
		cerr << "rvfinfo ERROR: could not open file " << outfilename << " for writing" << endl;
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

	outfile.write((char *)&b1, 1);
	outfile.write((char *)&b2, 1);
	outfile.write((char *)&b3, 1);
	outfile.write((char *)&b4, 1);

        infile.seekg(12);
        infile.read((char *)(&sx), 4);
        infile.read((char *)(&sy), 4);
        infile.read((char *)(&sz), 4);
	infile.read((char *)(&bitdepth), 4);

	swaplong(&sx);
	swaplong(&sy);
	swaplong(&sz);
	swaplong(&bitdepth);


	outfile.write((char *)&sx, 4);
	outfile.write((char *)&sy, 4);
	outfile.write((char *)&sz, 4);
	outfile.write((char *)&bitdepth, 4);

	// write flip display indications (I ignore these)
	long flip = 0;
	outfile.write((char *)&flip, 4);
	outfile.write((char *)&flip, 4);
	outfile.write((char *)&flip, 4);
	outfile.write((char *)&flip, 4);
	outfile.write((char *)&flip, 4);
	outfile.write((char *)&flip, 4);

	infile.seekg(52);
        infile.read((char *)(&xo), sizeof(float));
        infile.read((char *)(&dx), sizeof(float));
        infile.read((char *)(&yo), sizeof(float));
        infile.read((char *)(&dy), sizeof(float));
        infile.read((char *)(&zo), sizeof(float));
        infile.read((char *)(&dz), sizeof(float));

	swapfloat(&xo); swapfloat(&yo); swapfloat(&zo);
	swapfloat(&dx); swapfloat(&dy); swapfloat(&dz);

	outfile.write((char *)(&xo), sizeof(float));
        outfile.write((char *)(&dx), sizeof(float));
        outfile.write((char *)(&yo), sizeof(float));
        outfile.write((char *)(&dy), sizeof(float));
        outfile.write((char *)(&zo), sizeof(float));
        outfile.write((char *)(&dz), sizeof(float));

	clog << "Raw Volume File        : " << infilename << endl;
	clog << "Volume size            : " << 2*sx*sy*sz << " bytes" << endl;
	clog << "       dimensions      : " << sx << " x " << sy << " x " << sz << endl;
	clog << "       dimensions (mm) : " << sx*dx << " x " << sy*dy << " x " << sz*dz << endl;
	clog << "       origin          : " << xo << ", " << yo << ", " << zo << endl;

	// skip rest of header for now
        infile.seekg(532);

	// write out patient information (research ignores all this)
	char patient[448];
	memset(patient, 0, 448);
	outfile.write((char *)(&patient), 448);
	// write out two zeros 'cause we don't add dicom headers to the end
	outfile.write((char *)(&patient), 8); // reuse block of zeros from patient

	unsigned short temp;
	for(int k=0; k < sx*sy*sz; k++){
	  infile.read((char *)(&temp), 2);
	  swapshort(&temp);
	  outfile.write((char *) &temp, 2);
	}

	outfile.close();
	infile.close();

}


