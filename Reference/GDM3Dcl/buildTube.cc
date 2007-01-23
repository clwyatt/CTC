/********************************************************
File: buildRube.cc
Abstract: Construnction functions for level set.

See header (buildTube.hh) for documentation
*********************************************************/

#include "buildTube.hh"
#include "RBTree.hh"
#include "MinHeap.hh"


#include <limits.h>
#include <math.h>


using namespace IMTK;

// simple Gaussian Kernel (this function is redundant)
int calcSmoothKernel(float sigma, float * &kernel)
{
	int i, j, k, N, center, index; 
	float x, y, z;
	float a, b; 

	N = 2*((int)ceil(sigma)) + 1;

	center = ((int)ceil(sigma));

	kernel = new float[N*N*N];

	a = 15.74960995*sigma*sigma*sigma;
	b = 2.0*sigma*sigma;

	a = 1/a;
	b = 1/b;

	for(i=0; i < N; i++)
		for(j=0; j < N; j++)
			for(k=0; k < N; k++){
				index = k*N*N + i*N + j;
				x = (float)(i-center);
				y = (float)(j-center);
				z = (float)(k-center);

				kernel[index] = a*exp(-b*(x*x + y*y + z*z));

			}

	return N;

};

// smooth level function only along the tube using Gaussian filter
void smooth(LF3Dfull &levelFunction)
{
	float *innerHolder, *outerHolder;
	int counter;
	LF3Dfull::tubeIterator tubeit;
	float *kernel;
	float value, sum;
	int size, index, center;
	int i, j, k, ii, jj, kk, x, y, z;
       	bool onBorder;

	size = calcSmoothKernel(0.65, kernel);
	center = ((int)ceil(0.65));

	innerHolder = new float[levelFunction.getNumInner()];
	outerHolder = new float[levelFunction.getNumOuter()];

	tubeit = levelFunction.innerBegin();
	counter = 0;
	while(tubeit != levelFunction.innerEnd()){
		i = (*tubeit)[0];
		j = (*tubeit)[1];
		k = (*tubeit)[2];
		sum = 0;
		for(ii=0; ii < size; ii++)
			for(jj=0; jj < size; jj++)
				for(kk=0; kk < size; kk++){
					index = kk*size*size + ii*size + jj;
					x = i+(ii-center);
					y = j+(jj-center);
					z = k+(kk-center);
					value = levelFunction.getVoxel(x, y, z);
					sum = sum + kernel[index]*value;
				}
		innerHolder[counter] = sum;
		counter++;
		tubeit++;
	}
	tubeit = levelFunction.outerBegin();
	counter = 0;
	while(tubeit != levelFunction.outerEnd()){
		i = (*tubeit)[0];
		j = (*tubeit)[1];
		k = (*tubeit)[2];
		sum = 0;
		onBorder = false;
		for(ii=0; ii < size; ii++)
			for(jj=0; jj < size; jj++)
				for(kk=0; kk < size; kk++){
					index = kk*size*size + ii*size + jj;
					x = i+(ii-center);
					y = j+(jj-center);
					z = k+(kk-center);
					value = levelFunction.getVoxel(x, y, z);
 					if(fabs(value) >= FAR) onBorder = true;
					sum = sum + kernel[index]*value;
				}
 		if(!onBorder)
			outerHolder[counter] = sum;
 		else
 			outerHolder[counter] = levelFunction.get(*tubeit);

		counter++;
		tubeit++;
	}

	// update level function
	tubeit = levelFunction.innerBegin();
	counter = 0;
	while(tubeit != levelFunction.innerEnd()){
		levelFunction.set(*tubeit, innerHolder[counter]);
		counter++;
		tubeit++;
	}
	tubeit = levelFunction.outerBegin();
	counter = 0;
	while(tubeit != levelFunction.outerEnd()){
		levelFunction.set(*tubeit, outerHolder[counter]);
		counter++;
		tubeit++;
	}

	delete [] kernel;
	delete [] innerHolder;
	delete [] outerHolder;

};


void init_ellipsoid(LF3Dfull &levelFunction, Vector<float> center, Vector<float> scale, Vector<float> rotation){

// 	slist< IMTK::Subscript<3> > close; // list of initial close points to surface

// 	// check that datasets have the same bounds
// 	assert(levelFunction.getBounds() == data.getBounds());

// 	// intialize the level function to FAR
// 	levelFunction.fill(REAL_FAR);

// 	cerr << "Growing Ellipsoid." << endl;

// 	clog << center[0] << " " <<  center[1] << " " << center[2] << endl;

// 	// grow ellipsoid while initializing the levelset function to -FAR/FAR (or dist to surface if at an intersection)
// 	ellipsiod_grow(levelFunction, close, center, scale, rotation);

// 	cerr << "Marching forward." << endl;

// 	// iterate the fast marching method forward
// 	fast_march_forward(levelFunction, close);

// 	cerr << "Marching backward." << endl;

// 	// iterate the fast marching method backward
// 	fast_march_backward(levelFunction, close);

// 	// smooth the level function
// 	smooth(levelFunction);
};

// linearly interpolate between two isovalues
// if no intersection return false, else return true and fill x0
inline bool interp_plus(float x1, float x2, float y1, float y2, float &x0, float y0){

	x0 = (y0-y1)*(x2-x1)/(y2-y1);

	return ((fabs(x0) <= 1) && (fabs(x0-1) <= 1));
};
inline bool interp_minus(float x1, float x2, float y1, float y2, float &x0, float y0){

	x0 = 1 - (y0-y1)*(x2-x1)/(y2-y1);

	return ((fabs(x0) <= 1) && (fabs(x0-1) <= 1));
};

