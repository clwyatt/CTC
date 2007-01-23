/********************************************************
File: DataIO.cc
Abstract: Functions to read/write Data objects to 
          files, pipes, etc

See header (DataIO.hh) for documentation

Last Revision ($Revision: 1.2 $) 
by $Author: hongli $ 
on $Date: 2003/10/02 19:29:08 $
*********************************************************/

#include "DataIO.hh"
#include "Dicom.hh"
//#include "dctk.h"

#ifdef HAVE_ZLIB
#include <zlib.h>
#endif
//#define swap_order
#include "bytereverse.hh"

#include <iostream>
#include <fstream>
#include <list>

bool IMTK::isCompressed(const string &fileName, int &error){

        ifstream  infile;
        unsigned char b1, b2;
	bool status = false;

	error = 0;

	infile.open( fileName.c_str() );

	if(!infile){
		cerr << "Error opening file: " << fileName.c_str() << endl;
		error = IMTK::FILE_OPEN_ERROR;
		return false;
	}

        infile.read((char *)(&b1), 1);
        infile.read((char *)(&b2), 1);

	infile.close();

	status = false;
	if( (b1 == 0x1f) && (b2 == 0x8b) ) status = true;

	return status;
};


bool IMTK::isRVF(const string &fileName, int &error){

#ifdef HAVE_ZLIB
        gzFile infilegz;
#endif
        ifstream  infile;
	unsigned char b1, b2, b3, b4;
	bool status = false;

	error = 0;

	if(IMTK::isCompressed(fileName, error)){
#ifdef HAVE_ZLIB
		infilegz = gzopen(fileName.c_str(), "r");
		
		if(infilegz == NULL){
			cerr << "Error opening rvf(?) file " << fileName.c_str() << endl;
			error = IMTK::FILE_OPEN_ERROR;
			return status;
		}
		
		gzread(infilegz, (char *)(&b1), 1);
		gzread(infilegz, (char *)(&b2), 1);
		gzread(infilegz, (char *)(&b3), 1);
		gzread(infilegz, (char *)(&b4), 1);

		gzclose(infilegz);
#else
		cerr << "Reading Compressed file " << fileName.c_str() << " unsupported" << endl;
		error = IMTK::FILE_OPEN_ERROR;
		return status;
#endif
	}
	else{
		infile.open( fileName.c_str() );
		
		if(!infile){
			cerr << "Error opening rvf(?) file " << fileName.c_str() << endl;
			error = IMTK::FILE_OPEN_ERROR;
			return status;
		}
		
		infile.read((char *)(&b1), 1);
		infile.read((char *)(&b2), 1);
		infile.read((char *)(&b3), 1);
		infile.read((char *)(&b4), 1);

		infile.close();
	}

	if( (b1 == 100) && (b2 == 118) && (b3 == 118) && (b4 == 114) )
		status = true;

	return status;
};


void IMTK::readRVF(const string &fileName, Volume<unsigned short> &data, bool mask, int &error){

        ifstream  infile;
	IMTK::Subscript<3> bounds;
	unsigned short temp;
        long sx, sy, sz, bitdepth;
	float dx, dy, dz, xo, yo, zo;
        unsigned int i, j, k;

	error = 0;

	infile.open( fileName.c_str() );

	if(!infile){
		cerr << "Error opening rvf(?) file " << fileName.c_str() << endl;
		error = IMTK::FILE_OPEN_ERROR;
		return;
	}

        infile.seekg(12);

        infile.read((char *)(&sx), 4);
        infile.read((char *)(&sy), 4);
        infile.read((char *)(&sz), 4);
	infile.read((char *)(&bitdepth), 4);

#ifdef swap_order
        bounds[0] = byte_reverse(sx);
        bounds[1] = byte_reverse(sy);
        bounds[2] = byte_reverse(sz);
#else
        bounds[0] = sx;
        bounds[1] = sy;
        bounds[2] = sz;

#endif	
	data.setBounds(bounds);

	infile.seekg(52);
        infile.read((char *)(&xo), sizeof(float));
        infile.read((char *)(&dx), sizeof(float));
        infile.read((char *)(&yo), sizeof(float));
        infile.read((char *)(&dy), sizeof(float));
        infile.read((char *)(&zo), sizeof(float));
        infile.read((char *)(&dz), sizeof(float));
	
	if( (dx != 0) && (dy != 0) && (dz != 0) ){
		data.setVoxelSpacing(dx, dy, dz);
	}
	data.setOrigin(xo, yo, zo);

	clog << "Volume size: " <<  bounds[0] << " x " << bounds[1] << " x " << bounds[2];
	clog << "       (mm): " << bounds[0]*dx << "mm x " << bounds[1]*dy << "mm x " << bounds[2]*dz << "mm";
	clog << "     origin: " << xo << ", " << yo << ", " << zo << endl;

	// skip rest of header for now
        infile.seekg(532);

	for(k=0; k < bounds[2]; k++)
		for(j=0; j < bounds[1]; j++)
			for(i=0; i < bounds[0]; i++){
				infile.read((char *)(&temp), 2);
				if(mask) temp = temp & 07777;
				data.setVoxel(i, j, k, temp);
			}

	infile.close();
};

