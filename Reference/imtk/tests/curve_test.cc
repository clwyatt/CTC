/********************************************************
File: curve_test.cc
Abstract: Test file for Curve. Computes curve of embedded circle
          and write to Geomview file. 
Created: 11/17/2000
Author: Chris L. Wyatt

Last Revision ($Revision: 1.3 $) 
by $Author: clwyatt $ 
on $Date: 2001/03/31 00:42:53 $
*********************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <iostream>
#include <fstream>

#include "Curve.hh"

#define EPSILON 0.0000000001

using namespace std;

/* Function: interpolate2
 * Abstract: interpolate between two points
 *
*/
void interpolate2(float v1a[], float v1b[], float v2a[], float v2b[], float interp1[], float interp2[])
{

	interp1[0] = (v1a[0] + v1b[0])/2;
	interp1[1] = (v1a[1] + v1b[1])/2;

	interp2[0] = (v2a[0] + v2b[0])/2;
	interp2[1] = (v2a[1] + v2b[1])/2;
};

/* Function: interpolate4
 * Abstract: interpolate between four points
 *
*/
void interpolate4(float v1a[], float v1b[], float v2a[], float v2b[], 
		  float v3a[], float v3b[], float v4a[], float v4b[],
		  float interp1[], float interp2[], float interp3[], float interp4[])
{

	interp1[0] = (v1a[0] + v1b[0])/2;
	interp1[1] = (v1a[1] + v1b[1])/2;

	interp2[0] = (v2a[0] + v2b[0])/2;
	interp2[1] = (v2a[1] + v2b[1])/2;

	interp3[0] = (v3a[0] + v3b[0])/2;
	interp3[1] = (v3a[1] + v3b[1])/2;

	interp4[0] = (v4a[0] + v4b[0])/2;
	interp4[1] = (v4a[1] + v4b[1])/2;
};

/* Function: interpolate3
 * Abstract: interpolate between three points
 *
*/
void interpolate3(float v1a[], float v1b[], 
		  float v2a[], float v2b[], 
		  float v3a[], float v3b[],
		  float interp1[], float interp2[], float interp3[])
{

	interp1[0] = (v1a[0] + v1b[0])/2;
	interp1[1] = (v1a[1] + v1b[1])/2;

	interp2[0] = (v2a[0] + v2b[0])/2;
	interp2[1] = (v2a[1] + v2b[1])/2;

	interp3[0] = (v3a[0] + v3b[0])/2;
	interp3[1] = (v3a[1] + v3b[1])/2;
};
 
