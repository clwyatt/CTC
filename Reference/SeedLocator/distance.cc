/********************************************************
File: distance.cc
Abstract: 

See header (distance.hh) for documentation
*********************************************************/

#include <cstdio>

#include "distance.hh"


using namespace IMTK;

static int mask3d[13]={5, 4, 5, 4, 3, 4, 5, 4, 5, 4, 3, 4, 3};

#define MAGIC_NUMBER 255

void computeDT(IMTK::Volume<unsigned short> &data)
{
	int i,j,m;
	int dist, mindist;
	int dimx, dimy, dimz, dimxy,idx,idx1;

	IMTK::Subscript<3> bounds = data.getBounds();
	dimx = bounds[0];
	dimy = bounds[1];
	dimz = bounds[2];

	dimxy=dimx*dimy;

	for(m=1;m<dimz;m++){
		for(j=1;j<dimy;j++){
			for(i=1;i<dimx-1;i++){
				mindist=MAGIC_NUMBER;
				idx=m*dimxy+j*dimx+i;
				
				/* -1 -1 -1 */
				idx1=idx-dimxy-dimx-1;
				dist=mask3d[0]+data[idx1];
				if(dist<mindist){
					mindist=dist;
				}
				/* 0 -1 -1 */
				idx1=idx-dimxy-dimx;
				dist=mask3d[1]+data[idx1];
				if(dist<mindist){
					mindist=dist;
				}
				/* 1 -1 -1 */
				idx1=idx-dimxy-dimx+1;
				dist=mask3d[2]+data[idx1];
				if(dist<mindist){
					mindist=dist;
				}
				/* -1 0 -1 */
				idx1=idx-dimxy-1;
				dist=mask3d[3]+data[idx1];
				if(dist<mindist){
					mindist=dist;
				}
				/*  0 0 -1 */
				idx1=idx-dimxy;
				dist=mask3d[4]+data[idx1];
				if(dist<mindist){
					mindist=dist;
				}
				/* 1 0 -1 */
				idx1=idx-dimxy+1;
				dist=mask3d[5]+data[idx1];
				if(dist<mindist){
					mindist=dist;
				}
				/* -1 1 -1 */
				idx1=idx-dimxy+dimx-1;
				dist=mask3d[6]+data[idx1];
				if(dist<mindist){
					mindist=dist;
				}
				/* 0 1 -1 */
				idx1=idx-dimxy+dimx;
				dist=mask3d[7]+data[idx1];
				if(dist<mindist){
					mindist=dist;
				}
				/* 1 1 -1 */
				idx1=idx-dimxy+dimx+1;
				dist=mask3d[8]+data[idx1];
				if(dist<mindist){
					mindist=dist;
				}
				/* -1 -1 0 */
				idx1=idx-dimx-1;
				dist=mask3d[9]+data[idx1];
				if(dist<mindist){
					mindist=dist;
				}
				/* 0 -1 0 */
				idx1=idx-dimx;
				dist=mask3d[10]+data[idx1];
				if(dist<mindist){
					mindist=dist;
				}
				/* 1 -1 0 */
				idx1=idx-dimx+1;
				dist=mask3d[11]+data[idx1];
				if(dist<mindist){
					mindist=dist;
				}
				/* -1 0 0 */
				idx1=idx-1;
				dist=mask3d[12]+data[idx1];
				if(dist<mindist){
					mindist=dist;
				}
				if(mindist<data[idx]){
					data[idx]=mindist;
				}
			}
		}
	}
	
	for(m=dimz-2;m>=0;m--){
		for(j=dimy-2;j>=0;j--){
			for(i=dimx-2;i>=1;i--){
				mindist=MAGIC_NUMBER;
				idx=m*dimxy+j*dimx+i;
				
				/* 1 1 1 */
				idx1=idx+dimxy+dimx+1;
				dist=mask3d[0]+data[idx1];
				if(dist<mindist){
					mindist=dist;
				}
				/* 0 1 1 */
				idx1=idx+dimxy+dimx;
				dist=mask3d[1]+data[idx1];
				if(dist<mindist){
					mindist=dist;
				}
				/* -1 1 1 */
				idx1=idx+dimxy+dimx-1;
				dist=mask3d[2]+data[idx1];
				if(dist<mindist){
					mindist=dist;
				}
				/*  1 0 1 */
				idx1=idx+dimxy+1;
				dist=mask3d[3]+data[idx1];
				if(dist<mindist){
					mindist=dist;
				}
				/* 0 0 1 */
				idx1=idx+dimxy;
				dist=mask3d[4]+data[idx1];
				if(dist<mindist){
					mindist=dist;
				}
				/* -1 0 1 */
				idx1=idx+dimxy-1;
				dist=mask3d[5]+data[idx1];
				if(dist<mindist){
					mindist=dist;
				}
				/* 1 -1 1 */
				idx1=idx+dimxy-dimx+1;
				dist=mask3d[6]+data[idx1];
				if(dist<mindist){
					mindist=dist;
				}
				/* 0 -1 1 */
				idx1=idx+dimxy-dimx;
				dist=mask3d[7]+data[idx1];
				if(dist<mindist){
					mindist=dist;
				}
				/* -1 -1 1 */
				idx1=idx+dimxy-dimx-1;
				dist=mask3d[8]+data[idx1];
				if(dist<mindist){
						mindist=dist;
				}
				/* 1 1 0 */
				idx1=idx+dimx+1;
				dist=mask3d[9]+data[idx1];
				if(dist<mindist){
					mindist=dist;
				}
				/* 0 1 0 */
				idx1=idx+dimx;
				dist=mask3d[10]+data[idx1];
				if(dist<mindist){
					mindist=dist;
				}
				/* -1 1 0 */
				idx1=idx+dimx-1;
				dist=mask3d[11]+data[idx1];
				if(dist<mindist){
					mindist=dist;
				}
				/* 1 0 0 */
				idx1=idx+1;
				dist=mask3d[12]+data[idx1];
				if(dist<mindist){
					mindist=dist;
				}
				if(mindist<data[idx]){
					data[idx]=mindist;
				}
			}
		}
	}
};