void IMTK::readRVFheader(const string &fileName, float &xo, float &yo, float &zo, 
float & dx, float &dy, float &dz , int & xdim, int &ydim, int &zdim, int &error){

        ifstream  infile;
	IMTK::Subscript<3> bounds;
	unsigned short temp;
        long sx, sy, sz, bitdepth;
        unsigned int i, j, k;

	error = 0;

	infile.open( fileName.c_str() );

	if(!infile){
		cerr << "Error opening rvf(?) file " << fileName.c_str() << endl;
		error = IMTK::FILE_OPEN_ERROR;
		return;
	}

        infile.seekg(12);

        infile.read((char *)(&sx), 4);
        infile.read((char *)(&sy), 4);
        infile.read((char *)(&sz), 4);
	infile.read((char *)(&bitdepth), 4);

#ifdef swap_order
        xdim = byte_reverse(sx);
        ydim = byte_reverse(sy);
        zdim = byte_reverse(sz);
#else
        xdim = (int) sx;
        ydim = (int) sy;
        zdim = (int) sz;

#endif	

	infile.seekg(52);
        infile.read((char *)(&xo), sizeof(float));
        infile.read((char *)(&dx), sizeof(float));
        infile.read((char *)(&yo), sizeof(float));
        infile.read((char *)(&dy), sizeof(float));
        infile.read((char *)(&zo), sizeof(float));
        infile.read((char *)(&dz), sizeof(float));
	infile.close();
	clog << "Volume size: " <<  sx << " x " << sy << " x " <<sz;
	clog << "       (mm): " << sx*dx << "mm x " <<sy*dy << "mm x " << sz*dz << "mm";
	clog << "     origin: " << xo << ", " << yo << ", " << zo << endl;

};

#ifdef HAVE_ZLIB
void IMTK::readRVFgz(const string &fileName, Volume<unsigned short> &data, bool mask, int &error){

        gzFile infile;
	IMTK::Subscript<3> bounds;
	unsigned short temp;
        long sx, sy, sz;
        unsigned int i, j, k;

	error = 0;

	infile = gzopen(fileName.c_str(), "r");

	if(infile == NULL){
		cerr << "Error opening rvf(?) file " << fileName.c_str() << endl;
		error = IMTK::FILE_OPEN_ERROR;
		return;
	}

	gzseek(infile, 12, SEEK_SET);

	gzread(infile, (char *)(&sx), 4);
	gzread(infile, (char *)(&sy), 4);
	gzread(infile, (char *)(&sz), 4);

        bounds[0] = (int)sx;
        bounds[1] = (int)sy;
        bounds[2] = (int)sz;

	clog << "Volume size: " <<  bounds[0] << " x " << bounds[1] << " x " << bounds[2] << endl;

	// skip rest of header for now
	gzseek(infile, 532, SEEK_SET);

	data.setBounds(bounds);

	for(k=0; k < bounds[2]; k++)
		for(j=0; j < bounds[1]; j++)
			for(i=0; i < bounds[0]; i++){
				gzread(infile, (char *)(&temp), 2);
				if(mask) temp = temp & 07777;
				data.setVoxel(i, j, k, temp);
			}

	gzclose(infile);

};
#endif