/* Function: extractLevelCurve
 * Abstract: determine curve using contouring algorithm
 *
 * Note: the grid vertices are in the folowing order

         v1-------v2---->x
         |        |
	 |        |
         |        |
         v3-------v4
         |
	 |
	 v
	 y
*/
IMTK::Curve * extractCurve(float * data, int xdim, int ydim, float I0)
{
	int i, j;
	int x, y;
	unsigned int test;
	float v1[2], v2[2], v3[2], v4[2];
	float I1, I2, I3, I4;
	float interp1[2], interp2[2], interp3[2], interp4[2];
	IMTK::Vertex2D vtx1, vtx2;
	IMTK::Line l;
	IMTK::Curve * c;

	c = new IMTK::Curve(true);
	c->setBounds(xdim, ydim);

	for(i = 0; i < xdim-1; i++)
		for(j=0; j < ydim-1; j++){

			// vertex v1
			v1[0] = i; v1[1] = j;
			I1 = *(data + i*ydim + j);
			if(I1 == I0) I1 = I1 + EPSILON;

			// vertex v2
			x = i+1;
			y = j;
			v2[0] = x; v2[1] = y;
			I2 = *(data + x*ydim + y);
			if(I2 == I0) I2 = I2 + EPSILON;

			// vertex v3
			x = i;
			y = j+1;
			v3[0] = x; v3[1] = y;
			I3 = *(data + x*ydim + y);
			if(I3 == I0) I3 = I3 + EPSILON;

			// vertex v4
			x = i+1;
			y = j+1;
			v4[0] = x; v4[1] = y;
			I4 = *(data + x*ydim + y);
			if(I4 == I0) I4 = I4 + EPSILON;

			test = 0;
			if(I1 < I0) test+=8;
			if(I2 < I0) test+=4;
			if(I3 < I0) test+=2;
			if(I4 < I0) test+=1;

			if( (test > 0) && (test < 15) ){

				switch(test){

				case 1:
					interpolate2(v4, v2, v4, v3, interp1, interp2);
					vtx1.set(interp1[0], interp1[1]);
					vtx2.set(interp2[0], interp2[1]);
					l.set(&vtx1, &vtx2);
					c->addLine(l);
					break;
				case 2:
					interpolate2(v3, v1, v3, v4, interp1, interp2);
					vtx1.set(interp1[0], interp1[1]);
					vtx2.set(interp2[0], interp2[1]);
					l.set(&vtx1, &vtx2);
					c->addLine(l);
					break;
				case 3:
					interpolate2(v3, v1, v4, v2, interp1, interp2);
					vtx1.set(interp1[0], interp1[1]);
					vtx2.set(interp2[0], interp2[1]);
					l.set(&vtx1, &vtx2);
					c->addLine(l);
					break;
				case 4:
					interpolate2(v2, v1, v2, v4, interp1, interp2);
					vtx1.set(interp1[0], interp1[1]);
					vtx2.set(interp2[0], interp2[1]);
					l.set(&vtx1, &vtx2);
					c->addLine(l);
					break;
				case 5:
					interpolate2(v2, v1, v4, v3, interp1, interp2);
					vtx1.set(interp1[0], interp1[1]);
					vtx2.set(interp2[0], interp2[1]);
					l.set(&vtx1, &vtx2);
					c->addLine(l);
					break;
				case 6:
					interpolate4(v3, v1, v2, v1, v3, v4, v2, v4, 
						     interp1, interp2, interp3, interp4);
					vtx1.set(interp1[0], interp1[1]);
					vtx2.set(interp2[0], interp2[1]);
					l.set(&vtx1, &vtx2);
					c->addLine(l);
					vtx1.set(interp3[0], interp3[1]);
					vtx2.set(interp4[0], interp4[1]);
					l.set(&vtx1, &vtx2);
					c->addLine(l);
					break;
				case 7:
					interpolate3(v3, v1, v4, v1, v2, v1, 
						     interp1, interp2, interp3);
					vtx1.set(interp1[0], interp1[1]);
					vtx2.set(interp2[0], interp2[1]);
					l.set(&vtx1, &vtx2);
					c->addLine(l);
					vtx1.set(interp2[0], interp2[1]);
					vtx2.set(interp3[0], interp3[1]);
					l.set(&vtx1, &vtx2);
					c->addLine(l);
					break;
				case 8:
					interpolate2(v1, v2, v1, v3, interp1, interp2);
					vtx1.set(interp1[0], interp1[1]);
					vtx2.set(interp2[0], interp2[1]);
					l.set(&vtx1, &vtx2);
					c->addLine(l);
					break;
				case 9:
					interpolate4(v1, v3, v4, v3, v1, v2, v4, v2, 
						     interp1, interp2, interp3, interp4);
					vtx1.set(interp1[0], interp1[1]);
					vtx2.set(interp2[0], interp2[1]);
					l.set(&vtx1, &vtx2);
					c->addLine(l);
					vtx1.set(interp3[0], interp3[1]);
					vtx2.set(interp4[0], interp4[1]);
					l.set(&vtx1, &vtx2);
					c->addLine(l);
					break;
				case 10:
					interpolate2(v1, v2, v3, v4, interp1, interp2);
					vtx1.set(interp1[0], interp1[1]);
					vtx2.set(interp2[0], interp2[1]);
					l.set(&vtx1, &vtx2);
					c->addLine(l);
					break;
				case 11:
					interpolate3(v1, v2, v3, v2, v4, v2, 
						     interp1, interp2, interp3);
					vtx1.set(interp1[0], interp1[1]);
					vtx2.set(interp2[0], interp2[1]);
					l.set(&vtx1, &vtx2);
					c->addLine(l);
					vtx1.set(interp2[0], interp2[1]);
					vtx2.set(interp3[0], interp3[1]);
					l.set(&vtx1, &vtx2);
					c->addLine(l);
					break;
				case 12:
					interpolate2(v1, v3, v2, v4, interp1, interp2);
					vtx1.set(interp1[0], interp1[1]);
					vtx2.set(interp2[0], interp2[1]);
					l.set(&vtx1, &vtx2);
					c->addLine(l);
					break;
				case 13:
					interpolate3(v1, v3, v2, v3, v4, v3, 
						     interp1, interp2, interp3);
					vtx1.set(interp1[0], interp1[1]);
					vtx2.set(interp2[0], interp2[1]);
					l.set(&vtx1, &vtx2);
					c->addLine(l);
					vtx1.set(interp2[0], interp2[1]);
					vtx2.set(interp3[0], interp3[1]);
					l.set(&vtx1, &vtx2);
					c->addLine(l);
					break;
				case 14:
					interpolate3(v3, v4, v1, v4, v2, v4, 
						     interp1, interp2, interp3);
					vtx1.set(interp1[0], interp1[1]);
					vtx2.set(interp2[0], interp2[1]);
					l.set(&vtx1, &vtx2);
					c->addLine(l);
					vtx1.set(interp2[0], interp2[1]);
					vtx2.set(interp3[0], interp3[1]);
					l.set(&vtx1, &vtx2);
					c->addLine(l);
					break;
				default:
					fprintf(stderr, "Error (FATAL): case test in extractLevelCurve failed.\n");
					exit(1);
				}

			}
		}

	return c;
}

