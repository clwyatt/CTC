/********************************************************
File: mesh_test.cc
Abstract: Test file for Mesh. Computes surface of sphere
          and write to Geomview file. 
Created: 11/17/2000
Author: Chris L. Wyatt

Last Revision ($Revision: 1.1.1.1 $) 
by $Author: hongli $ 
on $Date: 2002/12/06 21:49:27 $
*********************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <iostream>
#include <fstream>

#include "Mesh.hh"

using namespace std;

struct XYZ{
	double x,y,z;
};

struct TRIANGLE{
	struct XYZ p[3];
};

struct GRIDCELL{
	struct XYZ p[8];
	double val[8];
};

/*
   Linearly interpolate the position where an isosurface cuts
   an edge between two vertices, each with their own scalar value
*/
struct XYZ VertexInterp(double isolevel, struct XYZ p1, struct XYZ p2, double valp1,double valp2)
{
   double mu;
   struct XYZ p;

   if (fabs(isolevel-valp1) < 0.000000001)
      return(p1);
   if (fabs(isolevel-valp2) < 0.000000001)
      return(p2);
   if (fabs(valp1-valp2) < 0.000000001)
      return(p1);
   mu = (isolevel - valp1) / (valp2 - valp1);

   p.x = p1.x + mu * (p2.x - p1.x);
   p.y = p1.y + mu * (p2.y - p1.y);
   p.z = p1.z + mu * (p2.z - p1.z);

   return(p);
}


/*
   Polygonise a tetrahedron given its vertices within a cube
   This is an alternative algorithm to polygonisegrid.
   It results in a smoother surface but more triangular facets.

                      + 0
                     /|\
                    / | \
                   /  |  \
                  /   |   \
                 /    |    \
                /     |     \
               +-------------+ 1
              3 \     |     /
                 \    |    /
                  \   |   /
                   \  |  /
                    \ | /
                     \|/
                      + 2

   It's main purpose is still to polygonise a gridded dataset and
   would normally be called 6 times, one for each tetrahedron making
   up the grid cell.
   Given the grid labelling as in PolygniseGrid one would call
      PolygoniseTri(grid,iso,triangles,0,2,3,7);
      PolygoniseTri(grid,iso,triangles,0,2,6,7);
      PolygoniseTri(grid,iso,triangles,0,4,6,7);
      PolygoniseTri(grid,iso,triangles,0,6,1,2);
      PolygoniseTri(grid,iso,triangles,0,6,1,4);
      PolygoniseTri(grid,iso,triangles,5,6,1,4);
*/
int PolygoniseTri(struct GRIDCELL grid,double iso,
        struct TRIANGLE *triangles,int v0,int v1,int v2,int v3)
{
   int ntriang = 0;
   int triindex;

   /*
      Determine which of the 16 cases we have given which vertices
      are above or below the isosurface
   */
   triindex = 0;
   if (grid.val[v0] < iso) triindex |= 1;
   if (grid.val[v1] < iso) triindex |= 2;
   if (grid.val[v2] < iso) triindex |= 4;
   if (grid.val[v3] < iso) triindex |= 8;

