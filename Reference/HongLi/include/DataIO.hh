/********************************************************
File: DataIO.hh
Abstract: Functions to read/write Data objects to 
          files, pipes, etc. 
Created: 01/18/2001
Author: Chris L. Wyatt

Last Revision ($Revision: 1.2 $) 
by $Author: hongli $ 
on $Date: 2003/10/02 19:45:39 $
*********************************************************/

//prevent recursive includes
#ifndef _DataIO_hh
#define _DataIO_hh 1

#include "Volume.hh"

#include <string>

namespace IMTK{

	// some common error conditions returned
	static const int FILE_OPEN_ERROR = 1;
	static const int FILE_PARSE_ERROR = 2;
	static const int UNSUPPORTED_FILE_FORMAT = 3;
	static const int FILE_WRITE_ERROR = 4;
 
	// determine if the file is compressed using gzip 
	bool isCompressed(const string &fileName, int &error);

	bool isRVF(const string &fileName, int &error);

	// read volume file in rvf format
	void readRVF(const string &fileName, Volume<unsigned short> &data, bool mask, int &error);
	void readRVFheader(const string &fileName, float &xo, float &yo, float &zo, 
float & dx, float &dy, float &dz , int & xdim, int &ydim, int &zdim, int &error);

#ifdef HAVE_ZLIB
	// read volume file in compressed rvf format
	void readRVFgz(const string &fileName, Volume<unsigned short> &data, bool mask, int &error);
#endif

	void writeRVF(const string &fileName, Volume<unsigned short> &data, int &error);
        void writeRVFinterp(const string &fileName, Volume<unsigned short> &data, int &error, int interp);

	bool isMatlab(const string &fileName, int &error);

	// read a list of DICOM files
	void readDicomFromList(const string &fileName, Volume<unsigned short> &data, int &error);

	// read an Analyze image database file (only a subset)
	void readAnalyze(const string &dbName, Volume<unsigned short> &data, int &error);
	// write an Analyze image database file (only a subset)
//	void writeAnalyze(const string &dbName, Volume<unsigned short> &data, int &error);

// #ifdef HAVE_HDF4
// 	// read volume from HDF version 4.x file
// 	void readHDF(const string &fileName
// #endif
};

#endif
