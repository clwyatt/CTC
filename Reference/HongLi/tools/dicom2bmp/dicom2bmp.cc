// File: dicom2rvf.cc
// Abstract: convert dicom files to sigle rvf
//
// Created: 02/26/02 by: Chris L. Wyatt
//
//                

#include <iostream>
#include <fstream>
#include <cstdlib>

#include "Volume_ext.hh"
#include "DataIO.hh"
#include "Dicom.hh"

using namespace IMTK;
void  readDicom(char * name, Volume<unsigned short> &data, int error);

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
		cerr << "dicom2rvf ERROR: one or more file names not given" << endl;
		cerr << "Usage: dicom2rvf dicom.file rvf.file" << endl << endl;
		exit(EXIT_FAILURE);
	}


	if(!((strstr(infilename, ".DCM"))!=NULL ||
		(strstr(infilename, ".dcm"))!=NULL ||
		(strstr(infilename, ".IMA"))!=NULL ||
		(strstr(infilename, ".ima"))!=NULL))
	{
		cerr << "dicom2bmp ERROR: not a valid dicom exteneion(DCM dcm IMA ima)" << endl;
		cerr << "Usage: dicom2bmp dicom.file bmp.file" << endl << endl;
		exit(EXIT_FAILURE);
	}
		
	Volume_ext<unsigned short> data;
	int error;

	// attempt dicom read
	readDicom(infilename, data, error);
	Volume_ext <unsigned short>::AxialSlice as = data.axial(0);
	ofstream sfile;
	sfile.open(outfilename);
	as.write(sfile, 0 , 4096, -1, -1);
	sfile.close();
	if(error) exit(EXIT_FAILURE);


	return 0;
}

void  readDicom(char * name, Volume<unsigned short> &data, int error)
{
	bool success;

	error = 0;

	// the dicom object and temp holders
	Dicom dcmObj;
	float xo, yo, zo; //origin
	unsigned short xdim, ydim, zdim; // slice dimensions
	float dx, dy, dz; // in-plane voxel size
	int slice; // the slice number

	success = dcmObj.open(name);
	
	if( !success ){
		cerr << "Dicom file : " << name << " could not be opened " << endl;
		error = IMTK::FILE_OPEN_ERROR;
		return;
	}
	success = dcmObj.getOrigin(xo, yo, zo); //origin of this slice
	if( !success ){
		cerr << "Dicom file : " << name << " parse error " << endl;
		error = IMTK::FILE_PARSE_ERROR;
		dcmObj.close();
		return;
	}
	success = dcmObj.getDims(xdim, ydim); //slice dimensions
	if( !success ){
		cerr << "Dicom file : " << name << " parse error " << endl;
		error = IMTK::FILE_PARSE_ERROR;
		dcmObj.close();
		return;
	}
	success = dcmObj.getPxlSize(dx, dy); //in-plane voxel size
	if( !success ){
		cerr << "Dicom file : " << name << " parse error " << endl;
		error = IMTK::FILE_PARSE_ERROR;
		dcmObj.close();
		return;
	}
	
	float x1, y1, z1, x2, y2, z2;
	success = dcmObj.getImageOrientP(x1, y1, z1, x2, y2, z2); //patient orientation
	if(success ){ //currently we take care the flip situation
		if (x1== -1) dx = (-1) * dx; 
		if (y2== -1) dy = (-1) * dy; 		
	}

	success = dcmObj.getSliceNum(slice); //the slice number
	if( !success ){
		cerr << "Dicom file : " << name << " parse error " << endl;
		error = IMTK::FILE_PARSE_ERROR;
		dcmObj.close();
		return;
	}

	// intialize the volume
	IMTK::Subscript<3> bounds;
	bounds[0] = xdim;
	bounds[1] = ydim;
	bounds[2] = 1;
	data.setBounds(bounds);
	data.setVoxelSpacing(dx, dy, dz);
	data.setOrigin(xo, yo, zo);
	
	clog << "Volume size: " <<  bounds[0] << " x " << bounds[1] << " x " << bounds[2];
	clog << "       (mm): " << bounds[0]*dx << "mm x " << bounds[1]*dy << "mm x " << bounds[2]*dz << "mm";
	clog << "     origin: " << xo << ", " << yo << ", " << zo << endl;

	// store each slice in the volume
	unsigned int i, j, k;
	unsigned short * slicedata;

	slicedata = new unsigned short[xdim*ydim];
	success = dcmObj.getData(slicedata); 
	if( !success ){
		cerr << "Dicom file : " << name << " parse error " << endl;
		error = IMTK::FILE_PARSE_ERROR;
		dcmObj.close();
		delete [] slicedata;
		return;
		}
	dcmObj.close();

	// put in the volume
	for(i=0; i < bounds[0]; i++)
		for(j=0; j < bounds[1]; j++){
			data.setVoxel(i, j, 0, slicedata[j*xdim + i]);
		}
// free up slice data
	delete [] slicedata;
}