   /* Form the vertices of the triangles for each case */
   switch (triindex) {
   case 0x00:
   case 0x0F:
      break;
   case 0x0E:
   case 0x01:
      triangles[0].p[0] = VertexInterp(iso,grid.p[v0],grid.p[v1],grid.val[v0],grid.val[v1]);
      triangles[0].p[1] = VertexInterp(iso,grid.p[v0],grid.p[v2],grid.val[v0],grid.val[v2]);
      triangles[0].p[2] = VertexInterp(iso,grid.p[v0],grid.p[v3],grid.val[v0],grid.val[v3]);
      ntriang++;
      break;
   case 0x0D:
   case 0x02:
      triangles[0].p[0] = VertexInterp(iso,grid.p[v1],grid.p[v0],grid.val[v1],grid.val[v0]);
      triangles[0].p[1] = VertexInterp(iso,grid.p[v1],grid.p[v3],grid.val[v1],grid.val[v3]);
      triangles[0].p[2] = VertexInterp(iso,grid.p[v1],grid.p[v2],grid.val[v1],grid.val[v2]);
      ntriang++;
      break;
   case 0x0C:
   case 0x03:
      triangles[0].p[0] = VertexInterp(iso,grid.p[v0],grid.p[v3],grid.val[v0],grid.val[v3]);
      triangles[0].p[1] = VertexInterp(iso,grid.p[v0],grid.p[v2],grid.val[v0],grid.val[v2]);
      triangles[0].p[2] = VertexInterp(iso,grid.p[v1],grid.p[v3],grid.val[v1],grid.val[v3]);
      ntriang++;
      triangles[1].p[0] = triangles[0].p[2];
      triangles[1].p[1] = VertexInterp(iso,grid.p[v1],grid.p[v2],grid.val[v1],grid.val[v2]);
      triangles[1].p[2] = triangles[0].p[1];
      ntriang++;
      break;
   case 0x0B:
   case 0x04:
      triangles[0].p[0] = VertexInterp(iso,grid.p[v2],grid.p[v0],grid.val[v2],grid.val[v0]);
      triangles[0].p[1] = VertexInterp(iso,grid.p[v2],grid.p[v1],grid.val[v2],grid.val[v1]);
      triangles[0].p[2] = VertexInterp(iso,grid.p[v2],grid.p[v3],grid.val[v2],grid.val[v3]);
      ntriang++;
      break;
   case 0x0A:
   case 0x05:
      triangles[0].p[0] = VertexInterp(iso,grid.p[v0],grid.p[v1],grid.val[v0],grid.val[v1]);
      triangles[0].p[1] = VertexInterp(iso,grid.p[v2],grid.p[v3],grid.val[v2],grid.val[v3]);
      triangles[0].p[2] = VertexInterp(iso,grid.p[v0],grid.p[v3],grid.val[v0],grid.val[v3]);
      ntriang++;
      triangles[1].p[0] = triangles[0].p[0];
      triangles[1].p[1] = VertexInterp(iso,grid.p[v1],grid.p[v2],grid.val[v1],grid.val[v2]);
      triangles[1].p[2] = triangles[0].p[1];
      ntriang++;
      break;
   case 0x09:
   case 0x06:
      triangles[0].p[0] = VertexInterp(iso,grid.p[v0],grid.p[v1],grid.val[v0],grid.val[v1]);
      triangles[0].p[1] = VertexInterp(iso,grid.p[v1],grid.p[v3],grid.val[v1],grid.val[v3]);
      triangles[0].p[2] = VertexInterp(iso,grid.p[v2],grid.p[v3],grid.val[v2],grid.val[v3]);
      ntriang++;
      triangles[1].p[0] = triangles[0].p[0];
      triangles[1].p[1] = VertexInterp(iso,grid.p[v0],grid.p[v2],grid.val[v0],grid.val[v2]);
      triangles[1].p[2] = triangles[0].p[2];
      ntriang++;
      break;
   case 0x07:
   case 0x08:
      triangles[0].p[0] = VertexInterp(iso,grid.p[v3],grid.p[v0],grid.val[v3],grid.val[v0]);
      triangles[0].p[1] = VertexInterp(iso,grid.p[v3],grid.p[v2],grid.val[v3],grid.val[v2]);
      triangles[0].p[2] = VertexInterp(iso,grid.p[v3],grid.p[v1],grid.val[v3],grid.val[v1]);
      ntriang++;
      break;
   }

   return(ntriang);
};

