/********************************************************
File: DataIO.hh
Abstract: Functions to read/write Data objects to 
          files, pipes, etc. 
Created: 01/18/2001
Author: Chris L. Wyatt

Last Revision ($Revision: 1.10 $) 
by $Author: clwyatt $ 
on $Date: 2002/07/31 20:19:53 $
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
	bool isCompressed(const std::string &fileName, int &error);

	bool isRVF(const std::string &fileName, int &error);

	// read volume file in rvf format
	void readRVF(const std::string &fileName, Volume<unsigned short> &data, bool mask, int &error);

#ifdef HAVE_ZLIB
	// read volume file in compressed rvf format
	void readRVFgz(const std::string &fileName, Volume<unsigned short> &data, bool mask, int &error);
#endif
	// write volume in RVF format
	void writeRVF(const std::string &fileName, Volume<unsigned short> &data, int &error);

	bool isMatlab(const std::string &fileName, int &error);

	// read a list of DICOM files
	void readDicomFromList(const std::string &fileName, Volume<unsigned short> &data, int &error);

	// read an Analyze image database file (only a subset)
	void readAnalyze(const std::string &dbName, Volume<unsigned short> &data, int &error);

	// read header (only) of an Analyze image database file (only a subset)
	void readAnalyzeHeader(const std::string &dbName, Subscript<3> &bounds, int &error);
		
	// write volume in analyze format
	void writeAnalyze(const std::string &dbName, Volume<unsigned short> &data, int &error);

// #ifdef HAVE_HDF4
// 	// read volume from HDF version 4.x file
// 	void readHDF(const std::string &fileName
// #endif

};

#endif