void ooglwrite(IMTK::Curve *mesh, char * name)
{
        ofstream  outfile;
	char entry[100];
	int i;
	IMTK::Vertex2D *v1, *v2;

	outfile.open( name );

	outfile << "appearance { shading flat }\n";
	outfile << "OFF\n";

	cerr << mesh->getNverticies() << " verticies, " << mesh->getNlines() << " lines." << endl;

	sprintf(entry, "%i %i 0\n\n", mesh->getNverticies(), mesh->getNlines());
	outfile << entry;

	IMTK::Curve::vertexIterator mvi = mesh->vertexBegin();
	i = 0;
	while( mvi !=  mesh->vertexEnd() ){
		sprintf(entry, "%f %f 0.0\n", (*mvi)->getX(), (*mvi)->getY());
		outfile << entry;
		// label vertex 
		(*mvi)->label(i);
		i++;
		mvi++;
	}

	outfile << "\n";

	IMTK::Curve::lineIterator mli = mesh->lineBegin();
	while( mli !=  mesh->lineEnd() ){
		v1 = (*mli).getV1();
		v2 = (*mli).getV2();
		sprintf(entry, "2\t%i %i %i\n", v1->label(), v2->label(), v1->label());
		outfile << entry;
		mli++;
	}

	outfile.close();

};

int main(int argv, char **argc)
{
   char outfilename[100];
   int xdim, ydim, radius;
   int centerx, centery;
   int i, j;
   float dx, dy, d2, r2, *data;
   IMTK::Curve * m;

   if(argv < 5)
    {
      fprintf(stderr, "Usage: curve_test xdim ydim radius outfile.oogl\n");
      exit(1);
    }

   //parse command line, no checking!
   xdim = atoi(*(argc+1));
   ydim = atoi(*(argc+2));
   radius = atoi(*(argc+3));
   strcpy(outfilename, *(argc+4));

   centerx = (int)((((float)(xdim))/2) + 0.5);
   centery = (int)((((float)(ydim))/2) + 0.5);

   data = new float[xdim*ydim];
   r2 = radius*radius;

   cout << "Creating test volume" << xdim << "x" << ydim << endl;

   for(i=0; i < xdim; i++)
	   for(j=0; j < ydim; j++){
		   dx = (float)(centerx - i);
		   dy = (float)(centery - j);
		   
		   d2 = dx*dx + dy*dy;
		   
		   *(data + i*ydim + j) = sqrt(d2);
	   }

   cout << "Computing Curve" << endl;
   //Compute surface
   m = extractCurve(data, xdim, ydim, radius);

   cout << "Writing File" << endl;
   //write surface
   ooglwrite(m, outfilename);

   delete m;
   delete data;
}

