

/************************************* dt3d3_4_5.c ***********************/
/*
 *	Author: 	Yaorong Ge
 * 	Date:		6-29-93
 *	Purpose: 	
 *				Given a binary image volume, create a 3D Chamfer 3-4-5 distance
 *				transform
 *				Mask
 *					5 4 5 	4 3 4	5 4 5	
 *                  4 3 4	3 0 3	4 3 4 
 *  				5 4 5	4 3 4	5 4 5 
 *
 *				Forward mask:
 *					5 4 5 	4 3 4
 *					4 3 4 	3 0
 *					5 4 5
 *
 *				Backward mask:
 *							5 4 5
 *					  0 3	4 3 4
 *					4 3 4	5 4 5
 *
 *				Assume here that the outimage has been initialized properly,
 *				i.e., the boundary points are zero and all other points are
 *				MAGIC_NUMBER.
 */

#include "fast_calc_dt.h"

static int mask3d[13]={5, 4, 5, 4, 3, 4, 5, 4, 5, 4, 3, 4, 3};

void dt3d3_4_5(short *outimage,
			  int dimx,
			  int dimy,
			  int dimz)
{
	
	int i,j,m;
	int dist, mindist;
	int dimxy,idx,idx1;

	dimxy=dimx*dimy;
		for(m=1;m<dimz;m++){
			for(j=1;j<dimy;j++){
				for(i=1;i<dimx-1;i++){
					mindist=MAGIC_NUMBER;
					idx=m*dimxy+j*dimx+i;

					/* -1 -1 -1 */
					idx1=idx-dimxy-dimx-1;
					dist=mask3d[0]+outimage[idx1];
					if(dist<mindist){
						mindist=dist;
					}
					/* 0 -1 -1 */
					idx1=idx-dimxy-dimx;
					dist=mask3d[1]+outimage[idx1];
					if(dist<mindist){
						mindist=dist;
					}
					/* 1 -1 -1 */
					idx1=idx-dimxy-dimx+1;
					dist=mask3d[2]+outimage[idx1];
					if(dist<mindist){
						mindist=dist;
					}
					/* -1 0 -1 */
					idx1=idx-dimxy-1;
					dist=mask3d[3]+outimage[idx1];
					if(dist<mindist){
						mindist=dist;
					}
					/*  0 0 -1 */
					idx1=idx-dimxy;
					dist=mask3d[4]+outimage[idx1];
					if(dist<mindist){
						mindist=dist;
					}
					/* 1 0 -1 */
					idx1=idx-dimxy+1;
					dist=mask3d[5]+outimage[idx1];
					if(dist<mindist){
						mindist=dist;
					}
					/* -1 1 -1 */
					idx1=idx-dimxy+dimx-1;
					dist=mask3d[6]+outimage[idx1];
					if(dist<mindist){
						mindist=dist;
					}
					/* 0 1 -1 */
					idx1=idx-dimxy+dimx;
					dist=mask3d[7]+outimage[idx1];
					if(dist<mindist){
						mindist=dist;
					}
					/* 1 1 -1 */
					idx1=idx-dimxy+dimx+1;
					dist=mask3d[8]+outimage[idx1];
					if(dist<mindist){
						mindist=dist;
					}
					/* -1 -1 0 */
					idx1=idx-dimx-1;
					dist=mask3d[9]+outimage[idx1];
					if(dist<mindist){
						mindist=dist;
					}
					/* 0 -1 0 */
					idx1=idx-dimx;
					dist=mask3d[10]+outimage[idx1];
					if(dist<mindist){
						mindist=dist;
					}
					/* 1 -1 0 */
					idx1=idx-dimx+1;
					dist=mask3d[11]+outimage[idx1];
					if(dist<mindist){
						mindist=dist;
					}
					/* -1 0 0 */
					idx1=idx-1;
					dist=mask3d[12]+outimage[idx1];
					if(dist<mindist){
						mindist=dist;
					}
					if(mindist<outimage[idx]){
						outimage[idx]=mindist;
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
					dist=mask3d[0]+outimage[idx1];
					if(dist<mindist){
						mindist=dist;
					}
					/* 0 1 1 */
					idx1=idx+dimxy+dimx;
					dist=mask3d[1]+outimage[idx1];
					if(dist<mindist){
						mindist=dist;
					}
					/* -1 1 1 */
					idx1=idx+dimxy+dimx-1;
					dist=mask3d[2]+outimage[idx1];
					if(dist<mindist){
						mindist=dist;
					}
					/*  1 0 1 */
					idx1=idx+dimxy+1;
					dist=mask3d[3]+outimage[idx1];
					if(dist<mindist){
						mindist=dist;
					}
					/* 0 0 1 */
					idx1=idx+dimxy;
					dist=mask3d[4]+outimage[idx1];
					if(dist<mindist){
						mindist=dist;
					}
					/* -1 0 1 */
					idx1=idx+dimxy-1;
					dist=mask3d[5]+outimage[idx1];
					if(dist<mindist){
						mindist=dist;
					}
					/* 1 -1 1 */
					idx1=idx+dimxy-dimx+1;
					dist=mask3d[6]+outimage[idx1];
					if(dist<mindist){
						mindist=dist;
					}
					/* 0 -1 1 */
					idx1=idx+dimxy-dimx;
					dist=mask3d[7]+outimage[idx1];
					if(dist<mindist){
						mindist=dist;
					}
					/* -1 -1 1 */
					idx1=idx+dimxy-dimx-1;
					dist=mask3d[8]+outimage[idx1];
					if(dist<mindist){
						mindist=dist;
					}
					/* 1 1 0 */
					idx1=idx+dimx+1;
					dist=mask3d[9]+outimage[idx1];
					if(dist<mindist){
						mindist=dist;
					}
					/* 0 1 0 */
					idx1=idx+dimx;
					dist=mask3d[10]+outimage[idx1];
					if(dist<mindist){
						mindist=dist;
					}
					/* -1 1 0 */
					idx1=idx+dimx-1;
					dist=mask3d[11]+outimage[idx1];
					if(dist<mindist){
						mindist=dist;
					}
					/* 1 0 0 */
					idx1=idx+1;
					dist=mask3d[12]+outimage[idx1];
					if(dist<mindist){
						mindist=dist;
					}
					if(mindist<outimage[idx]){
						outimage[idx]=mindist;
					}
				}
			}
		}
}

/*void calc_dt(short *imgVolume, short *binaryVolume, 
int dimx, int dimy, int dimz)
{
	short *sptr;
	int i,dimxyz;

	dimxyz=dimx*dimy*dimz;
	for(i=0;i<dimxyz;i++){
		if(imgVolume[i]==1) binaryVolume[i] = MAGIC_NUMBER;
		else binaryVolume[i] = 0;
	}
	dt3d3_4_5(binaryVolume,dimx,dimy,dimz);
}*/
