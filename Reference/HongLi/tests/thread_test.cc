/********************************************************
File: thread_test.cc
Abstract: Test file for abstract Thread class. 
Created: 11/14//2000
Author: Chris L. Wyatt

Last Revision ($Revision: 1.1.1.1 $) 
by $Author: hongli $ 
on $Date: 2002/12/06 21:49:27 $
*********************************************************/
#include <iostream>
#include <fstream>
#include <string.h>

#include "Thread.hh"

using namespace std;

// derive a class from thread to read an rvf file
class ImtkRVFreader: public IMTK::Thread
{
	class RVFheader{
	public:
		char filename[128];
		int xdim, ydim, zdim, bits;
	};

private:
	RVFheader header;

	unsigned short * threedp;

	IMTK::Semaphore sem;
	double percentComplete;

	/**
	 * thread function to read file
	 * 
	 */
	void * thread_func(){
		
		ifstream  infile;
		//unsigned short * threedp;
		long sx, sy, sz, bits;
		int i, size, xdim, ydim, zdim;
		
		fprintf(stderr, "File name is %s.\n", header.filename);
		
		infile.open( header.filename );

		infile.seekg(12);
		
		infile.read((char *)(&sx), 4);
		infile.read((char *)(&sy), 4);
		infile.read((char *)(&sz), 4);
		infile.read((char *)(&bits), 4);

		xdim = (int)sx;
		ydim = (int)sy;
		zdim = (int)sz;
		
		fprintf(stderr, "Volume size: %i x %i x %i with %li bits\n", xdim, ydim, zdim, bits);

		size = (xdim)*(ydim)*(zdim);
		
		infile.seekg(532);
		
		//if(bits == 16)
		threedp = new unsigned short[size];
		//else
		//threedp = NULL;
		
		if(threedp != NULL){
			for(i=0; i < zdim; i++){
				fprintf(stderr, "Reading slice %i\n", i);
				infile.read((char *)(threedp+i*xdim*ydim), 2*xdim*ydim);
				this->setCompletion(double(i)/double(zdim));
			}
		}
		
		infile.close();
		
	
		setStatus(FINISHED);
		
		pthread_exit(threedp);
		
		return NULL;
	}


public:
	/// Default Constructor
	ImtkRVFreader(){}

	~ImtkRVFreader(){}

	void setCompletion(double thePercent){

		sem.down();
		percentComplete = thePercent;
		sem.up();
	}
	
	double getCompletion(){
		
		double temp;
		
		sem.down();
		temp = percentComplete;
		sem.up();
		
		return temp;
	}

	/**
	 * Set the file name
	 * 
	 */
	void setFileName(char * name){

		strcpy(header.filename, name);
	}

	unsigned short * getData(){
		return threedp;
	}
	
};


int main(int argc, char **argv) {

	ImtkRVFreader filereader;
	char * name;
	unsigned short * data;

	name = new char[128];

	fprintf(stderr, "Parsing command line.\n");
	filereader.setFileName(*(argv+1));

	fprintf(stderr, "File name is %s.\n", name);

	// start the thread
	fprintf(stderr, "Starting thread.\n");
	filereader.run();
	fprintf(stderr, "Waiting .....");
	
	// wait for it to finish
// 	while(filereader.getStatus() != ImtkRVFreader::FINISHED){

// 		fprintf(stderr, "Outside thread: %f completed\n", filereader.getCompletion());
// 	}

	filereader.join();
	
	data =  filereader.getData();
}