IMTK::Mesh * tetradecomp(float * V, int xdim, int ydim, int zdim, float rad2)     
{ 
	int i, j, k, l, numT, dimxy, dimy;
	double I;
	struct GRIDCELL g;
	struct TRIANGLE t[5];
	IMTK::Mesh * mesh;
	IMTK::Vertex v1, v2, v3;
	IMTK::Triangle tri;

	I = rad2;

	dimxy = xdim*ydim;
	dimy = ydim;

	mesh = new IMTK::Mesh();

	mesh->setBounds(xdim, ydim, zdim);

	/* Compute the iosphote */
	for(i=0; i < xdim-1; i++){
		cout << "Slice " << i << endl;
		for(j=0; j < ydim-1; j++)
			for(k=0; k < zdim-1; k++){

				  g.p[0].x = (double)(i);
				  g.p[0].y = (double)(j);
				  g.p[0].z = (double)(k);
				  g.val[0] = *(V + dimxy*k + dimy*i + j);
				  g.p[1].x = (double)(i);
				  g.p[1].y = (double)(j+1);
				  g.p[1].z = (double)(k);
				  g.val[1] = *(V + dimxy*k + dimy*i + j+1);
				  g.p[2].x = (double)(i+1);
				  g.p[2].y = (double)(j+1);
				  g.p[2].z = (double)(k);
				  g.val[2] = *(V + dimxy*k + dimy*(i+1) + j+1);
				  g.p[3].x = (double)(i+1);
				  g.p[3].y = (double)(j);
				  g.p[3].z = (double)(k);
				  g.val[3] = *(V + dimxy*k + dimy*(i+1) + j);
				  g.p[4].x = (double)(i);
				  g.p[4].y = (double)(j);
				  g.p[4].z = (double)(k+1);
				  g.val[4] = *(V + dimxy*(k+1) + dimy*i + j);
				  g.p[5].x = (double)(i);
				  g.p[5].y = (double)(j+1);
				  g.p[5].z = (double)(k+1);
				  g.val[5] = *(V + dimxy*(k+1) + dimy*i + j+1);
				  g.p[6].x = (double)(i+1);
				  g.p[6].y = (double)(j+1);
				  g.p[6].z = (double)(k+1);
				  g.val[6] = *(V + dimxy*(k+1) + dimy*(i+1) + j+1);
				  g.p[7].x = (double)(i+1);
				  g.p[7].y = (double)(j);
				  g.p[7].z = (double)(k+1);
				  g.val[7] = *(V + dimxy*(k+1) + dimy*(i+1) + j);

				  numT = PolygoniseTri(g,I,t,0,2,3,7);
				  for(l = 0; l < numT; l++){
					  v1.set(t[l].p[0].x, t[l].p[0].y, t[l].p[0].z);
					  v2.set(t[l].p[1].x, t[l].p[1].y, t[l].p[1].z);
					  v3.set(t[l].p[2].x, t[l].p[2].y, t[l].p[2].z);
					  tri.set(&v1, &v2, &v3);

					  mesh->addTriangle(tri);
				  }
				  
				  numT = PolygoniseTri(g,I,t,0,2,6,7);
				  for(l = 0; l < numT; l++){
					  v1.set(t[l].p[0].x, t[l].p[0].y, t[l].p[0].z);
					  v2.set(t[l].p[1].x, t[l].p[1].y, t[l].p[1].z);
					  v3.set(t[l].p[2].x, t[l].p[2].y, t[l].p[2].z);
					  tri.set(&v1, &v2, &v3);

					  mesh->addTriangle(tri);
				  }
				  
				  numT = PolygoniseTri(g,I,t,0,4,6,7);
				  for(l = 0; l < numT; l++){
					  v1.set(t[l].p[0].x, t[l].p[0].y, t[l].p[0].z);
					  v2.set(t[l].p[1].x, t[l].p[1].y, t[l].p[1].z);
					  v3.set(t[l].p[2].x, t[l].p[2].y, t[l].p[2].z);
					  tri.set(&v1, &v2, &v3);

					  mesh->addTriangle(tri);
				  }
				  
				  numT = PolygoniseTri(g,I,t,0,6,1,2);
				  for(l = 0; l < numT; l++){
					  v1.set(t[l].p[0].x, t[l].p[0].y, t[l].p[0].z);
					  v2.set(t[l].p[1].x, t[l].p[1].y, t[l].p[1].z);
					  v3.set(t[l].p[2].x, t[l].p[2].y, t[l].p[2].z);
					  tri.set(&v1, &v2, &v3);

					  mesh->addTriangle(tri);
				  }
				  
				  numT = PolygoniseTri(g,I,t,0,6,1,4);
				  for(l = 0; l < numT; l++){
					  v1.set(t[l].p[0].x, t[l].p[0].y, t[l].p[0].z);
					  v2.set(t[l].p[1].x, t[l].p[1].y, t[l].p[1].z);
					  v3.set(t[l].p[2].x, t[l].p[2].y, t[l].p[2].z);
					  tri.set(&v1, &v2, &v3);

					  mesh->addTriangle(tri);
				  }
				  
				  numT = PolygoniseTri(g,I,t,5,6,1,4);				  
				  for(l = 0; l < numT; l++){
					  v1.set(t[l].p[0].x, t[l].p[0].y, t[l].p[0].z);
					  v2.set(t[l].p[1].x, t[l].p[1].y, t[l].p[1].z);
					  v3.set(t[l].p[2].x, t[l].p[2].y, t[l].p[2].z);
					  tri.set(&v1, &v2, &v3);

					  mesh->addTriangle(tri);
				  }
			}
	}

	return mesh;
};

int main(int argv, char **argc)
{
   char outfilename[100];
   int xdim, ydim, zdim, radius;
   int centerx, centery, centerz;
   int i, j, k;
   float dx, dy, dz, d2, r2, *data;
   IMTK::Mesh * m, m2;

   if(argv < 6)
    {
      fprintf(stderr, "Usage: mesh_test xdim ydim zdim radius outfile.oogl\n");
      exit(1);
    }

   //parse command line, no checking!
   xdim = atoi(*(argc+1));
   ydim = atoi(*(argc+2));
   zdim = atoi(*(argc+3));
   radius = atoi(*(argc+4));
   strcpy(outfilename, *(argc+5));

   centerx = (int)((((float)(xdim))/2) + 0.5);
   centery = (int)((((float)(ydim))/2) + 0.5);
   centerz = (int)((((float)(zdim))/2) + 0.5);

   data = new float[xdim*ydim*zdim];
   r2 = radius*radius;

   cout << "Creating test volume" << xdim << "x" << ydim << "x" << zdim << endl;

   for(i=0; i < xdim; i++)
	   for(j=0; j < ydim; j++)
		   for(k=0; k < zdim; k++){
			   dx = (float)(centerx - i);
			   dy = (float)(centery - j);
			   dz = (float)(centerz - k);

			   d2 = dx*dx + dy*dy + dz*dz;

			   *(data + k*xdim*ydim + i*ydim + j) = sqrt(d2);
		   }

   cout << "Computing Surface" << endl;
   //Compute surface
   m = tetradecomp(data, xdim, ydim, zdim, radius);

   cout << "Writing File" << endl;
   //write surface
   m->ooglwrite(outfilename);
   
   m2.ooglread(outfilename);

   m2.ooglwrite("read.oogl");
   
   delete m;
   delete data;

}

