/********************************************************
File: Volume_ext.hh
Abstract: A extended 3 dimensional volume template derived 
          from the template Volume. 
Created: 09/28/2001

(C) 2002 by Wake Forest University School of Medicine

Last Revision ($Revision: 1.1.1.1 $) 
by $ Hong Li $ 
on $Date: 2002/12/06 21:49:31 $
*********************************************************/

#ifndef _Volume_ext_hh
#define _Volume_ext_hh 

#include "LabeledVolume.hh"
#include "mvVec3f.hh"
#include "Vertex_with_Features.hh"

/** Extended 3 dimensional volume template.
Here we only derive the subclass to implement the calculation
 of the normal direction of a given point(vertex). 
@see Volume.hh for more detail. 
*/

template <class T>
class Volume_ext: public LabeledVolume <T> 
{
protected:
		
public:
	/**
	*	An constructor 
	*/
	Volume_ext() {
		setVoxelSpacing(1,1,1);
		setOrigin(0,0,0);
			}
			
	/**
	*	Calculate the normal direction
	*	@param vert the input vertex for calculation
	*	@return the normal direction
	*/		
	mvVec3f * getNormDir(Vertex_with_Features& vert)
	{
	int i, j, k, ii;
	float x = vert.getX();
	float y = vert.getY();
	float z = vert.getZ();
	IMTK::Subscript<3> dim = getBounds();
	unsigned short intx = (unsigned short)floor(x);
	unsigned short inty = (unsigned short)floor(y);
	unsigned short intz = (unsigned short)floor(z);

	float dfdx[8];
	float dfdy[8];
	float dfdz[8];

	float mask[3][3] = {{1.0/16.0, 1.0/8.0, 1.0/16.0},
        	            {1.0/8.0, 1.0/4.0, 1.0/8.0},
			    {1.0/16.0, 1.0/8.0, 1.0/16.0}};
		    			 
	
	if (intx <= 1 ||inty <= 1 || intz<= 1 ) return NULL;
	if (intx >= dim[0]-2 ||inty >= dim[1]-2 || intz>= dim[2]-2) return NULL;


	IMTK::Subscript<3> center[8]; 
	float weight[8];
	center[0][0] = intx ;
	center[0][1] = inty ;
	center[0][2] = intz ;
	weight[0] = (intx+1-x)*(inty+1-y)*(intz+1-z);
		
	center[1][0] = intx + 1;
	center[1][1] = inty ;
	center[1][2] = intz ;
	weight[1] = (x - intx)*(inty+1-y)*(intz+1-z);
	
	center[2][0] = intx ;
	center[2][1] = inty + 1;
	center[2][2] = intz ;
	weight[2] = (intx+1-x)*(y-inty)*(intz+1-z);
	
	center[3][0] = intx + 1;
	center[3][1] = inty + 1;
	center[3][2] = intz ;
	weight[3] = (x-intx)*(y-inty)*(intz+1-z);
	
	center[4][0] = intx ;
	center[4][1] = inty ;
	center[4][2] = intz + 1;
	weight[4] = (intx+1-x)*(inty+1-y)*(z-intz);
	
	center[5][0] = intx + 1;
	center[5][1] = inty ;
	center[5][2] = intz + 1;
	weight[5] = (x-intx)*(inty+1-y)*(z-intz);
	
	center[6][0] = intx ;
	center[6][1] = inty + 1;
	center[6][2] = intz + 1;
	weight[6] = (intx+1-x)*(y-inty)*(z-intz);
	
	center[7][0] = intx + 1;
	center[7][1] = inty + 1;
	center[7][2] = intz + 1;
	weight[7] = (x-intx)*(y-inty)*(z-intz);


	float a[8][3][3][3]; 
	for (ii = 0; ii < 8;ii++)
		{
		dfdx[ii] = dfdy[ii]=dfdz[ii] = 0;
		for (i = 0; i < 3; i++)
			for ( j = 0; j < 3; j++)
				for ( k = 0; k < 3; k++)
		 			{
					a[ii][i][j][k] = (float)getVoxel(center[ii][0]-1+i,
					center[ii][1]-1+j, center[ii][2]-1+k);
					}
		}		
		
	for (i = 0; i < 8 ;i++)
		for(j = 0;j<3;j++)
			for(k=0;k<3;k++)
			       {
		       		dfdx[i] += mask[j][k]*(a[i][2][j][k]-a[i][0][j][k]);
		       		dfdy[i] += mask[j][k]*(a[i][j][2][k]-a[i][j][0][k]);
		       		dfdz[i] += mask[j][k]*(a[i][j][k][2]-a[i][j][k][0]);
				}				


	mvVec3f * norm  = new(mvVec3f);
	for (i=0;i<8;i++)
		{
		norm->x += weight[i]*dfdx[i];
		norm->y += weight[i]*dfdy[i];
		norm->z += weight[i]*dfdz[i];
		}
	norm->normalize();

	return 	norm;		

	}

/**
*	Calculate the value of a given point in the volume
*       by interpolation. 
*	@param sx the x coordinate of the point
*	@param sy the y coordinate of the point
*	@param sz the z coordinate of the point
*	@return the output value in float type
*/		

float getValue(float sx, float sy, float sz)
{
float value;
int i;
float dx, dy, dz;
float xorig, yorig, zorig;

getVoxelSpacing(dx,dy,dz);
getOrigin(xorig,yorig,zorig);

if(dx<=0 ||dy <=0||dz<=0) 
	{
	dx = 1;
	dy = 1;
	dz = 1;
	}

float x = (sx-xorig)/dx;
float y = (sy-yorig)/dy;
float z = (sz-zorig)/dz;

int intx = (int)floor(x);
int inty = (int)floor(y);
int intz = (int)floor(z);

IMTK::Subscript<3> dim = getBounds();

if (intx < 0 ||inty < 0 || intz < 0 ) return 0;
if (intx >= (int)dim[0]-1 ||inty >= (int)dim[1]-1 || intz>= (int)dim[2]-1) return 0;

	IMTK::Subscript<3> center[8]; 
	float weight[8];
	center[0][0] = intx ;
	center[0][1] = inty ;
	center[0][2] = intz ;
	weight[0] = (intx+1-x)*(inty+1-y)*(intz+1-z);
		
	center[1][0] = intx + 1;
	center[1][1] = inty ;
	center[1][2] = intz ;
	weight[1] = (x - intx)*(inty+1-y)*(intz+1-z);
	
	center[2][0] = intx ;
	center[2][1] = inty + 1;
	center[2][2] = intz ;
	weight[2] = (intx+1-x)*(y-inty)*(intz+1-z);
	
	center[3][0] = intx + 1;
	center[3][1] = inty + 1;
	center[3][2] = intz ;
	weight[3] = (x-intx)*(y-inty)*(intz+1-z);
	
	center[4][0] = intx ;
	center[4][1] = inty ;
	center[4][2] = intz + 1;
	weight[4] = (intx+1-x)*(inty+1-y)*(z-intz);
	
	center[5][0] = intx + 1;
	center[5][1] = inty ;
	center[5][2] = intz + 1;
	weight[5] = (x-intx)*(inty+1-y)*(z-intz);
	
	center[6][0] = intx ;
	center[6][1] = inty + 1;
	center[6][2] = intz + 1;
	weight[6] = (intx+1-x)*(y-inty)*(z-intz);
	
	center[7][0] = intx + 1;
	center[7][1] = inty + 1;
	center[7][2] = intz + 1;
	weight[7] = (x-intx)*(y-inty)*(z-intz);



	value = 0;
	for (i=0;i<8;i++)
		value += weight[i]*(float)getVoxel(center[i][0], center[i][1],center[i][2]);
return  value;
}
	
};
#endif
