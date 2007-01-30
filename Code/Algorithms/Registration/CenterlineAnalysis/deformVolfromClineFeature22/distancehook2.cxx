/*=================================================================
 *
 * distancehook.c	MEX file to calculate the distance
 *                      transform of a 3D shape.
 *
 * The calling syntax is:
 *
 *		[3D Numerical Array] = distance(3D Numerical Array)
 *
 *
 *=================================================================*/
/* $Revision: 1.10 $ */
#include <math.h>
//#include "mex.h"
#include "tnt/tnt_array3d.h"
#include "distance.h"
#include "local_maxima.h"
#include "lut.h"
#include <vector>
#include <string.h>

//vector< localMaximum<int> > LMList;
//vector< localMaximum<int> > MAList;


void fillArray(TNT::Array3D<int> &M, double *p)
{
  int dimx = M.dim1();
  int dimy = M.dim2();
  int dimz = M.dim3();
  int max = 0, mx=0, my=0, mz=0;

  for(int i=0; i < dimx; i++){
    for(int j=0; j < dimy; j++){
      for(int k=0; k < dimz; k++){
	*(p + k*dimx*dimy + j*dimx + i) = M[i][j][k];

	if(max < M[i][j][k]){
	    max = M[i][j][k];
	    mx = i;  my = j;  mz = k;
	}

      }
    }
  }

  //mexPrintf("max value=%d, mx=%d, my=%d, mz=%d\n", max, mx, my, mz);
}


void findMAX_DT(TNT::Array3D<int> &M)
{
  int dimx = M.dim1();
  int dimy = M.dim2();
  int dimz = M.dim3();
  int max = 0, mx=0, my=0, mz=0;

  for(int i=0; i < dimx; i++){
    for(int j=0; j < dimy; j++){
      for(int k=0; k < dimz; k++){
	//	*(p + k*dimx*dimy + j*dimx + i) = M[i][j][k];

	if(max < M[i][j][k]){
	    max = M[i][j][k];
	    mx = i;  my = j;  mz = k;
	}

      }
    }
  }

  //clog << "max value="<< max << " mx="<<mx<<" my="<<my<<" mz="<<mz<<endl;
  //  mexPrintf("max value=%d, mx=%d, my=%d, mz=%d\n", max, mx, my, mz);
}

inline void arrayFill(TNT::Array3D<int> &M, int *p)
{
  int dimx = M.dim1();
  int dimy = M.dim2();
  int dimz = M.dim3();
  for(int i=0; i < dimx; i++){
    for(int j=0; j < dimy; j++){
      for(int k=0; k < dimz; k++){
	M[i][j][k] = *(p + k*dimx*dimy + j*dimx + i);
      }
    }
  }
}

#if 0
void fillMAMatrix(double *p)
{
  int dim1 = LMList.size();

  clog << " MAList size =" << dim1 << endl;

  for(int i=0; i < dim1; i++) {
    *(p+i) = LMList[i].y+1;
    *(p+dim1+i) = LMList[i].x+1;
    *(p+dim1*2+i) = LMList[i].z+1;
    *(p+dim1*3+i) = LMList[i].radius;
  }
}


void fillMAwithVoxel(double *p, TNT::Array3D<int> &M)
{
  int dim1 = LMList.size();
  int xx=0, yy=0, zz=0;

  clog << " MAList size =" << dim1 << endl;

  for(int i=0; i < dim1; i++) {
     TNT::Array3D<double> vin(dimx, dimy, dimz)
    yy = LMList[i].y+1;
    xx = LMList[i].x+1;
    zz = LMList[i].z+1;
    *(p+i) = yy;
    *(p+dim1+i) = xx;
    *(p+dim1*2+i) = zz;
    *(p+dim1*3+i) = LMList[i].radius;

    //    M[xx][yy][zz] +=1;  // display centerline by wook
  }
}


void addCenterLine2Z(double *slice, int sizeX, int sizeY, int sizeZ)
{
  int lm,i,j;
  int dim1 = LMList.size();
  int xx=0, yy=0, zz=0;

  for(lm=0; lm<dim1; lm++){

    yy = LMList[lm].y+1;
    xx = LMList[lm].x+1;
    zz = LMList[lm].z+1;
    
    *(slice + xx*sizeX + yy) = 0.19;
  }

}

void addCenterLine2Y(double *slice, int sizeX, int sizeY, int sizeZ)
{
  int lm,i,j;
  int dim1 = LMList.size();
  int xx=0, yy=0, zz=0;

  for(lm=0; lm<dim1; lm++){

    yy = LMList[lm].y+1;
    xx = LMList[lm].x+1;
    zz = LMList[lm].z+1;
    
    *(slice + zz*sizeX + xx) = 0.19;
  }

}
void addCenterLine2X(double *slice, int sizeX, int sizeY, int sizeZ)
{
  int lm,i,j;
  int dim1 = LMList.size();
  int xx=0, yy=0, zz=0; TNT::Array3D<double> vin(dimx, dimy, dimz)

  for(lm=0; lm<dim1; lm++){

    yy = LMList[lm].y+1;
    xx = LMList[lm].x+1;
    zz = LMList[lm].z+1;
    
    *(slice + zz*sizeZ + yy) = 0.19;
  }

}
#endif

void distancehook(
		  TNT::Array3D<int> &v,
		  int dimx ,
		  int dimy,
		  int dimz
		  // double *out_ptr,
		  // int &num_LM,
		  // int &num_MA
		  )
{
  //  TNT::Array3D<double> vin(dimx, dimy, dimz);
  //arrayFill(vin, in_ptr);
  //clog << "In distancehook function" << endl;
  //TNT::Array3D<int> vout = vin.copy();    

  //clog << "after setting vout" << endl;
  distance(v);

  //clog << "after distance( )" << endl;
  /*
  findLocalMaxima(v, LMList);


  clog << "after findLoaclMaxima()" << endl;

  LUT(v, LMList);
  num_MA = LMList.size();   clog << " LMList size =" << num_MA << endl;
  */  
//  fillArray(vout, out_ptr);
}

void projectionZ(TNT::Array3D<int> &M, double *slice, int sizeX, int sizeY, int sizeZ)
{
  int k,i,j;

  for(k=0; k<sizeZ; k++){
    for(i=0; i<sizeX; i++){
    for(j=0; j<sizeY; j++){

      *(slice + i*sizeX + j) += (M[i][j][k]/(double)sizeZ);
      if(*(slice + i*sizeX + j)>3.5) *(slice + i*sizeX + j)=3.5;
    }
    }
  } // k
  
}

void projectionY(TNT::Array3D<int> &M, double *slice, int sizeX, int sizeY, int sizeZ)
{
  int k,i,j;

  for(j=0; j<sizeY; j++){
    for(k=0; k<sizeZ; k++){
    for(i=0; i<sizeX; i++){


      *(slice + k*sizeX + i) += (M[i][j][k]/(double)sizeY);
       if(*(slice + k*sizeX + i)>3.5) *(slice + k*sizeX + i)=3.5;
    }
    }
  } // j
  
}


void projectionX(TNT::Array3D<int> &M, double *slice, int sizeX, int sizeY, int sizeZ)
{
  int k,i,j;

  for(i=0; i<sizeX; i++){
    for(j=0; j<sizeY; j++){
    for(k=0; k<sizeZ; k++){


      *(slice + k*sizeZ + j) += (M[i][j][k]/(double)sizeX);
       if(*(slice + k*sizeZ + j)>3.5) *(slice + k*sizeZ + j)=3.5;
    }
    }
  } // i
  
}

    
