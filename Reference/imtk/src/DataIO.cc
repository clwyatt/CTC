/********************************************************
File: DataIO.cc
Abstract: Functions to read/write Data objects to 
          files, pipes, etc

See header (DataIO.hh) for documentation

Last Revision ($Revision: 1.15 $) 
by $Author: clwyatt $ 
on $Date: 2002/07/31 20:19:53 $
*********************************************************/

#include "DataIO.hh"
#include "Dicom.hh"

#ifdef HAVE_ZLIB
#include <zlib.h>
#endif

#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <algorithm>

using namespace std;

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

        bounds[0] = (int)sx;
        bounds[1] = (int)sy;
        bounds[2] = (int)sz;
	data.setBounds(bounds);

	infile.seekg(52);
        infile.read((char *)(&xo), sizeof(float));
        infile.read((char *)(&dx), sizeof(float));
        infile.read((char *)(&yo), sizeof(float));
        infile.read((char *)(&dy), sizeof(float));
        infile.read((char *)(&zo), sizeof(float));
        infile.read((char *)(&dz), sizeof(float));
	
	if( (fabs(dx) > 0) && (fabs(dy) > 0) && (fabs(dz) > 0) ){
		data.setVoxelSpacing(dx, dy, dz);
	}
	data.setOrigin(xo, yo, zo);

	clog << "Volume size: " <<  bounds[0] << " x " << bounds[1] << " x " << bounds[2];
	clog << "       (mm): " << bounds[0]*dx << "mm x " << bounds[1]*dy << "mm x " << bounds[2]*dz << "mm";
	clog << "     origon: " << xo << ", " << yo << ", " << zo << endl;

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
	clog << "     origon: " << xo << ", " << yo << ", " << zo << endl;

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
	float xo, yo, zo; //first slice origin
	float xn, yn, zn; //next slice origin
	unsigned short xdim, ydim, zdim; // slice dimensions
	float dx, dy, dz; // in-plane voxel size
	long slice;

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
	success = dcmObj.getDims(xdim, ydim); //slice dimensions
	if( !success ){
		cerr << "Dicom file : " << (*lit).c_str() << " parse error " << endl;
		error = IMTK::FILE_PARSE_ERROR;
		return;
	}
	success = dcmObj.getPxlSize(dx, dy); //in-plane voxel size
	if( !success ){
		cerr << "Dicom file : " << (*lit).c_str() << " parse error " << endl;
		error = IMTK::FILE_PARSE_ERROR;
		return;
	}
	dcmObj.close();

	// loop through checking for consistency and storing the
	// instance numbers
	std::vector<long> instance_list( plist.size() );
	int instance_index = 0;
	lit = plist.begin();
	while( lit != plist.end() ){
		unsigned short test_row_dim, test_col_dim;
		float testdr, testdc;

		success = dcmObj.open(*lit);
		if( !success ){
			cerr << "Dicom file : " << (*lit).c_str() << " could not be opened " << endl;
			error = IMTK::FILE_OPEN_ERROR;
			return;
		}
		success = dcmObj.getDims(test_row_dim, test_col_dim);
		if( !success ){
			cerr << "Dicom file : " << (*lit).c_str() << " parse error " << endl;
			error = IMTK::FILE_PARSE_ERROR;
			return;
		}
		success = dcmObj.getPxlSize(testdr, testdc);
		if( !success ){
			cerr << "Dicom file : " << (*lit).c_str() << " parse error " << endl;
			error = IMTK::FILE_PARSE_ERROR;
		return;
		}
		success = dcmObj.getInstanceNumber(slice);
		if( !success ){
			cerr << "Dicom file : " << (*lit).c_str() << " parse error " << endl;
			error = IMTK::FILE_PARSE_ERROR;
			return;
		}
		instance_list[instance_index] = slice;
		instance_index += 1;
		dcmObj.close();

		if( (xdim != test_row_dim) || (ydim != test_col_dim) || 
		    (dx != testdr) || (dy != testdc) ){
			cerr << "Dicom file : " << (*lit).c_str() << " parse error: Inconsistent geometry " << endl;
			error = IMTK::FILE_PARSE_ERROR;
			return;
		}

		lit++;
	}

	// sort instance numbers
	sort(instance_list.begin(), instance_list.end() );

	// assume there plist.size() slices 
	zdim = plist.size();

	// intialize the volume
	IMTK::Subscript<3> bounds;
	bounds[0] = xdim;
	bounds[1] = ydim;
	bounds[2] = zdim;
	data.setBounds(bounds);


	// store each slice in the volume
	unsigned int i, j, k;
	unsigned short * slicedata;

	slicedata = new unsigned short[xdim*ydim];
	lit = plist.begin();

	while( lit != plist.end() ){

		//open the DICOM file
		success = dcmObj.open(*lit);
		if( !success ){
			cerr << "Dicom file : " << (*lit).c_str() << " could not be opened " << endl;
			error = IMTK::FILE_OPEN_ERROR;
			return;
		}
		// get the data
		success = dcmObj.getData(slicedata, xdim*ydim); 
		if( !success ){
			cerr << "Dicom file : " << (*lit).c_str() << " parse error " << endl;
			error = IMTK::FILE_PARSE_ERROR;
			delete [] slicedata;
			return;
		}
		// get the slice number, k, (sorted instance number -1)
		success = dcmObj.getInstanceNumber(slice);		
		if( !success ){
			cerr << "Dicom file : " << (*lit).c_str() << " parse error " << endl;
			error = IMTK::FILE_PARSE_ERROR;
			delete [] slicedata;
			return;
		}
		for(k = 0; k < instance_list.size(); k++){
			if(slice == instance_list[k]) break;
		}
		// if first slice, read origin
		if(k == 0){
			success = dcmObj.getOrigin(xo, yo, zo); 
			if( !success ){
				cerr << "Dicom file : " << (*lit).c_str() << " parse error " << endl;
				error = IMTK::FILE_PARSE_ERROR;
				return;
			}
		}
		// if second slice, read origin
		if(k == 1){
			success = dcmObj.getOrigin(xn, yn, zn); 
			if( !success ){
				cerr << "Dicom file : " << (*lit).c_str() << " parse error " << endl;
				error = IMTK::FILE_PARSE_ERROR;
				return;
			}
		}

		dcmObj.close();

		// put in the volume in radiologic format
		for(i=0; i < bounds[0]; i++)
			for(j=0; j < bounds[1]; j++){
				data.setVoxel(i, j, k, slicedata[i*ydim + j]);
			}

		//clog << "Read DICOM slice " << k << endl;
		
		lit++;
	}

	// compute dx, dy, dz depending upon orientation (axial, coronal, sagital)
	dz = 0;
	if( fabs(xn-xo) > 0.00001){ // sagital
		dz = xn-xo;
	}
	if( fabs(yn-yo) > 0.00001){ // coronal
		dz = yn-yo;
	}
	if( fabs(zn-zo) > 0.00001){ // axial
		dz = zn-zo;
	}
	data.setVoxelSpacing(dx, dy, dz);
	data.setOrigin(xo, yo, zo);
	
	clog << "    Volume size: " <<  bounds[0] << " x " << bounds[1] << " x " << bounds[2] << endl;
	clog << "           (mm): " << bounds[0]*dx << "mm x " << bounds[1]*dy << "mm x " << bounds[2]*dz << "mm" << endl;
	clog << "         origon: " << xo << ", " << yo << ", " << zo << endl;
	clog << "Voxel size (mm): " << dx << ", " << dy << ", " << dz << endl;


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

	// print some info
	//clog << "sizeof_hdr = " << header.hk.sizeof_hdr << endl;
	//clog << "data_type = " << &header.hk.data_type[0] << endl;
	//clog << "db_name = " << &header.hk.db_name[0] << endl;
	//clog << "extents = " << header.hk.extents << endl;
	//clog << "session_error = " << header.hk.session_error << endl;
	//clog << "regular = " << header.hk.regular << endl;
	//clog << "hkey_un0 = " << header.hk.hkey_un0 << endl;

	// check if it is short or unknown type
	switch(header.dime.datatype){
	case 0: cerr << "WARNING: Analyze Data Type Unknown, Assuming 16 bit" << endl;
		break;

	case 4:	break; // this is what we want

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

void IMTK::readAnalyzeHeader(const string & dbName, Subscript<3> &bounds, int &error)
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

	// print some info
	//clog << "sizeof_hdr = " << header.hk.sizeof_hdr << endl;
	//clog << "data_type = " << &header.hk.data_type[0] << endl;
	//clog << "db_name = " << &header.hk.db_name[0] << endl;
	//clog << "extents = " << header.hk.extents << endl;
	//clog << "session_error = " << header.hk.session_error << endl;
	//clog << "regular = " << header.hk.regular << endl;
	//clog << "hkey_un0 = " << header.hk.hkey_un0 << endl;

	// check if it is short or unknown type
	switch(header.dime.datatype){
	case 0: cerr << "WARNING: Analyze Data Type Unknown, Assuming 16 bit" << endl;
		break;

	case 4:	break; // this is what we want

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
	bounds[0] = header.dime.dim[1];
	bounds[1] = header.dime.dim[2];
	bounds[2] = header.dime.dim[3];

	// close the header file
	infile.close(); 
};

void IMTK::writeAnalyze(const string &dbName, Volume<unsigned short> &data, int &error)
{
	// clear error flag
	error = 0;

	// analyze header
	struct dsr header;
	memset(&header, 0, sizeof(header));

	// set data type name to dsr
	strncpy(&header.hk.data_type[0], "dsr", 4);

	// set as regular
	header.hk.regular = 'r';

	// set datatype to short
	header.dime.datatype = 4;

	// set dimensions
	Subscript<3> bounds = data.getBounds();
	header.dime.dim[0] = 3;
	header.dime.dim[1] = bounds[0];
	header.dime.dim[2] = bounds[1];
	header.dime.dim[3] = bounds[2];

	// set voxel size
	float dx, dy, dz;
	data.getVoxelSpacing(dx, dy, dz);
	header.dime.pixdim[0] = 3;
	header.dime.pixdim[1] = fabs(dx);
	header.dime.pixdim[2] = fabs(dy);
	header.dime.pixdim[3] = fabs(dz);

	// set database name
	//if(dbName.length() > 17){
	//	cerr << "Analyze databse name must be less than 17 characters: " << dbName.c_str() << endl;
	//	error = IMTK::UNSUPPORTED_FILE_FORMAT;
	//	return;
	//}
	//strncpy(&header.hk.db_name[0], dbName.c_str(), dbName.length()+1);

	// set header size
	header.hk.sizeof_hdr = sizeof(header);

	// open the header file
	std::string hdrstr = dbName + ".hdr";
	ofstream  hdrfile;
	hdrfile.open( hdrstr.c_str() );
	if(!hdrfile){
		cerr << "Error opening header file " << hdrstr.c_str() << endl;
		error = IMTK::FILE_OPEN_ERROR;
		return;
	}

	// write the header
	hdrfile.write((char *) &header, header.hk.sizeof_hdr);

	// close header file
	hdrfile.close();

	// now open and write data
	std::string imgstr = dbName + ".img";
	ofstream  imgfile;
	imgfile.open( imgstr.c_str() );
	if(!imgfile){
		cerr << "Error opening image file " << imgstr.c_str() << endl;
		error = IMTK::FILE_OPEN_ERROR;
		return;
	}
	unsigned short ustemp;
	for(int i = 0; i < data.size(); i++){
		ustemp = data[i];
		imgfile.write((char *) &ustemp, 2);
		if( imgfile.bad() ){
			error = IMTK::FILE_WRITE_ERROR;
			break;
		}
	}
	
	//close the image file
	imgfile.close();

};

