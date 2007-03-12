// File: bvolume.h
// Abstract: Header file for bvolume.cc
// Created: 7/10/96 by: Chris L. Wyatt
// Modified: 07/28/98 to use only binary class
//
// class defined; bframe3d
//
// prevent recursive and/or excessive includes
#ifndef _bvolume_h

#define _bvolume_h 1

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fstream>
#include <iostream>
#include <ctype.h>

// #define bool char
#define FALSE false 
#define TRUE true 

class bframe3d  //3d frame of binary for region images
{
  private:
           bool *frame_data;  //pointer to boolean 3D pixel array
           int horz_res;
           int vert_res;
           int z_res;
  public:
         //normal stuff
           bframe3d();             // default constructor
           bframe3d(const bframe3d & in);  // copy constructor
           ~bframe3d();            // destructor
           bframe3d(int horz_res, int vert_res, int z_res); //custom

          //operations
             void setpixel(int x, int y, int z, bool value);
             bool getpixel(int x, int y, int z);
             void querydim(int *x, int *y, int *z); //query for stack dims
             void setdim(int x, int y, int z); //set 3d frame dims
             bframe3d & operator=(const bframe3d & in);
             void clear(); //set all values to FALSE
             void surface(); // remove all inner points of region leaving surface
             float * vert(int *maxverts); //generate veticies
                                                   //array for render
             float * norm(int *maxverts); //generate normal at each point for  
                                         //render
             float * spline(int *np); //test

             int getvert(int i, int j, int k, unsigned int v);

             void write(char * filename);

             void read(char * filename, int xdim, int ydim, int zdim);

	     //int * label();
};

#endif