void IMTK::writeRVF(const string &fileName, Volume<unsigned short> &data, int &error){

	// initialize
	error = 0;

	// open the file stream
        ofstream  outfile;
	outfile.open( fileName.c_str() );
	if(!outfile){
		cerr << "Error opening rvf(?) file " << fileName.c_str() << endl;
		error = IMTK::FILE_OPEN_ERROR;
		return;
	}
	cout <<"Start writing..."<<endl;
	// write out the magic number (b1 == 100) && (b2 == 118) && (b3 == 118) && (b4 == 114)
	unsigned char b1, b2, b3, b4;
	b1 = 100; b2 = 118; b3 = 118; b4 = 114;
	outfile.write((char *)&b1, 1);
	outfile.write((char *)&b2, 1);
	outfile.write((char *)&b3, 1);
	outfile.write((char *)&b4, 1);

	// write out the version
	float version = 1.6;
	outfile.write((char *)&version, 4);

	// write a serial number
	long serial = 0;
	outfile.write((char *)&serial, 4);

	// write the volume dimensions
	long xdim, ydim, zdim, bits;
	IMTK::Subscript<3> bounds;
	bounds = data.getBounds();
	xdim = bounds[0];
	ydim = bounds[1];
	zdim = bounds[2];
	bits = 16;
	outfile.write((char *)&xdim, 4);
	outfile.write((char *)&ydim, 4);
	outfile.write((char *)&zdim, 4);
	outfile.write((char *)&bits, 4);

	// write flip display indications (I ignore these)
	long flip = 0;
	outfile.write((char *)&flip, 4);
	outfile.write((char *)&flip, 4);
	outfile.write((char *)&flip, 4);
	outfile.write((char *)&flip, 4);
	outfile.write((char *)&flip, 4);
	outfile.write((char *)&flip, 4);

	// write out the origin and voxel dimensions
	float xo, yo, zo;
	float dx, dy, dz;
	data.getOrigin(xo, yo, zo);
	data.getVoxelSpacing(dx, dy, dz);
        outfile.write((char *)(&xo), sizeof(float));
        outfile.write((char *)(&dx), sizeof(float));
        outfile.write((char *)(&yo), sizeof(float));
        outfile.write((char *)(&dy), sizeof(float));
        outfile.write((char *)(&zo), sizeof(float));
        outfile.write((char *)(&dz), sizeof(float));

	clog << "Volume size: " <<  bounds[0] << " x " << bounds[1] << " x " << bounds[2];
	clog << "       (mm): " << bounds[0]*dx << "mm x " << bounds[1]*dy << "mm x " << bounds[2]*dz << "mm";
	clog << "     origin: " << xo << ", " << yo << ", " << zo << endl;

	// write out patient information (research ignores all this)
	char patient[448];
	memset(patient, 0, 448);
	outfile.write((char *)(&patient), 448);

	// write out two zeros 'cause we don't add dicom headers to the end
	outfile.write((char *)(&patient), 8); // reuse block of zeros from patient

	// now, write out the data
	unsigned short ustemp;
	for(int i = 0; i < data.size(); i++){
		ustemp = data[i];
		outfile.write((char *) &ustemp, 2);
		if( outfile.bad() ){
			error = FILE_WRITE_ERROR;
			break;
		}
	}

	// close up
	outfile.close();
}

bool IMTK::isMatlab(const string &fileName, int &error)
{

	ifstream  infile;
	unsigned char b1, b2;
	bool status = false;

	error = 0;

	infile.open( fileName.c_str() );
		
	if(!infile){
		cerr << "Error opening Matlab(?) file " << fileName.c_str() << endl;
		error = IMTK::FILE_OPEN_ERROR;
		return status;
	}
	
	infile.seekg(124); // skip text part of header
	infile.read((char *)(&b1), 1);
	infile.read((char *)(&b2), 1);

	if( (b1 == 1) && (b2 == 0) )
		status = true;

	infile.close();

	return status;
};