// if no intersection return false, else return true and set
// distance reference to interpolated distance to isosurface
// see Sethian pgs.133-135
bool get_intersection_inside(int x, int y, int z, float seedIntensity, 
		      float thresh, const Volume<unsigned short> &data, 
		      float &distance){

	int n, pixelx, pixely, pixelz; //neighborhood indecies
	int numberOutside; // number of neighbors outside the isosurface
	float I[7]; // temp storage for pixel values
	float y0; // isovalue to find the intersection at
	float minXdist, SxMinus, SxPlus, minYdist, SyMinus, SyPlus, minZdist, SzMinus, SzPlus; //interpolated axis intersections
	bool xIntersect, xMinusIntersect, xPlusIntersect; // flag for x axis intersections
	bool yIntersect, yMinusIntersect, yPlusIntersect; // flag for y axis intersections
	bool zIntersect, zMinusIntersect, zPlusIntersect; // flag for z axis intersections

	I[0] = (float)(data.getVoxel(x, y, z));
	numberOutside = 0;
	for(n = 0; n < 6; n++){
		pixelx = (n == 0) ? x-1 : x;
		pixelx = (n == 1) ? x+1 : pixelx;
		pixely = (n == 2) ? y-1 : y;
		pixely = (n == 3) ? y+1 : pixely;
		pixelz = (n == 4) ? z-1 : z;
		pixelz = (n == 5) ? z+1 : pixelz;

		I[n+1] = (float)(data.getVoxel(pixelx, pixely, pixelz));
		if(fabs(seedIntensity - I[n+1]) >= thresh) numberOutside += 1;
	}

	// there are no neighbors outside the isosurface
	if(numberOutside == 0){
		return false;
	}


	// decide if intersections exist and if so where they occur
	// there are two cases for seedIntensity+thresh and seedIntensity-thresh
	y0 = ((I[1] > I[0])) ? seedIntensity + thresh : seedIntensity - thresh;
	xMinusIntersect = interp_minus(float(x-1), float(x), I[1], I[0], SxMinus, y0);
	y0 = ((I[2] > I[0])) ? seedIntensity + thresh : seedIntensity - thresh;
	xPlusIntersect = interp_plus(float(x), float(x+1), I[0], I[2], SxPlus, y0);

	y0 = ((I[3] > I[0])) ? seedIntensity + thresh : seedIntensity - thresh;
	yMinusIntersect = interp_minus(float(y-1), float(y), I[3], I[0], SyMinus, y0);
	y0 = ((I[4] > I[0])) ? seedIntensity + thresh : seedIntensity - thresh;
	yPlusIntersect = interp_plus(float(y), float(y+1), I[0], I[4], SyPlus, y0);

	y0 = ((I[5] > I[0])) ? seedIntensity + thresh : seedIntensity - thresh;
	zMinusIntersect = interp_minus(float(z-1), float(z), I[5], I[0], SzMinus, y0);
	y0 = ((I[6] > I[0])) ? seedIntensity + thresh : seedIntensity - thresh;
	zPlusIntersect = interp_plus(float(z), float(z+1), I[0], I[6], SzPlus, y0);

	// compute the closest distance to the isosurface
	if(!xMinusIntersect & !xPlusIntersect){
		xIntersect = false;
	}
	if(xMinusIntersect & !xPlusIntersect){
		minXdist = SxMinus;
		xIntersect = true;
	}
	if(!xMinusIntersect & xPlusIntersect){
		minXdist = SxPlus;
		xIntersect = true;
	}
	if(xMinusIntersect & xPlusIntersect){
		minXdist = SxMinus;
		if(SxPlus < minXdist) minXdist = SxPlus; 
		xIntersect = true;
	}

	if(!yMinusIntersect & !yPlusIntersect){
		yIntersect = false;
	}
	if(yMinusIntersect & !yPlusIntersect){
		minYdist = SyMinus;
		yIntersect = true;
	}
	if(!yMinusIntersect & yPlusIntersect){
		minYdist = SyPlus;
		yIntersect = true;
	}
	if(yMinusIntersect & yPlusIntersect){
		minYdist = SyMinus;
		if(SyPlus < minYdist) minYdist = SyPlus; 
		yIntersect = true;
	}

	if(!zMinusIntersect & !zPlusIntersect){
		zIntersect = false;
	}
	if(zMinusIntersect & !zPlusIntersect){
		minZdist = SzMinus;
		zIntersect = true;
	}
	if(!zMinusIntersect & zPlusIntersect){
		minZdist = SzPlus;
		zIntersect = true;
	}
	if(zMinusIntersect & zPlusIntersect){
		minZdist = SzMinus;
		if(SzPlus < minZdist) minZdist = SzPlus; 
		zIntersect = true;
	}

	// depending on the above cases compute distance
	if(xIntersect & yIntersect & zIntersect){
		distance = 1/sqrt(1/(minXdist*minXdist) + 1/(minYdist*minYdist) + 1/(minZdist*minZdist));
	}
	if(xIntersect & yIntersect & !zIntersect){
		distance = 1/sqrt(1/(minXdist*minXdist) + 1/(minYdist*minYdist));
	}
	if(xIntersect & !yIntersect & zIntersect){
		distance = 1/sqrt(1/(minXdist*minXdist) + 1/(minZdist*minZdist));
	}
	if(xIntersect & !yIntersect & !zIntersect){
		distance = minXdist;
	}
	if(!xIntersect & yIntersect & zIntersect){
		distance = 1/sqrt(1/(minYdist*minYdist) + 1/(minZdist*minZdist));
	}
	if(!xIntersect & yIntersect & !zIntersect){
		distance = minYdist;
	}
	if(!xIntersect & !yIntersect & zIntersect){
		distance = minZdist;
	}
	if(!xIntersect & !yIntersect & !zIntersect){
		//we shouldn't ever get here
		
		assert(0);
	}

	//cerr << "Intersection at " << x << ", " << y << ", " << z << " : " << distance << endl;

	return true;
};

// if no intersection return false, else return true and set
// distance reference to interpolated distance to isosurface
// see Sethian pgs.133-135
bool get_intersection_outside(int x, int y, int z, float seedIntensity, 
		      float thresh, const Volume<unsigned short> &data, 
		      float &distance){

	int n, pixelx, pixely, pixelz; //neighborhood indecies
	int numberInside; // number of neighbors inside the isosurface
	float I[7]; // temp storage for pixel values
	float y0; // isovalue to find the intersection at
	float minXdist, SxMinus, SxPlus, minYdist, SyMinus, SyPlus, minZdist, SzMinus, SzPlus; //interpolated axis intersections
	bool xIntersect, xMinusIntersect, xPlusIntersect; // flag for x axis intersections
	bool yIntersect, yMinusIntersect, yPlusIntersect; // flag for y axis intersections
	bool zIntersect, zMinusIntersect, zPlusIntersect; // flag for z axis intersections

	I[0] = (float)(data.getVoxel(x, y, z));
	numberInside = 0;
	for(n = 0; n < 6; n++){
		pixelx = (n == 0) ? x-1 : x;
		pixelx = (n == 1) ? x+1 : pixelx;
		pixely = (n == 2) ? y-1 : y;
		pixely = (n == 3) ? y+1 : pixely;
		pixelz = (n == 4) ? z-1 : z;
		pixelz = (n == 5) ? z+1 : pixelz;

		I[n+1] = (float)(data.getVoxel(pixelx, pixely, pixelz));
		if(fabs(seedIntensity - I[n+1]) < thresh) numberInside += 1;
	}

	// there are no neighbors outside the isosurface
	if(numberInside == 0){
		return false;
	}


	// decide if intersections exist and if so where they occur
	// there are two cases for seedIntensity+thresh and seedIntensity-thresh
	y0 = ((I[1] < I[0])) ? seedIntensity + thresh : seedIntensity - thresh;
	xMinusIntersect = interp_minus(float(x-1), float(x), I[1], I[0], SxMinus, y0);
	y0 = ((I[2] < I[0])) ? seedIntensity + thresh : seedIntensity - thresh;
	xPlusIntersect = interp_plus(float(x), float(x+1), I[0], I[2], SxPlus, y0);

	y0 = ((I[3] < I[0])) ? seedIntensity + thresh : seedIntensity - thresh;
	yMinusIntersect = interp_minus(float(y-1), float(y), I[3], I[0], SyMinus, y0);
	y0 = ((I[4] < I[0])) ? seedIntensity + thresh : seedIntensity - thresh;
	yPlusIntersect = interp_plus(float(y), float(y+1), I[0], I[4], SyPlus, y0);

	y0 = ((I[5] < I[0])) ? seedIntensity + thresh : seedIntensity - thresh;
	zMinusIntersect = interp_minus(float(z-1), float(z), I[5], I[0], SzMinus, y0);
	y0 = ((I[6] < I[0])) ? seedIntensity + thresh : seedIntensity - thresh;
	zPlusIntersect = interp_plus(float(z), float(z+1), I[0], I[6], SzPlus, y0);

	// compute the closest distance to the isosurface
	if(!xMinusIntersect & !xPlusIntersect){
		xIntersect = false;
	}
	if(xMinusIntersect & !xPlusIntersect){
		minXdist = SxMinus;
		xIntersect = true;
	}
	if(!xMinusIntersect & xPlusIntersect){
		minXdist = SxPlus;
		xIntersect = true;
	}
	if(xMinusIntersect & xPlusIntersect){
		minXdist = SxMinus;
		if(SxPlus < minXdist) minXdist = SxPlus; 
		xIntersect = true;
	}

	if(!yMinusIntersect & !yPlusIntersect){
		yIntersect = false;
	}
	if(yMinusIntersect & !yPlusIntersect){
		minYdist = SyMinus;
		yIntersect = true;
	}
	if(!yMinusIntersect & yPlusIntersect){
		minYdist = SyPlus;
		yIntersect = true;
	}
	if(yMinusIntersect & yPlusIntersect){
		minYdist = SyMinus;
		if(SyPlus < minYdist) minYdist = SyPlus; 
		yIntersect = true;
	}

	if(!zMinusIntersect & !zPlusIntersect){
		zIntersect = false;
	}
	if(zMinusIntersect & !zPlusIntersect){
		minZdist = SzMinus;
		zIntersect = true;
	}
	if(!zMinusIntersect & zPlusIntersect){
		minZdist = SzPlus;
		zIntersect = true;
	}
	if(zMinusIntersect & zPlusIntersect){
		minZdist = SzMinus;
		if(SzPlus < minZdist) minZdist = SzPlus; 
		zIntersect = true;
	}

	// depending on the above cases compute distance
	if(xIntersect & yIntersect & zIntersect){
		distance = 1/sqrt(1/(minXdist*minXdist) + 1/(minYdist*minYdist) + 1/(minZdist*minZdist));
	}
	if(xIntersect & yIntersect & !zIntersect){
		distance = 1/sqrt(1/(minXdist*minXdist) + 1/(minYdist*minYdist));
	}
	if(xIntersect & !yIntersect & zIntersect){
		distance = 1/sqrt(1/(minXdist*minXdist) + 1/(minZdist*minZdist));
	}
	if(xIntersect & !yIntersect & !zIntersect){
		distance = minXdist;
	}
	if(!xIntersect & yIntersect & zIntersect){
		distance = 1/sqrt(1/(minYdist*minYdist) + 1/(minZdist*minZdist));
	}
	if(!xIntersect & yIntersect & !zIntersect){
		distance = minYdist;
	}
	if(!xIntersect & !yIntersect & zIntersect){
		distance = minZdist;
	}
	if(!xIntersect & !yIntersect & !zIntersect){
		//we shouldn't ever get here
		
		assert(0);
	}

	//cerr << "Intersection at " << x << ", " << y << ", " << z << " : " << distance << endl;

	return true;
};



