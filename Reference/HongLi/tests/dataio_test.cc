/********************************************************
File: data_test.cc
Abstract: Test file for DataIO functions. 
Created: 04/15//2001
Author: Chris L. Wyatt

Last Revision ($Revision: 1.1.1.1 $) 
by $Author: hongli $ 
on $Date: 2002/12/06 21:49:27 $
*********************************************************/

#include "DataIO.hh"
#include "Volume.hh"

#include <string>

using namespace std;


int main(int argc, char **argv) {

	string fileName1 = string("file.rvf");
	string fileName2 = string("file.rvf.gz");
	string fileName3 = string("file.mat");
	IMTK::Volume<unsigned short> vol;
	int error;

	if(IMTK::isCompressed(fileName1, error)) 
		cout << "File " << fileName1.c_str() << " is compressed" << endl;
	else
		cout << "File " << fileName1.c_str() << " is not compressed" << endl;

	if(IMTK::isCompressed(fileName2, error)) 
		cout << "File " << fileName2.c_str() << " is compressed" << endl;
	else
		cout << "File " << fileName2.c_str() << " is not compressed" << endl;

	
	if(IMTK::isRVF(fileName1, error)) 
		cout << "File " << fileName1.c_str() << " is in RVF format" << endl;
	else
		cout << "File " << fileName1.c_str() << " is not in RVF format" << endl;

	IMTK::readRVF(fileName1, vol, false, error);

	if(IMTK::isRVF(fileName2, error)) 
		cout << "File " << fileName2.c_str() << " is in RVF format" << endl;
	else
		cout << "File " << fileName2.c_str() << " is not in RVF format" << endl;

#ifdef HAVE_ZLIB
	IMTK::readRVFgz(fileName2, vol, false, error);
#endif

	if(IMTK::isMatlab(fileName3, error)) 
		cout << "File " << fileName3.c_str() << " is in MATLAB format" << endl;
	else
		cout << "File " << fileName3.c_str() << " is not in MATLAB format" << endl;
		
	// test dicom reader
	string fileName4 = string("_dataset");
	IMTK::readDicomFromList(fileName4, vol, error);
	if(error){
		cout << "Error " << error << " reading DICOM list" << endl;
	}
	else{
		vol.dump("dataset.dump");
	}
}
