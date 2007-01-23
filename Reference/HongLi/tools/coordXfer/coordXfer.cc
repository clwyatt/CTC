// File: coordXfer.cc
// Abstract: convert coordinates between world and volume
//
// Created: 02/26/02 by: Chris L. Wyatt
//
//                

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stdio.h>

#include "DataIO.hh"

using namespace IMTK;

/*
* main routine
*/
int main(int argv, char **argc)
{
	char infilename[256];
	float x, y, z;  // coordinates
	float xo, yo, zo;
	float dx, dy, dz;
	int xdim, ydim,zdim;
	int error;
	int flag;
	//parse command line
	if(argv == 6){
		strncpy(infilename, *(argc+1), 256);
		
		sscanf(argc[2], "%f", &x);
		sscanf(argc[3], "%f", &y);
		sscanf(argc[4], "%f", &z);	 
		flag = atoi(argc[5]);
	}
	else{
		cerr << "coordXfer (world<->volume coordinates transfer tool) ERROR: " << endl;
		cerr << "Usage: coordXfer rvf x, y, z, flag" << endl;
		cerr << "	rvf : the rvf file name;"<< endl;
		cerr << "	x, y ,z : coordinate to be transfered;"<< endl;
		cerr << "	flag : 0, world -> volume, 1 reverse. "<< endl;
		exit(EXIT_FAILURE);
	}
	
	readRVFheader(infilename, xo,yo,zo,dx,dy,dz,xdim,ydim,zdim, error);

	if(error) exit(EXIT_FAILURE);
	if(flag == 0) //world -> volume
	{
		x = (x-xo)/dx;
		y = (y-yo)/dy;
		z = (z-zo)/dz;
		printf("%s, %s, %s ==> %d, %d, %d\n", argc[2], argc[3], argc[4], 
				(int) x, (int) y, (int)z);
	}
	else
	{
		x = x*dx + xo;
		y = y*dy + yo;
		z = z*dz + zo;
		printf("%s, %s, %s ==> %f, %f, %f\n", argc[2], argc[3], argc[4], 
				x, y, z);
	}
}				
	