// grow region using connected region growing
// in voxel in the interior set level function to -FAR 
// if the voxel falls at the isosurface intersection, then use interpolation to get a distance to the surface
// set the level function to that distance.
// NOTE: this function assumes that data point is NOT exactly equal to the threshold.
void region_grow(LF3Dfull &levelFunction, slist< IMTK::Subscript<3> > &close, Subscript<3> seed, float thresh, const Volume<unsigned short> &data){

	int x, y, z;
	int n, pixelx, pixely, pixelz, found;
	double key;
	float seedIntensity, intensityDifference1, intensityDifference2, distance;
	RBTree< Subscript<3> > stack;
	Subscript<3> bounds, tempP, stack_top;
	bool out;

	x= seed[0];
	y= seed[1];
	z= seed[2];
	seedIntensity = (float)(data.getVoxel(x, y, z));

	cerr << "Seed (" << x << ", " << y << ", " << z << ") Intensity = " << seedIntensity << endl;


	levelFunction.setVoxel(x,y,z,-FAR);
	tempP[0] = x;
	tempP[1] = y;
	tempP[2] = z;

	bounds = data.getBounds();
	key = (double)(tempP.getIndex(bounds));
	stack.insertNode(tempP, key);
    
	while(!stack.isEmpty()){

		out = false;
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
				// calculate the intensity difference from the seed
				intensityDifference1 = fabs((float)(data.getVoxel(pixelx, pixely, pixelz)) - seedIntensity);
				intensityDifference2 = fabs((float)(data.getVoxel(x, y, z)) - seedIntensity);

				// if intensity difference OR neighbor intensity difference is within threshold, push onto stack
				if( (intensityDifference1 < thresh) || (intensityDifference2 < thresh)){ 
					// check if this voxel has been examined or is already on stack
					if(levelFunction.getVoxel(pixelx, pixely, pixelz) == REAL_FAR){
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
			else{
				out = true;
			}
		}
		
		// if a neighbor was out of bounds, set the level function to zero
		// and add to inner tube list
		if(out){
			levelFunction.setVoxel(x, y, z, 0.0);
			tempP[0] = x; tempP[1] = y; tempP[2] = z;
			close.push_front(tempP);
			//levelFunction.addInner(x, y, z);
		}

		stack_top = stack.getRoot();
		x = stack_top[0];
		y = stack_top[1];
		z = stack_top[2];
		
		// switch on inside/outside of surface
		intensityDifference1 = fabs((float)(data.getVoxel(x, y, z)) - seedIntensity);
		if(intensityDifference1 < thresh){ //inside
			// if voxel intersects the isosurface, set the level function to minus
			// the linearly interpolated distance to the isosurface.
			if(get_intersection_inside(x, y, z, seedIntensity, thresh, data, distance)){
				levelFunction.setVoxel(x, y, z, -distance);
				tempP[0] = x; tempP[1] = y; tempP[2] = z;
				close.push_front(tempP);
				//levelFunction.addInner(x, y, z);
			}
			else{ // else set to -FAR
				
				levelFunction.setVoxel(x, y, z, -FAR);
			}
		}
		else{ //outside
			// if voxel intersects the isosurface, set the level function to
			// the linearly interpolated distance to the isosurface.
			if(get_intersection_outside(x, y, z, seedIntensity, thresh, data, distance)){
				levelFunction.setVoxel(x, y, z, distance);
				tempP[0] = x; tempP[1] = y; tempP[2] = z;
				close.push_front(tempP);
				//levelFunction.addInner(x, y, z);
			}
			else{ // else set to FAR
				levelFunction.setVoxel(x, y, z, FAR);
			}
		}

		key = (double)(stack_top.getIndex(bounds));
		stack.deleteNode(key);
	}


};


// grow region using connected region growing
// in voxel in the interior set level function to -FAR 
// if the voxel falls at the isosurface intersection, then use interpolation to get a distance to the surface
// set the level function to that distance.
// NOTE: this function assumes that data point is NOT exactly equal to the threshold.
void region_grow_dist(LF3Dfull &levelFunction, slist< IMTK::Subscript<3> > &close, Subscript<3> seed, float thresh, const Volume<unsigned short> &data){

	int x, y, z;
	int n, pixelx, pixely, pixelz, found;
	double key;
	float seedIntensity, intensityDifference1, intensityDifference2, distance;
	RBTree< Subscript<3> > stack;
	Subscript<3> bounds, tempP, stack_top;
	bool out;

	x= seed[0];
	y= seed[1];
	z= seed[2];
	seedIntensity = (float)(data.getVoxel(x, y, z));

	cerr << "Seed (" << x << ", " << y << ", " << z << ") Intensity = " << seedIntensity << endl;

	// adjust threshold so that it is absolute from zero
	// remember seedIntensity is a local max for this function
	thresh = seedIntensity - thresh;

	levelFunction.setVoxel(x,y,z,-FAR);
	tempP[0] = x;
	tempP[1] = y;
	tempP[2] = z;

	bounds = data.getBounds();
	key = (double)(tempP.getIndex(bounds));
	stack.insertNode(tempP, key);
    
	while(!stack.isEmpty()){

		out = false;
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
				// calculate the intensity difference from the seed
				intensityDifference1 = fabs((float)(data.getVoxel(pixelx, pixely, pixelz)) - seedIntensity);
				intensityDifference2 = fabs((float)(data.getVoxel(x, y, z)) - seedIntensity);

				// if intensity difference OR neighbor intensity difference is within threshold, push onto stack
				if( (intensityDifference1 < thresh) || (intensityDifference2 < thresh)){ 
					// check if this voxel has been examined or is already on stack
					if(levelFunction.getVoxel(pixelx, pixely, pixelz) == REAL_FAR){
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
			else{
				out = true;
			}
		}
		
		// if a neighbor was out of bounds, set the level function to zero
		// and add to inner tube list
		if(out){
			levelFunction.setVoxel(x, y, z, 0.0);
			tempP[0] = x; tempP[1] = y; tempP[2] = z;
			close.push_front(tempP);
			//levelFunction.addInner(x, y, z);
		}

		stack_top = stack.getRoot();
		x = stack_top[0];
		y = stack_top[1];
		z = stack_top[2];
		
		// switch on inside/outside of surface
		intensityDifference1 = fabs((float)(data.getVoxel(x, y, z)) - seedIntensity);
		if(intensityDifference1 < thresh){ //inside
			// if voxel intersects the isosurface, set the level function to minus
			// the linearly interpolated distance to the isosurface.
			if(get_intersection_inside(x, y, z, seedIntensity, thresh, data, distance)){
				levelFunction.setVoxel(x, y, z, -distance);
				tempP[0] = x; tempP[1] = y; tempP[2] = z;
				close.push_front(tempP);
				//levelFunction.addInner(x, y, z);
			}
			else{ // else set to -FAR
				
				levelFunction.setVoxel(x, y, z, -FAR);
			}
		}
		else{ //outside
			// if voxel intersects the isosurface, set the level function to
			// the linearly interpolated distance to the isosurface.
			if(get_intersection_outside(x, y, z, seedIntensity, thresh, data, distance)){
				levelFunction.setVoxel(x, y, z, distance);
				tempP[0] = x; tempP[1] = y; tempP[2] = z;
				close.push_front(tempP);
				//levelFunction.addInner(x, y, z);
			}
			else{ // else set to FAR
				levelFunction.setVoxel(x, y, z, FAR);
			}
		}

		key = (double)(stack_top.getIndex(bounds));
		stack.deleteNode(key);
	}


};


// function to update the trial point using equation (8.2)
// in Sethian's book. 
void updateTrialForward(LF3Dfull &levelFunction, const Subscript<3> &neighbor){

	float tx[2]; // values at x-1,y,z and x+1,y,z
	float ty[2]; // values at x,y-1,z and x,y+1,z
	float tz[2]; // values at x,y,z-1 and x,y,z+1
	int i, j, k;
	bool txflag, tyflag, tzflag, solutionFound;
	float a, b, c; // coefficients of the quadratic equation (a*T^2 + b*T + c = 0)
	float sqrtterm; // temp to hold sqrt term of quadratic equation
	float s1, s2; // two solutions to the quadratic equation
	float solution; // the final maximum solution to the equation

	//cout << "Updating: " << neighbor[0] << "," << neighbor[1] << "," << neighbor[2] << " Index " << neighbor.getIndex(levelFunction.getBounds()) << endl;
	//cout << "Was: " << levelFunction.getVoxel(neighbor[0], neighbor[1], neighbor[2]) << endl;

	tx[0] = levelFunction.getVoxel(neighbor[0]-1, neighbor[1], neighbor[2]);
	tx[1] = levelFunction.getVoxel(neighbor[0]+1, neighbor[1], neighbor[2]);
	ty[0] = levelFunction.getVoxel(neighbor[0], neighbor[1]-1, neighbor[2]);
	ty[1] = levelFunction.getVoxel(neighbor[0], neighbor[1]+1, neighbor[2]);
	tz[0] = levelFunction.getVoxel(neighbor[0], neighbor[1], neighbor[2]-1);
	tz[1] = levelFunction.getVoxel(neighbor[0], neighbor[1], neighbor[2]+1);

	solutionFound = false;
	// iterate through all 27 cases
	// i, j, k count which result of the max term is used
	for(i=0; i < 3; i++)
		for(j=0; j < 3; j++)
			for(k=0; k < 3; k++){

				// determine coefficients
				a = 0;
				b = 0;
				c = 0;
				if(i < 2){
					a = a + 1;
					b = b + tx[i];
					c = c + tx[i]*tx[i];
				}
				if(j < 2){
					a = a + 1;
					b = b + ty[j];
					c = c + ty[j]*ty[j];
				}
				if(k < 2){
					a = a + 1;
					b = b + tz[k];
					c = c + tz[k]*tz[k];
				}
				b = -2.0*b;
				c = c - 1;

				sqrtterm = b*b - 4.0*a*c;
				
				// if no real solution, skip rest
				if(sqrtterm < 0) continue;

				s1 = (-b + sqrt(sqrtterm))/(2*a);
				s2 = (-b - sqrt(sqrtterm))/(2*a);
				
				if(s1 < s2) s1 = s2; //make s1 the larger

				// check constraints, if not met, skip rest

				// first constraint
				txflag = ( (i == 0) && ((s1 - tx[0]) >= (s1 - tx[1])) ) ||
					( (i == 1) && ((s1 - tx[1]) >= (s1 - tx[0])) ) ||
					( (i == 2) && ((s1 - tx[0]) <= 0) && ((s1 - tx[1]) <= 0) );
				// second constraint
				tyflag = ( (j == 0) && ((s1 - ty[0]) >= (s1 - ty[1])) ) ||
					( (j == 1) && ((s1 - ty[1]) >= (s1 - ty[0])) ) ||
					( (j == 2) && ((s1 - ty[0]) <= 0) && ((s1 - ty[1]) <= 0) );
				// third constraint
				tzflag = ( (k == 0) && ((s1 - tz[0]) >= (s1 - tz[1])) ) ||
					( (k == 1) && ((s1 - tz[1]) >= (s1 - tz[0])) ) ||
					( (k == 2) && ((s1 - tz[0]) <= 0) && ((s1 - tz[1]) <= 0) );


				if(!(txflag && tyflag && tzflag)) continue;


				//cerr << "Solution: " << s1 << endl;
				//cout << "max(" << s1 - tx[0] << ", " << s1 - tx[1] << ", 0) is " << i  << endl;
				//cout << "max(" << s1 - ty[0] << ", " << s1 - ty[1] << ", 0) is " << j  << endl;
				//cout << "max(" << s1 - tz[0] << ", " << s1 - tz[1] << ", 0) is " << k  << endl;

				if(!solutionFound){ //first possible solution
					solutionFound = true;
					solution = s1;
				}
				else{ // largest possible solution
					if(s1 > solution) solution = s1;
				}
				
			}

	if(solutionFound){
		levelFunction.set(neighbor, solution);
		//cout << "Now: " << levelFunction.get(neighbor) << endl;
	}
	else{
		cerr << "Warning: no solution found in updateTrialForward" << endl;
		cout << "Updating: " << neighbor[0] << "," << neighbor[1] << "," << neighbor[2] << " Index " << neighbor.getIndex(levelFunction.getBounds()) << endl;
		cout << "Was: " << levelFunction.getVoxel(neighbor[0], neighbor[1], neighbor[2]) << endl;
		exit(-1);
	}
	
};

// backward version of the update
void updateTrialBackward(LF3Dfull &levelFunction, const Subscript<3> &neighbor){

	float tx[2]; // values at x-1,y,z and x+1,y,z
	float ty[2]; // values at x,y-1,z and x,y+1,z
	float tz[2]; // values at x,y,z-1 and x,y,z+1
	int i, j, k;
	bool txflag, tyflag, tzflag, solutionFound;
	float a, b, c; // coefficients of the quadratic equation (a*T^2 + b*T + c = 0)
	float sqrtterm; // temp to hold sqrt term of quadratic equation
	float s1, s2; // two solutions to the quadratic equation
	float solution; // the final maximum solution to the equation

	//cout << "Updating: " << neighbor[0] << "," << neighbor[1] << "," << neighbor[2] << " Index " << neighbor.getIndex(levelFunction.getBounds()) << endl;
	//cout << "Was: " << levelFunction.get(neighbor) << endl;

	tx[0] = levelFunction.getVoxel(neighbor[0]-1, neighbor[1], neighbor[2]);
	tx[1] = levelFunction.getVoxel(neighbor[0]+1, neighbor[1], neighbor[2]);
	ty[0] = levelFunction.getVoxel(neighbor[0], neighbor[1]-1, neighbor[2]);
	ty[1] = levelFunction.getVoxel(neighbor[0], neighbor[1]+1, neighbor[2]);
	tz[0] = levelFunction.getVoxel(neighbor[0], neighbor[1], neighbor[2]-1);
	tz[1] = levelFunction.getVoxel(neighbor[0], neighbor[1], neighbor[2]+1);


	solutionFound = false;
	// iterate through all 27 cases
	// i, j, k count which result of the min term is used
	for(i=0; i < 3; i++)
		for(j=0; j < 3; j++)
			for(k=0; k < 3; k++){

				// determine coefficients
				a = 0;
				b = 0;
				c = 0;
				if(i < 2){
					a = a + 1;
					b = b + tx[i];
					c = c + tx[i]*tx[i];
				}
				if(j < 2){
					a = a + 1;
					b = b + ty[j];
					c = c + ty[j]*ty[j];
				}
				if(k < 2){
					a = a + 1;
					b = b + tz[k];
					c = c + tz[k]*tz[k];
				}
				b = -2.0*b;
				c = c - 1;

				sqrtterm = b*b - 4.0*a*c;
				
				// if no real solution, skip rest
				if(sqrtterm < 0) continue;

				s1 = (-b + sqrt(sqrtterm))/(2*a);
				s2 = (-b - sqrt(sqrtterm))/(2*a);
				
				if(s1 > s2) s1 = s2; //make s1 the smaller

				// check constraints, if not met, skip rest

				// first constraint
				txflag = ( (i == 0) && ((s1 - tx[0]) <= (s1 - tx[1])) ) ||
					( (i == 1) && ((s1 - tx[1]) <= (s1 - tx[0])) ) ||
					( (i == 2) && ((s1 - tx[0]) >= 0) && ((s1 - tx[1]) >= 0) );
				// second constraint
				tyflag = ( (j == 0) && ((s1 - ty[0]) <= (s1 - ty[1])) ) ||
					( (j == 1) && ((s1 - ty[1]) <= (s1 - ty[0])) ) ||
					( (j == 2) && ((s1 - ty[0]) >= 0) && ((s1 - ty[1]) >= 0) );
				// third constraint
				tzflag = ( (k == 0) && ((s1 - tz[0]) <= (s1 - tz[1])) ) ||
					( (k == 1) && ((s1 - tz[1]) <= (s1 - tz[0])) ) ||
					( (k == 2) && ((s1 - tz[0]) >= 0) && ((s1 - tz[1]) >= 0) );


				if(!(txflag && tyflag && tzflag)) continue;


				//cerr << "Solution: " << s1 << endl;
				//cout << "max(" << s1 - tx[0] << ", " << s1 - tx[1] << ", 0) is " << i  << endl;
				//cout << "max(" << s1 - ty[0] << ", " << s1 - ty[1] << ", 0) is " << j  << endl;
				//cout << "max(" << s1 - tz[0] << ", " << s1 - tz[1] << ", 0) is " << k  << endl;

				if(!solutionFound){ //first possible solution
					solutionFound = true;
					solution = s1;
				}
				else{ // smallest possible solution
					if(s1 < solution) solution = s1;
				}
				
			}

	if(solutionFound){
		levelFunction.set(neighbor, solution);
		//cout << "Now: " << levelFunction.get(neighbor) << endl;
	}
	else{
		cerr << "Warning: no solution found in updateTrialBackward" << endl;
		exit(-1);
	}
	
};



// **************************************** Functions used to operate on a simple min and max heap *********************************
#define HEAP_SIZE 1000000

// Function to insert into min Heap
void insertMinHeap(unsigned int *heap, unsigned int &heapSize, LF3Dfull &levelFunction, Volume<unsigned int> &heapIndex, unsigned int index){

	unsigned int i, j;
	
	heapSize += 1;
	assert(heapSize < HEAP_SIZE);
	
	i = heapSize;
	j = i >> 1;
	
	// find the proper position for insertion
	while( (j >= 1) && (levelFunction[heap[j]] > levelFunction[index]) ){
		
		heap[i] = heap[j]; // move parent down one position
		heapIndex[heap[i]] = i;
		i = j;
		j >>= 1;
	}
	
	// copy to location found above
	heap[i] = index;
	heapIndex[index] = i;
}


// Function to adjust the min Heap up
void upMinHeap(unsigned int *heap, unsigned int &heapSize, LF3Dfull &levelFunction, Volume<unsigned int> &heapIndex, unsigned int index){
	
	unsigned int i, j;
	bool found;
	unsigned int temp;
	
	i = index;
	temp = heap[i]; // temp holder for node to be moved up through the tree
	found = false;	
	j = i;
	
	//clog << " Moving " << levelFunction[heap[index]] << " up in heap." << endl;

	while( (j > 1) && (!found) ){
		if(levelFunction[temp] > levelFunction[heap[j >> 1]]){
			found = true;
		}
		else{
			// move parent down
			heap[j] = heap[j >> 1];
			heapIndex[heap[j]] = j;
			
			// move to next higher position in tree
			j >>= 1;
		}
	}
	
	// place the node
	heap[j] = temp;
	heapIndex[temp] = j;
	
};

// Function to adjust the min Heap down
void downMinHeap(unsigned int *heap, unsigned int &heapSize, LF3Dfull &levelFunction, Volume<unsigned int> &heapIndex, unsigned int index){
	
	unsigned int i, j;
	bool found;
	unsigned int temp;
	
	i = index;
	temp = heap[i]; // temp holder for node to be dropped through the tree
	found = false;	
	j = i << 1;
	
	//clog << " Moving " << levelFunction[heap[index]] << " down in heap." << endl;

	while( (j <= heapSize) && (!found) ){
		// move to right child depending on comparision
		if( (j < heapSize)  && (levelFunction[heap[j]] > levelFunction[heap[j+1]]) ){
			j = j + 1;
		}
		
		if(levelFunction[temp] < levelFunction[heap[j]]){
			found = true;
		}
		else{
			// move child up
			heap[j >> 1] = heap[j];
			heapIndex[heap[j >> 1]] = j >> 1;
			
			// move to next position
			j <<= 1;
			}
	}
	
	// place the node
	heap[j >> 1] = temp;
	heapIndex[temp] = j >> 1;
}


// Function to remove min Heap root and readjust heap
void removeMinHeapRoot(unsigned int *heap, unsigned int &heapSize, LF3Dfull &levelFunction, Volume<unsigned int> &heapIndex){

	assert(heapSize > 0);
	
	heap[1] = heap[heapSize];
	heapIndex[heap[1]] = 1;

	heapSize -= 1;
	
	downMinHeap(heap, heapSize, levelFunction, heapIndex, 1);
}

// Function to insert into max Heap
void insertMaxHeap(unsigned int *heap, unsigned int &heapSize, LF3Dfull &levelFunction, Volume<unsigned int> &heapIndex, unsigned int index){

	unsigned int i, j;
	
	heapSize += 1;
	assert(heapSize <= HEAP_SIZE);
	
	i = heapSize;
	j = i >> 1;
	
	// find the proper position for insertion
	while( (j >= 1) && (levelFunction[heap[j]] < levelFunction[index]) ){
		
		heap[i] = heap[j]; // move parent down one position
		heapIndex[heap[i]] = i;
		i = j;
		j >>= 1;
	}
	
	// copy to location found above
	heap[i] = index;
	heapIndex[index] = i;
}

// Function to adjust the max Heap up
void upMaxHeap(unsigned int *heap, unsigned int &heapSize, LF3Dfull &levelFunction, Volume<unsigned int> &heapIndex, unsigned int index){
	
	unsigned int i, j;
	bool found;
	unsigned int temp;
	
	i = index;
	temp = heap[i]; // temp holder for node to be moved up through the tree
	found = false;	
	j = i;

	while( (j > 1) && (!found) ){
		if(levelFunction[temp] < levelFunction[heap[j >> 1]]){
			found = true;
		}
		else{
			// move parent down
			heap[j] = heap[j >> 1];
			heapIndex[heap[j]] = j;
			
			// move to next higher position in tree
			j >>= 1;
		}
	}
	
	// place the node
	heap[j] = temp;
	heapIndex[temp] = j;
	
};

// Function to adjust the max Heap down
void downMaxHeap(unsigned int *heap, unsigned int &heapSize, LF3Dfull &levelFunction, Volume<unsigned int> &heapIndex, unsigned int index){
	
	unsigned int i, j;
	bool found;
	unsigned int temp;
	
	i = index;
	temp = heap[i]; // temp holder for node to be dropped through the tree
	found = false;	
	j = i << 1;
	
	while( (j <= heapSize) && (!found) ){
		// move to right child depending on comparision
		if( (j < heapSize)  && (levelFunction[heap[j]] < levelFunction[heap[j+1]]) ){
			j = j + 1;
		}
		
		if(levelFunction[temp] > levelFunction[heap[j]]){
			found = true;
		}
		else{
				// move child up
			heap[j >> 1] = heap[j];
			heapIndex[heap[j >> 1]] = j >> 1;
			
				// move to next position
			j <<= 1;
			}
	}
	
	// place the node
	heap[j >> 1] = temp;
	heapIndex[temp] = j >> 1;
}


// Function to remove max Heap root and readjust heap
void removeMaxHeapRoot(unsigned int *heap, unsigned int &heapSize, LF3Dfull &levelFunction, Volume<unsigned int> &heapIndex){

	assert(heapSize > 0);
	
	heap[1] = heap[heapSize];
	heapIndex[heap[1]] = 1;

	heapSize -= 1;
	
	downMaxHeap(heap, heapSize, levelFunction, heapIndex, 1);
}

// ****************************************************************************************************************


// function to iterate the fast marching method until the tube diameter
// is exceeded. The set of trail points is stored in a heap.
void fast_march_forward(LF3Dfull &levelFunction, slist< IMTK::Subscript<3> > &close){

	unsigned int count, index, n, x, y, z, nx, ny, nz;
	float T = 1.0;
	float innerR, outerR, oldT, newT;
	Subscript<3> bounds, location, neighbor;

	unsigned int trialHeap[HEAP_SIZE]; // simple heap for getting smallest T value
	unsigned int currentHeapSize = 0;

	Volume<unsigned int> heapIndex;

	bounds = levelFunction.getBounds();

	heapIndex.setBounds(bounds);

	innerR = levelFunction.getInnerR();
	outerR = levelFunction.getOuterR();
	assert(outerR > innerR);

	// a heap index of 0 indicates its not in the heap
	heapIndex.fill(0);

	//**************  initialize the heap *****************
	slist< IMTK::Subscript<3> >::iterator it = close.begin();
	while(it != levelFunction.innerEnd()){
		// add initial points to heap
		if(levelFunction.get(*it) > 0){
			insertMinHeap(trialHeap, currentHeapSize, levelFunction, heapIndex, (*it).getIndex(bounds));
		}
		it++;
	}

	//************** loop until reach the tube size **************

	// get smallest from heap
	index = trialHeap[1];
	removeMinHeapRoot(trialHeap, currentHeapSize, levelFunction, heapIndex);
	location.setSubscript(bounds, index);
	heapIndex.set(location, 0); // no longer in heap
	levelFunction.addInner(location[0], location[1], location[2]);

	count = 1;

	while( (levelFunction.get(location) < outerR) && (currentHeapSize > 0)){

		// loop through the neighbors
		for(n = 0; n < 6; n++){
			neighbor[0] = (n == 0) ? location[0]-1 : location[0];
			neighbor[0] = (n == 1) ? location[0]+1 : neighbor[0];
			neighbor[1] = (n == 2) ? location[1]-1 : location[1];
			neighbor[1] = (n == 3) ? location[1]+1 : neighbor[1];
			neighbor[2] = (n == 4) ? location[2]-1 : location[2];
			neighbor[2] = (n == 5) ? location[2]+1 : neighbor[2];
			
			// if on inside or not in the interior of the bounding box, continue
			if( (neighbor[0] < 1) || (neighbor[0] > bounds[0]-2) ||
			    (neighbor[1] < 1) || (neighbor[1] > bounds[1]-2) ||
			    (neighbor[2] < 1) || (neighbor[2] > bounds[2]-2) ){
				continue;
			}
			if(levelFunction.get(neighbor) < 0) continue;

			// if in FAR, add to heap
			if(levelFunction.get(neighbor) >= FAR){
				insertMinHeap(trialHeap, currentHeapSize, levelFunction, heapIndex, neighbor.getIndex(bounds));
			}

			// if not in KNOWN (there is a valid heap pointer)
			// update distance
			index = heapIndex.get(neighbor);
			if(index > 0){

				//clog << "Updating " << levelFunction.get(neighbor) << endl;
				oldT = levelFunction.get(neighbor);

				updateTrialForward(levelFunction, neighbor);

				//clog << " To " << levelFunction.get(neighbor) << endl;
				newT = levelFunction.get(neighbor);

				// adjust the heap
				if(newT > oldT) downMinHeap(trialHeap, currentHeapSize, levelFunction, heapIndex, index);
				else if(newT < oldT) upMinHeap(trialHeap, currentHeapSize, levelFunction, heapIndex, index);

			}
		}

		// get smallest from heap
		index = trialHeap[1];
		removeMinHeapRoot(trialHeap, currentHeapSize, levelFunction, heapIndex);
		location.setSubscript(bounds, index);
		heapIndex.set(location, 0); // no longer in heap

		if(levelFunction.get(location) < innerR){
			levelFunction.addInner(location[0], location[1], location[2]);
		}
		else if(levelFunction.get(location) < outerR){
			levelFunction.addOuter(location[0], location[1], location[2]);
		}

		count += 1;
	}

	clog << count << " tube points in forward march." << endl; 

};

// function to iterate the fast marching method until the tube diameter
// is exceeded. The set of trail points is stored in a heap.
void fast_march_backward(LF3Dfull &levelFunction, slist< IMTK::Subscript<3> > &close){

	unsigned int count, index, n, x, y, z, nx, ny, nz;
	float T = -1.0;
	float innerR, outerR, oldT, newT;
	Subscript<3> bounds, location, neighbor;

	unsigned int trialHeap[HEAP_SIZE]; // simple heap for getting smallest T value
	unsigned int currentHeapSize = 0;

	Volume<unsigned int> heapIndex;

	bounds = levelFunction.getBounds();

	heapIndex.setBounds(bounds);

	innerR = levelFunction.getInnerR();
	outerR = levelFunction.getOuterR();
	assert(outerR > innerR);

	// a heap index of 0 indicates its not in the heap
	heapIndex.fill(0);

	//**************  initialize the heap *****************
	slist< IMTK::Subscript<3> >::iterator it = close.begin();
	while(it != levelFunction.innerEnd()){
		// add initial points to heap
		if(levelFunction.get(*it) < 0){
			insertMaxHeap(trialHeap, currentHeapSize, levelFunction, heapIndex, (*it).getIndex(bounds));
		}
		it++;
	}

	//************** loop until reach the tube size **************

	// get largest from heap
	index = trialHeap[1];
	removeMaxHeapRoot(trialHeap, currentHeapSize, levelFunction, heapIndex);
	location.setSubscript(bounds, index);
	heapIndex.set(location, 0); // no longer in heap
	levelFunction.addInner(location[0], location[1], location[2]);

	count = 1;

	while( (levelFunction.get(location) > -outerR) && (currentHeapSize > 0) ){

		
		// loop through the neighbors
		for(n = 0; n < 6; n++){
			neighbor[0] = (n == 0) ? location[0]-1 : location[0];
			neighbor[0] = (n == 1) ? location[0]+1 : neighbor[0];
			neighbor[1] = (n == 2) ? location[1]-1 : location[1];
			neighbor[1] = (n == 3) ? location[1]+1 : neighbor[1];
			neighbor[2] = (n == 4) ? location[2]-1 : location[2];
			neighbor[2] = (n == 5) ? location[2]+1 : neighbor[2];
			
			// if in FAR, add to heap
			if(levelFunction.get(neighbor) <= -FAR){
				insertMaxHeap(trialHeap, currentHeapSize, levelFunction, heapIndex, neighbor.getIndex(bounds));

			}
			// if not in KNOWN (there is a valid heap pointer)
			// update distance
			index = heapIndex.get(neighbor);
			if(index > 0){

				oldT = levelFunction.get(neighbor);

				updateTrialBackward(levelFunction, neighbor);

				newT = levelFunction.get(neighbor);

				// adjust the heap
				if(newT < oldT) downMaxHeap(trialHeap, currentHeapSize, levelFunction, heapIndex, index);
				else if(newT > oldT) upMaxHeap(trialHeap, currentHeapSize, levelFunction, heapIndex, index);

			}
		}

		// get largest from heap
		index = trialHeap[1];
		removeMaxHeapRoot(trialHeap, currentHeapSize, levelFunction, heapIndex);
		location.setSubscript(bounds, index);
		heapIndex.set(location, 0); // no longer in heap

		//cerr << "Largest = " << levelFunction.get(location) << endl;
		//cout << "Press <return> to coninue" << endl;
		//getc(stdin);

		if(levelFunction.get(location) > -innerR){
			levelFunction.addInner(location[0], location[1], location[2]);
		}
		else if(levelFunction.get(location) > -outerR){
			levelFunction.addOuter(location[0], location[1], location[2]);
		}

		count += 1;
	}

	clog << count << " tube points in backward march." << endl;
};




void init_region(LF3Dfull &levelFunction, list< Subscript<3> > &seeds, float thresh, const Volume<unsigned short> &data){

	slist< IMTK::Subscript<3> > close; // list of initial close points to surface

	// check that datasets have the same bounds
	assert(levelFunction.getBounds() == data.getBounds());

	// intialize the level function to FAR
	levelFunction.fill(REAL_FAR);

	cerr << "Growing Region(s)." << endl;

	list< Subscript<3> >::iterator it = seeds.begin();

	while(it != seeds.end() ){

		clog << (*it)[0] << " " <<  (*it)[1] << " " << (*it)[2] << endl;

		// grow region while initializing the levelset function to -FAR/FAR (or dist to isosurface if at an intersection)
		region_grow(levelFunction, close, *it, thresh, data);

		cerr << "Marching forward." << endl;

		// iterate the fast marching method forward
		fast_march_forward(levelFunction, close);

		cerr << "Marching backward." << endl;

		// iterate the fast marching method backward
		fast_march_backward(levelFunction, close);

		it++;
	}

	//levelFunction.dump("original.raw");
	// smooth the level function
	//smooth(levelFunction);
	//smooth(levelFunction);
	//levelFunction.dump("smoothed.raw");
};

void init_region_dist(LF3Dfull &levelFunction, list< Subscript<3> > &seeds, float thresh, const Volume<unsigned short> &data){

	slist< IMTK::Subscript<3> > close; // list of initial close points to surface

	// check that datasets have the same bounds
	assert(levelFunction.getBounds() == data.getBounds());

	// intialize the level function to FAR
	levelFunction.fill(REAL_FAR);

	cerr << "Growing Region(s)." << endl;

	list< Subscript<3> >::iterator it = seeds.begin();

	while(it != seeds.end() ){

		clog << (*it)[0] << " " <<  (*it)[1] << " " << (*it)[2] << endl;

		// grow region while initializing the levelset function to -FAR/FAR (or dist to isosurface if at an intersection)
		region_grow_dist(levelFunction, close, *it, thresh, data);



		it++;
	}

		cerr << "Marching forward." << endl;

		// iterate the fast marching method forward
		fast_march_forward(levelFunction, close);

		cerr << "Marching backward." << endl;

		// iterate the fast marching method backward
		fast_march_backward(levelFunction, close);

	//levelFunction.dump("original.raw");
	// smooth the level function
	//smooth(levelFunction);
	//smooth(levelFunction);
	//levelFunction.dump("smoothed.raw");
};

// if no intersection return false, else return true and set
// distance reference to interpolated distance to isosurface
// see Sethian pgs.133-135
bool get_lsintersection_inside(int x, int y, int z, 
		      const Volume<float> &data, 
		      float &distance){

	int n, pixelx, pixely, pixelz; //neighborhood indecies
	int numberOutside; // number of neighbors outside the isosurface
	float I[7]; // temp storage for pixel values
	float y0; // isovalue to find the intersection at
	float minXdist, SxMinus, SxPlus, minYdist, SyMinus, SyPlus, minZdist, SzMinus, SzPlus; //interpolated axis intersections
	bool xIntersect, xMinusIntersect, xPlusIntersect; // flag for x axis intersections
	bool yIntersect, yMinusIntersect, yPlusIntersect; // flag for y axis intersections
	bool zIntersect, zMinusIntersect, zPlusIntersect; // flag for z axis intersections

	I[0] = (float)(data.getVoxel(x, y, z));
	numberOutside = 0;
	for(n = 0; n < 6; n++){
		pixelx = (n == 0) ? x-1 : x;
		pixelx = (n == 1) ? x+1 : pixelx;
		pixely = (n == 2) ? y-1 : y;
		pixely = (n == 3) ? y+1 : pixely;
		pixelz = (n == 4) ? z-1 : z;
		pixelz = (n == 5) ? z+1 : pixelz;

		I[n+1] = (float)(data.getVoxel(pixelx, pixely, pixelz));
		if(I[n+1] >= 0) numberOutside += 1;
	}

	// there are no neighbors outside the isosurface
	if(numberOutside == 0){
		return false;
	}


	// decide if intersections exist and if so where they occur
	xMinusIntersect = interp_minus(float(x-1), float(x), I[1], I[0], SxMinus, 0);
	xPlusIntersect = interp_plus(float(x), float(x+1), I[0], I[2], SxPlus, 0);

	yMinusIntersect = interp_minus(float(y-1), float(y), I[3], I[0], SyMinus, 0);
	yPlusIntersect = interp_plus(float(y), float(y+1), I[0], I[4], SyPlus, 0);

	zMinusIntersect = interp_minus(float(z-1), float(z), I[5], I[0], SzMinus, 0);
	zPlusIntersect = interp_plus(float(z), float(z+1), I[0], I[6], SzPlus, 0);

	// compute the closest distance to the isosurface
	if(!xMinusIntersect & !xPlusIntersect){
		xIntersect = false;
	}
	if(xMinusIntersect & !xPlusIntersect){
		minXdist = SxMinus;
		xIntersect = true;
	}
	if(!xMinusIntersect & xPlusIntersect){
		minXdist = SxPlus;
		xIntersect = true;
	}
	if(xMinusIntersect & xPlusIntersect){
		minXdist = SxMinus;
		if(SxPlus < minXdist) minXdist = SxPlus; 
		xIntersect = true;
	}

	if(!yMinusIntersect & !yPlusIntersect){
		yIntersect = false;
	}
	if(yMinusIntersect & !yPlusIntersect){
		minYdist = SyMinus;
		yIntersect = true;
	}
	if(!yMinusIntersect & yPlusIntersect){
		minYdist = SyPlus;
		yIntersect = true;
	}
	if(yMinusIntersect & yPlusIntersect){
		minYdist = SyMinus;
		if(SyPlus < minYdist) minYdist = SyPlus; 
		yIntersect = true;
	}

	if(!zMinusIntersect & !zPlusIntersect){
		zIntersect = false;
	}
	if(zMinusIntersect & !zPlusIntersect){
		minZdist = SzMinus;
		zIntersect = true;
	}
	if(!zMinusIntersect & zPlusIntersect){
		minZdist = SzPlus;
		zIntersect = true;
	}
	if(zMinusIntersect & zPlusIntersect){
		minZdist = SzMinus;
		if(SzPlus < minZdist) minZdist = SzPlus; 
		zIntersect = true;
	}

	// depending on the above cases compute distance
	if(xIntersect & yIntersect & zIntersect){
		distance = 1/sqrt(1/(minXdist*minXdist) + 1/(minYdist*minYdist) + 1/(minZdist*minZdist));
	}
	if(xIntersect & yIntersect & !zIntersect){
		distance = 1/sqrt(1/(minXdist*minXdist) + 1/(minYdist*minYdist));
	}
	if(xIntersect & !yIntersect & zIntersect){
		distance = 1/sqrt(1/(minXdist*minXdist) + 1/(minZdist*minZdist));
	}
	if(xIntersect & !yIntersect & !zIntersect){
		distance = minXdist;
	}
	if(!xIntersect & yIntersect & zIntersect){
		distance = 1/sqrt(1/(minYdist*minYdist) + 1/(minZdist*minZdist));
	}
	if(!xIntersect & yIntersect & !zIntersect){
		distance = minYdist;
	}
	if(!xIntersect & !yIntersect & zIntersect){
		distance = minZdist;
	}
	if(!xIntersect & !yIntersect & !zIntersect){
		//we shouldn't ever get here
		
		assert(0);
	}

	//cerr << "Intersection at " << x << ", " << y << ", " << z << " : " << distance << endl;

	return true;
};

// if no intersection return false, else return true and set
// distance reference to interpolated distance to isosurface
// see Sethian pgs.133-135
bool get_lsintersection_outside(int x, int y, int z, 
		      const Volume<float> &data, 
		      float &distance){

	int n, pixelx, pixely, pixelz; //neighborhood indecies
	int numberInside; // number of neighbors inside the isosurface
	float I[7]; // temp storage for pixel values
	float y0; // isovalue to find the intersection at
	float minXdist, SxMinus, SxPlus, minYdist, SyMinus, SyPlus, minZdist, SzMinus, SzPlus; //interpolated axis intersections
	bool xIntersect, xMinusIntersect, xPlusIntersect; // flag for x axis intersections
	bool yIntersect, yMinusIntersect, yPlusIntersect; // flag for y axis intersections
	bool zIntersect, zMinusIntersect, zPlusIntersect; // flag for z axis intersections

	I[0] = (float)(data.getVoxel(x, y, z));
	numberInside = 0;
	for(n = 0; n < 6; n++){
		pixelx = (n == 0) ? x-1 : x;
		pixelx = (n == 1) ? x+1 : pixelx;
		pixely = (n == 2) ? y-1 : y;
		pixely = (n == 3) ? y+1 : pixely;
		pixelz = (n == 4) ? z-1 : z;
		pixelz = (n == 5) ? z+1 : pixelz;

		I[n+1] = (float)(data.getVoxel(pixelx, pixely, pixelz));
		if(I[n+1] < 0) numberInside += 1;
	}

	// there are no neighbors outside the isosurface
	if(numberInside == 0){
		return false;
	}


	// decide if intersections exist and if so where they occur
	// there are two cases for seedIntensity+thresh and seedIntensity-thresh
	xMinusIntersect = interp_minus(float(x-1), float(x), I[1], I[0], SxMinus, 0);
	xPlusIntersect = interp_plus(float(x), float(x+1), I[0], I[2], SxPlus, 0);

	yMinusIntersect = interp_minus(float(y-1), float(y), I[3], I[0], SyMinus, 0);
	yPlusIntersect = interp_plus(float(y), float(y+1), I[0], I[4], SyPlus, 0);

	zMinusIntersect = interp_minus(float(z-1), float(z), I[5], I[0], SzMinus, 0);
	zPlusIntersect = interp_plus(float(z), float(z+1), I[0], I[6], SzPlus, 0);

	// compute the closest distance to the isosurface
	if(!xMinusIntersect & !xPlusIntersect){
		xIntersect = false;
	}
	if(xMinusIntersect & !xPlusIntersect){
		minXdist = SxMinus;
		xIntersect = true;
	}
	if(!xMinusIntersect & xPlusIntersect){
		minXdist = SxPlus;
		xIntersect = true;
	}
	if(xMinusIntersect & xPlusIntersect){
		minXdist = SxMinus;
		if(SxPlus < minXdist) minXdist = SxPlus; 
		xIntersect = true;
	}

	if(!yMinusIntersect & !yPlusIntersect){
		yIntersect = false;
	}
	if(yMinusIntersect & !yPlusIntersect){
		minYdist = SyMinus;
		yIntersect = true;
	}
	if(!yMinusIntersect & yPlusIntersect){
		minYdist = SyPlus;
		yIntersect = true;
	}
	if(yMinusIntersect & yPlusIntersect){
		minYdist = SyMinus;
		if(SyPlus < minYdist) minYdist = SyPlus; 
		yIntersect = true;
	}

	if(!zMinusIntersect & !zPlusIntersect){
		zIntersect = false;
	}
	if(zMinusIntersect & !zPlusIntersect){
		minZdist = SzMinus;
		zIntersect = true;
	}
	if(!zMinusIntersect & zPlusIntersect){
		minZdist = SzPlus;
		zIntersect = true;
	}
	if(zMinusIntersect & zPlusIntersect){
		minZdist = SzMinus;
		if(SzPlus < minZdist) minZdist = SzPlus; 
		zIntersect = true;
	}

	// depending on the above cases compute distance
	if(xIntersect & yIntersect & zIntersect){
		distance = 1/sqrt(1/(minXdist*minXdist) + 1/(minYdist*minYdist) + 1/(minZdist*minZdist));
	}
	if(xIntersect & yIntersect & !zIntersect){
		distance = 1/sqrt(1/(minXdist*minXdist) + 1/(minYdist*minYdist));
	}
	if(xIntersect & !yIntersect & zIntersect){
		distance = 1/sqrt(1/(minXdist*minXdist) + 1/(minZdist*minZdist));
	}
	if(xIntersect & !yIntersect & !zIntersect){
		distance = minXdist;
	}
	if(!xIntersect & yIntersect & zIntersect){
		distance = 1/sqrt(1/(minYdist*minYdist) + 1/(minZdist*minZdist));
	}
	if(!xIntersect & yIntersect & !zIntersect){
		distance = minYdist;
	}
	if(!xIntersect & !yIntersect & zIntersect){
		distance = minZdist;
	}
	if(!xIntersect & !yIntersect & !zIntersect){
		//we shouldn't ever get here
		
		assert(0);
	}

	//cerr << "Intersection at " << x << ", " << y << ", " << z << " : " << distance << endl;

	return true;
};

void rebuildLevelFunction(LF3Dfull &levelFunction)
{
	unsigned int i, j, k;
	IMTK::Subscript<3> bounds, location;
	float distance;
	slist< IMTK::Subscript<3> > close; // list of initial close points to surface
	IMTK::Volume<float> oldLevelSet;
	bool intersect;

	bounds = levelFunction.getBounds();

	// clone old level set values
	oldLevelSet.setBounds(bounds);
	for(i = 0; i < bounds[0]*bounds[1]*bounds[2]; i++)
		oldLevelSet[i] = levelFunction[i];

	//loop through setting to close, -FAR, or FAR
	for(i=1; i < bounds[0]-1; i++)
		for(j=1; j < bounds[1]-1; j++)
			for(k=1; k < bounds[2]-1; k++){

				location[0] = i;
				location[1] = j;
				location[2] = k;

				// inside
				if(oldLevelSet.getVoxel(i, j, k) < 0){
					intersect = get_lsintersection_inside(i, j, k, oldLevelSet, distance); 

					if(intersect){
						levelFunction.setVoxel(i, j, k, -distance);
						close.push_front(location);
					}
					else{
						levelFunction.setVoxel(i, j, k, -FAR);
					}
				}

				// outside
				if(oldLevelSet.getVoxel(i, j, k) > 0){
					intersect = get_lsintersection_outside(i, j, k, oldLevelSet, distance); 

					if(intersect){
						levelFunction.setVoxel(i, j, k, distance);
						close.push_front(location);
					}
					else{
						levelFunction.setVoxel(i, j, k, FAR);
					}
				}

				if(oldLevelSet.getVoxel(i, j, k) == 0){ // force to be nonzero
					levelFunction.setVoxel(i, j, k, 0.0000000001);
					close.push_front(location);
				}
			}

	// clear old tube lists
	levelFunction.clearInner();
	levelFunction.clearOuter();

	clog << "Marching forward." << endl;
	// iterate the fast marching method forward
	fast_march_forward(levelFunction, close);

	clog << "Marching backward." << endl;
	// iterate the fast marching method backward
	fast_march_backward(levelFunction, close);

	// smooth the level function
	//smooth(levelFunction);
};
