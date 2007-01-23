/********************************************************
File: mask.cc
Abstract: 

See header (mask.hh) for documentation
*********************************************************/

#include <cstdio>

#include "mask.hh"

#include "RBTree.hh"

using namespace IMTK;
using namespace std;

void maskBg(Volume<unsigned short> &data, Volume<unsigned short> &region, float thresh)
{

	int x, y, z;
	int n, pixelx, pixely, pixelz, found, num;
	unsigned short value;
	double key;
	RBTree< Subscript<3> > stack;
	Subscript<3> bounds, tempP, stack_top;

	//********** first, segment background

	// initialize region
	region.fill(255);

	// seed is first voxel (assuming it is in background)
	x= 0;
	y= 0;
	z= 0;

	bounds = data.getBounds();

	num = 0;

	// put seed in if < thesh
	value = data.getVoxel(x, y, z);
	if( (value < thresh) || (value == 2096) ){
		tempP[0] = x;
		tempP[1] = y;
		tempP[2] = z;

		region.set(tempP, 0);

		key = (double)(tempP.getIndex(bounds));
		stack.insertNode(tempP, key);

		num += 1;
	}
    
	while(!stack.isEmpty()){

		// iterate through the 6 neighgbors
		for(n = 0; n < 6; n++){
			pixelx = (n == 0) ? x-1 : x;
 			pixelx = (n == 1) ? x+1 : pixelx;
			pixely = (n == 2) ? y-1 : y;
 			pixely = (n == 3) ? y+1 : pixely;
			pixelz = (n == 4) ? z-1 : z;
 			pixelz = (n == 5) ? z+1 : pixelz;

					
			if((pixelx >= 0 && pixelx < bounds[0]) &&
			   (pixely >= 0 && pixely < bounds[1]) &&
			   (pixelz >= 0 && pixelz < bounds[2])){
				value = data.getVoxel(pixelx, pixely, pixelz);
				if(  (value < thresh) || (value == 2096) ){ 
					// check if this voxel has been examined or is already on stack
					if( region.getVoxel(pixelx, pixely, pixelz) ){
						// push onto stack if not in it already
						tempP[0] = pixelx;
						tempP[1] = pixely;
						tempP[2] = pixelz;

						key = (double)(tempP.getIndex(bounds));
						stack.binTreeSearch(key, &found);
						if(!found){
							stack.insertNode(tempP, key);
							//clog << "Push " << pixelx << ", " << pixely << ", " << pixelz << endl;
						}
					}
				}
			}
		}

		stack_top = stack.getRoot();
		x = stack_top[0];
		y = stack_top[1];
		z = stack_top[2];
		
		//clog << "Pop " << x << ", " << y << ", " << z << endl;

		region.set(stack_top, 0);

		key = (double)(stack_top.getIndex(bounds));
		stack.deleteNode(key);

		num += 1;

		//clog << "Press Return to continue ....." << endl;
		//getc(stdin);
	}


	//********** next loop through, thresholding for tissue pointwise
	int i;
	for(i = 0; i < data.size(); i++){
		if(data[i] >= thresh) region[i] = 0;
	}

	
	clog << num << " Voxels in BG mask." << endl;
};

void maskRegion(int sx, int sy, int sz, IMTK::Volume<unsigned short> &data)
{
	int x, y, z;
	int n, pixelx, pixely, pixelz, found, num;
	unsigned short value;
	double key;
	RBTree< Subscript<3> > stack;
	Subscript<3> bounds, tempP, stack_top;

	// initialize with seed
	x= sx;
	y= sy;
	z= sz;

	bounds = data.getBounds();

	num = 0;

	// put seed in if > 0
	value = data.getVoxel(x, y, z);
	if(value > 0){
		tempP[0] = x;
		tempP[1] = y;
		tempP[2] = z;

		key = (double)(tempP.getIndex(bounds));
		stack.insertNode(tempP, key);

		num += 1;
	}
    
	while(!stack.isEmpty()){

		// iterate through the 6 neighgbors
		for(n = 0; n < 6; n++){
			pixelx = (n == 0) ? x-1 : x;
 			pixelx = (n == 1) ? x+1 : pixelx;
			pixely = (n == 2) ? y-1 : y;
 			pixely = (n == 3) ? y+1 : pixely;
			pixelz = (n == 4) ? z-1 : z;
 			pixelz = (n == 5) ? z+1 : pixelz;

					
			if((pixelx >= 0 && pixelx < bounds[0]) &&
			   (pixely >= 0 && pixely < bounds[1]) &&
			   (pixelz >= 0 && pixelz < bounds[2])){
				value = data.getVoxel(pixelx, pixely, pixelz);
				if(value > 0){ 
					// push onto stack if not in it already
					tempP[0] = pixelx;
					tempP[1] = pixely;
					tempP[2] = pixelz;
					
					key = (double)(tempP.getIndex(bounds));
					stack.binTreeSearch(key, &found);
					if(!found){
						stack.insertNode(tempP, key);
					}
				}
			}
		}

		stack_top = stack.getRoot();
		x = stack_top[0];
		y = stack_top[1];
		z = stack_top[2];

		data.set(stack_top, 0);

		key = (double)(stack_top.getIndex(bounds));
		stack.deleteNode(key);

		num += 1;

	}

	clog << num << " Voxels in mask." << endl;
};