void IMTK::readDicomFromList(const string &fileName, Volume<unsigned short> &data, int &error)
{
	ifstream  infile;
	std::list<std::string> plist;
	std::string line;
	bool success;

	error = 0;

	infile.open( fileName.c_str() );
		
	if(!infile){
		cerr << "Error opening DICOM DIR (?) file " << fileName.c_str() << endl;
		error = IMTK::FILE_OPEN_ERROR;
		return;
	}


	// make sure it is a dicom _dataset file
        success = getline(infile, line);
	if( !success || line.empty() ){
		cerr << "File : " << fileName.c_str() << " does not appear to be a _dataset file " << endl;
		error = IMTK::UNSUPPORTED_FILE_FORMAT;
		return;
	}
	if( line.compare(std::string("#_dataset")) != 0 ){
		cerr << "File : " << fileName.c_str() << " does not appear to be a _dataset file " << endl;
		error = IMTK::UNSUPPORTED_FILE_FORMAT;
		return;
	}

	// make sure the next line is DICOM
	success = getline(infile, line);
	if( !success || line.empty() ){
		cerr << "File : " << fileName.c_str() << " does not appear to be a _dataset file " << endl;
		error = IMTK::UNSUPPORTED_FILE_FORMAT;
		return;
	}
	if( line.compare(std::string("DICOM")) != 0 ){
		cerr << "File : " << fileName.c_str() << " does not appear to be a _dataset file " << endl;
		error = IMTK::UNSUPPORTED_FILE_FORMAT;
		return;
	}

	// make sure the next line is FILENAMES :
	success = getline(infile, line);
	if( !success || line.empty() ){
		cerr << "File : " << fileName.c_str() << " does not appear to be a _dataset file " << endl;
		error = IMTK::UNSUPPORTED_FILE_FORMAT;
		return;
	}
	if( line.compare(std::string("FILENAMES :")) != 0 ){
		cerr << "File : " << fileName.c_str() << " does not appear to be a _dataset file " << endl;
		error = IMTK::UNSUPPORTED_FILE_FORMAT;
		return;
	}

	// get the list of file names
        while( getline(infile, line) ){
		plist.push_back(line);
	}

	infile.close(); // we are done with the list file

	// the dicom object and temp holders
	IMTK::Dicom dcmObj;
	float xo, yo, zo; //origin
	unsigned short xdim, ydim, zdim; // slice dimensions
	float dx, dy, dz; // in-plane voxel size
	int slice; // the slice number

	// list iterator
	std::list<std::string>::iterator lit;

	// now read the first DICOM file header
	lit = plist.begin();
	success = dcmObj.open(*lit);
	
	if( !success ){
		cerr << "Dicom file : " << (*lit).c_str() << " could not be opened " << endl;
		error = IMTK::FILE_OPEN_ERROR;
		return;
	}
	success = dcmObj.getOrigin(xo, yo, zo); //origin of this slice
	if( !success ){
		cerr << "Dicom file : " << (*lit).c_str() << " parse error " << endl;
		error = IMTK::FILE_PARSE_ERROR;
		dcmObj.close();
		return;
	}
	success = dcmObj.getDims(xdim, ydim); //slice dimensions
	if( !success ){
		cerr << "Dicom file : " << (*lit).c_str() << " parse error " << endl;
		error = IMTK::FILE_PARSE_ERROR;
		dcmObj.close();
		return;
	}
	success = dcmObj.getPxlSize(dx, dy); //in-plane voxel size
	if( !success ){
		cerr << "Dicom file : " << (*lit).c_str() << " parse error " << endl;
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
		cerr << "Dicom file : " << (*lit).c_str() << " parse error " << endl;
		error = IMTK::FILE_PARSE_ERROR;
		dcmObj.close();
		return;
	}
	dcmObj.close();

	// now read the origin of the next slice
	lit++;
	success = dcmObj.open(*lit);
	if( !success ){
		cerr << "Dicom file : " << (*lit).c_str() << " could not be opened " << endl;
		error = IMTK::FILE_OPEN_ERROR;
		return;
	}
	float tempxo, tempyo, tempzo;
	success = dcmObj.getOrigin(tempxo, tempyo, tempzo); //origin of this slice
	if( !success ){
		cerr << "Dicom file : " << (*lit).c_str() << " parse error " << endl;
		error = IMTK::FILE_PARSE_ERROR;
		dcmObj.close();
		return;
	}
	dcmObj.close();

	// assume there plist.size() slices and dz is difference is z origins
	zdim = plist.size();
	dz = tempzo - zo;

	// intialize the volume
	IMTK::Subscript<3> bounds;
	bounds[0] = xdim;
	bounds[1] = ydim;
	bounds[2] = zdim;
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
	lit = plist.begin();
	k = 1;
	while( lit != plist.end() ){

		//open the DICOM file
		success = dcmObj.open(*lit);
		if( !success ){
			cerr << "Dicom file : " << (*lit).c_str() << " could not be opened " << endl;
			error = IMTK::FILE_OPEN_ERROR;
			return;
		}
		// get the data
		success = dcmObj.getData(slicedata); 
		if( !success ){
			cerr << "Dicom file : " << (*lit).c_str() << " parse error " << endl;
			error = IMTK::FILE_PARSE_ERROR;
			dcmObj.close();
			delete [] slicedata;
			return;
		}
		dcmObj.close();

		// put in the volume
		for(i=0; i < bounds[0]; i++)
			for(j=0; j < bounds[1]; j++){
//				data.setVoxel(i, j, k, slicedata[i*ydim + j]);
				data.setVoxel(i, j, k, slicedata[j*xdim + i]);
			}

		clog << "Read DICOM slice " << k << endl;
		
		k += 1;
		lit++;
	}

	// free up slice data
	delete [] slicedata;
};


#include "AnalyzeHdr.hh"

void IMTK::readAnalyze(const string & dbName, Volume<unsigned short> &data, int &error)
{
	// clear error flag
	error = 0;

	// open database hdr file
	ifstream  infile;
	std::string fileName = dbName + ".hdr";
	infile.open( fileName.c_str() );
	if(!infile){
		cerr << "Error opening Analyze(?) file " << fileName.c_str() << endl;
		error = IMTK::FILE_OPEN_ERROR;
		return;
	}

	// read in the header
	struct dsr header;
	infile.read((char *)(&header), sizeof(struct dsr));

	// check if it is short or unknown type
	switch(header.dime.datatype){
	case 0: cerr << "WARNING: Analyze Data Type Unknown, Assuming 16 bit" << endl;
		break;

	case 4:	break; // this is what we want
	case 132 : break; //this is unsigned short

	default:cerr << "Analyze file data type not short or unknown: " << fileName.c_str() << endl;
		error = IMTK::UNSUPPORTED_FILE_FORMAT;
		infile.close(); // clean up
		return;
	}


	// check the number of dimensions is 3 or (4 and the number of time points is 1)
	switch(header.dime.dim[0]){

	case 3: break;

	case 4: if(header.dime.dim[4] == 1) break;

	default:cerr << "Analyze file data dimensions not single volume: " << fileName.c_str() << endl;
		error = IMTK::UNSUPPORTED_FILE_FORMAT;
		infile.close(); // clean up
		return;

	}

	// set the volume bounding box
	IMTK::Subscript<3> bounds;
	bounds[0] = header.dime.dim[1];
	bounds[1] = header.dime.dim[2];
	bounds[2] = header.dime.dim[3];
	data.setBounds(bounds);

	// set voxel size and origin
	data.setVoxelSpacing(header.dime.pixdim[1], header.dime.pixdim[2], header.dime.pixdim[3]);
	data.setOrigin(0, 0, 0);

	clog << "Volume size: " <<  bounds[0] << " x " << bounds[1] << " x " << bounds[2] << "\n";
	clog << "Voxel Size (mm): " << header.dime.pixdim[1] << "mm x " << header.dime.pixdim[2] << "mm x " << header.dime.pixdim[3] << "mm" << endl;

	// close the header file
	infile.close(); 

	// open database img file
	fileName = dbName + ".img";
	infile.open( fileName.c_str() );
	if(!infile){
		cerr << "Error opening Analyze(?) file " << fileName.c_str() << endl;
		error = IMTK::FILE_OPEN_ERROR;
		return;
	}

	unsigned short temp;
        unsigned int i, j, k;
	for(k=0; k < bounds[2]; k++)
		for(j=0; j < bounds[1]; j++)
			for(i=0; i < bounds[0]; i++){
				infile.read((char *)(&temp), 2);
				data.setVoxel(i, j, k, temp);
			}

	// clean up
	infile.close(); 
};



void IMTK::writeRVFinterp(const string &fileName, Volume<unsigned short> &data, int &error, int interp){

	// initialize
	error = 0;

	// open the file stream
        ofstream  outfile;
	outfile.open( fileName.c_str() );
	if(!outfile){
		cerr << "Error opening rvf(?) file " << fileName.c_str() << endl;
		error = IMTK::FILE_OPEN_ERROR;
		return;
	}

	// write out the magic number (b1 == 100) && (b2 == 118) && (b3 == 118) && (b4 == 114)
	unsigned char b1, b2, b3, b4;
	b1 = 100; b2 = 118; b3 = 118; b4 = 114;
	outfile.write((char *)&b1, 1);
	outfile.write((char *)&b2, 1);
	outfile.write((char *)&b3, 1);
	outfile.write((char *)&b4, 1);

	// write out the version
	float version = 1.6;
	outfile.write((char *)&version, 4);

	// write a serial number
	long serial = 0;
	outfile.write((char *)&serial, 4);

	// write the volume dimensions
	long xdim, ydim, zdim, bits;
	IMTK::Subscript<3> bounds;
	bounds = data.getBounds();
	xdim = bounds[0];
	ydim = bounds[1];
	zdim = (bounds[2]-1)*interp + 1;
	bits = 16;
	outfile.write((char *)&xdim, 4);
	outfile.write((char *)&ydim, 4);
	outfile.write((char *)&zdim, 4);
	outfile.write((char *)&bits, 4);

	// write flip display indications (I ignore these)
	long flip = 0;
	outfile.write((char *)&flip, 4);
	outfile.write((char *)&flip, 4);
	outfile.write((char *)&flip, 4);
	outfile.write((char *)&flip, 4);
	outfile.write((char *)&flip, 4);
	outfile.write((char *)&flip, 4);

	// write out the origin and voxel dimensions
	float xo, yo, zo;
	float dx, dy, dz;
	data.getOrigin(xo, yo, zo);
	data.getVoxelSpacing(dx, dy, dz);
	dz = dz/(float)interp;
        outfile.write((char *)(&xo), sizeof(float));
        outfile.write((char *)(&dx), sizeof(float));
        outfile.write((char *)(&yo), sizeof(float));
        outfile.write((char *)(&dy), sizeof(float));
        outfile.write((char *)(&zo), sizeof(float));
        outfile.write((char *)(&dz), sizeof(float));

	clog << "Volume size: " <<  bounds[0] << " x " << bounds[1] << " x " << bounds[2]*interp;
	clog << "       (mm): " << bounds[0]*dx << "mm x " << bounds[1]*dy << "mm x " << bounds[2]*dz*interp << "mm";
	clog << "     origin: " << xo << ", " << yo << ", " << zo << endl;

	// write out patient information (research ignores all this)
	char patient[448];
	memset(patient, 0, 448);
	outfile.write((char *)(&patient), 448);

	// write out two zeros 'cause we don't add dicom headers to the end
	outfile.write((char *)(&patient), 8); // reuse block of zeros from patient

	// now, write out the data
	unsigned short ustemp;
	int slice = 0;
	float a, b;
	int k;   
	for(k = 0; k < bounds[2]-1; k++)
	  {
	  for (int l = 0; l < interp; l++)
	    {
	    cout <<"Writing slice "<<slice ++<<endl;
	    for ( int j =0; j < bounds[1]; j++)
	      for( int i = 0; i < bounds[0]; i++)
	      {
	        a = (float)data.getVoxel(i,j,k);
	        b = (float)data.getVoxel(i,j,k+1);
		ustemp = (unsigned short) (((float)l/(float)interp)*b + (1-(float)l/(float)interp) * a); 
		outfile.write((char *) &ustemp, 2);
		if( outfile.bad() ){
			error = FILE_WRITE_ERROR;
			break;
		   }
               }
	    }
	  }  
	  //k == bound2-1
	for ( int j = 0; j < bounds[1]; j++)
	  for( int i = 0; i < bounds[0]; i++)
	      {
	        ustemp = data.getVoxel(i,j,k);
	        outfile.write((char *) &ustemp, 2);
	        if( outfile.bad() ){
		  error = FILE_WRITE_ERROR;
		  break;
	         }
	     }

	// close up
	outfile.close();
}

