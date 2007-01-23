// File: levelSetEvolve.cc
// Abstract: Evolution of the 3D levelset equation using volume data
//
// Author: Chris L. Wyatt 03/06/99
//
// Revised for using grid points on surface (no interpolation) 07/29/99

// NOTES:
//

#include <iostream>
#include <cstdio>

#include "Vector.hh"
#include "Mesh.hh"
#include "levelSetEvolve.hh"
#include "buildTube.hh"
#include "tetradecomp.hh"


#define PI 3.141592654

// function to compute the diffusion
inline void calcDiffusionOnline(IMTK::Volume<float> &data, int i, int j, int k, float &diffx, float &diffy, float &diffz){
	diffx = -(data.getVoxel(i+1, j, k) - data.getVoxel(i-1, j, k))/2;
	diffy = -(data.getVoxel(i, j+1, k) - data.getVoxel(i, j-1, k))/2;
	diffz = -(data.getVoxel(i, j, k+1) - data.getVoxel(i, j, k-1))/2;
}


//function to update inner tube points
float updateInner(LF3Dfull &levelFunction,
		 IMTK::Volume<float> &cmap,
		 float *holder,
		 double timeStep, 
		 double epsilon,
		 double F0)
{

	LF3Dfull::tubeIterator tubeit;
	IMTK::Subscript<3> current;
	
	int counter;

	float phi, kI, term1, term2, term3;
	double cdx,cdy,cdz,fdx,fdy,fdz,bdx,bdy,bdz,curv;
	double dxx,dxy,dxz,dyy,dyz,dzz,den,kf,maxkf,minkf;
	double maxbdx,minfdx,maxbdy,minfdy,maxbdz,minfdz,maxfdx,minbdx,maxfdy,minbdy,maxfdz,minbdz;
	double gradplus, gradminus, key;
	float u, v, w, minu, maxu, minv, maxv, minw, maxw, max;

	float phi0, phi1, phi2, phi3, phi4, phi5, phi6, phi7, phi8, phi9, phi10;
	float phi11, phi12, phi13, phi14, phi15, phi16, phi17, phi18, phi19, phi20, phi21, phi22, phi23, phi24;

	tubeit = levelFunction.innerBegin();
	counter = 0;
	max = 0.0;
	while( tubeit != levelFunction.innerEnd() ){

		current = *tubeit;
			
		// check if we are on the boundary


		phi0 = levelFunction.getVoxel(current[0], current[1], current[2]);
		phi1 = levelFunction.getVoxel(current[0]+1, current[1], current[2]);
		phi2 = levelFunction.getVoxel(current[0]-1, current[1], current[2]);
		phi3 = levelFunction.getVoxel(current[0], current[1]+1, current[2]);
		phi4 = levelFunction.getVoxel(current[0], current[1]-1, current[2]);
		phi5 = levelFunction.getVoxel(current[0], current[1], current[2]+1);
		phi6 = levelFunction.getVoxel(current[0], current[1], current[2]-1);
		phi7 = levelFunction.getVoxel(current[0]+2, current[1], current[2]);
		phi8 = levelFunction.getVoxel(current[0]-2, current[1], current[2]);
		phi9 = levelFunction.getVoxel(current[0], current[1]+2, current[2]);
		phi10 = levelFunction.getVoxel(current[0], current[1]-2, current[2]);
		phi11 = levelFunction.getVoxel(current[0], current[1], current[2]+2);
		phi12 = levelFunction.getVoxel(current[0], current[1], current[2]-2);
		phi13 = levelFunction.getVoxel(current[0]+1, current[1]+1, current[2]);
		phi14 = levelFunction.getVoxel(current[0]+1, current[1], current[2]+1);
		phi15 = levelFunction.getVoxel(current[0], current[1]+1, current[2]+1);
		phi16 = levelFunction.getVoxel(current[0]-1, current[1]+1, current[2]);
		phi17 = levelFunction.getVoxel(current[0]+1, current[1]-1, current[2]);
		phi18 = levelFunction.getVoxel(current[0]-1, current[1]-1, current[2]);
		phi19 = levelFunction.getVoxel(current[0]-1, current[1], current[2]+1);
		phi20 = levelFunction.getVoxel(current[0]+1, current[1], current[2]-1);
		phi21 = levelFunction.getVoxel(current[0]-1, current[1], current[2]-1);
		phi22 = levelFunction.getVoxel(current[0], current[1]-1, current[2]+1);
		phi23 = levelFunction.getVoxel(current[0], current[1]+1, current[2]-1);
		phi24 = levelFunction.getVoxel(current[0], current[1]-1, current[2]-1);
		
		//Compute forward derivative with respect to x.
		fdx = phi1 - phi0;
		
		//Compute backward derivative with respect to x.
		bdx = phi0 - phi2;
		
		//Compute central derivative with respect to x.
		cdx = (phi1 - phi2)*0.5;
		
		//Compute forward derivative with respect to y.
		fdy = phi3 - phi0;
		
		//Compute backward derivative with respect to y.
		bdy = phi0 - phi4;
		
		//Compute central derivative with respect to y.
		cdy = (phi3 - phi4)*0.5;
		
		//Compute forward derivative with respect to z.
		fdz = phi5 - phi0;
		
		//Compute backward derivative with respect to z.
		bdz = phi0 - phi6;
		
		//Compute central derivative with respect to z.
		cdz = (phi5 - phi6)*0.5;
		
		//Compute partials and curvature
		dxx = 0.25*((phi7 - phi0) - (phi0 - phi8));
		dyy = 0.25*((phi9 - phi0) - (phi0 - phi10));
		dzz = 0.25*((phi11 - phi0) - (phi0 - phi12));
		dxy = 0.25*((phi13 - phi16) - (phi17 - phi18));
		dxz = 0.25*((phi14 - phi19) - (phi20 - phi21));
		dyz = 0.25*((phi15 - phi22) - (phi23 - phi24));
		
		den = cdx*cdx + cdy*cdy + cdz*cdz;
		// from Dr. Ge's origional 2D code, I do the same here
		/* Note: The real curvature should use:
		   
		   dtemp = sqrt(cdx*cdx+cdy*cdy);
		   curv = (dxx*cdy*cdy - 2*cdy*cdx*dxy + dyy*cdx*cdx) / (dtemp*dtemp*dtemp);
		   
		   However, since in the actual update function, the curvature is
		   multiplied by a term of sqrt(cdx*cdx+cdy*cdy), I simply change the
		   value of curv to the one above and omit the sqrt(...) term in
		   the update function below.
		*/
		if (den == 0) 
			curv = 0;
		else 
			curv = ((dyy+dzz)*cdx*cdx + (dxx+dzz)*cdy*cdy + (dxx+dyy)*cdz*cdz -
				2*cdx*cdy*dxy - 2*cdx*cdz*dxz -2*cdy*cdz*dyz)/ den;
	   	
		//Update the level function
		kI = cmap.get(current);
		kf = F0*kI*kI;
		
		calcDiffusionOnline(cmap, current[0], current[1], current[2], u, v, w);
		
		if(u > 0){
			maxu = u;
			minu = 0;
		}
		else{
			maxu = 0;
			minu = u;
		}
		
		if(v > 0){
			maxv = v;
			minv = 0;
		}
		else{
			maxv = 0;
			minv = v;
		}
		if(w > 0){
			maxw = w;
			minw = 0;
		}
		else{
			maxw = 0;
			minw = w;
		}
		
		if (kf > 0) {
			maxkf = kf;
			minkf = 0;
		}
		else {
			maxkf = 0;
			minkf = kf;
		}
		
		if (fdx > 0) {
			maxfdx = fdx;
			minfdx = 0;
			}
		else {
			maxfdx = 0;
			minfdx = fdx;
		}
			
		if (bdx > 0) {
			maxbdx = bdx;
			minbdx = 0;
		}
		else {
			maxbdx = 0;
			minbdx = bdx;
		}
		
		if (fdy > 0) {
			maxfdy = fdy;
			minfdy = 0;
		}
		else {
			maxfdy = 0;
			minfdy = fdy;
		}
		
		if (bdy > 0) {
			maxbdy = bdy;
			minbdy = 0;
		}
		else {
			maxbdy = 0;
			minbdy = bdy;
		}
		
		if (fdz > 0) {
			maxfdz = fdz;
			minfdz = 0;
		}
		else {
			maxfdz = 0;
			minfdz = fdz;
		}
		
		if (bdz > 0) {
			maxbdz = bdz;
			minbdz = 0;
		}
		else {
			maxbdz = 0;
			minbdz = bdz;
		}
		
		gradplus = sqrt(maxbdx*maxbdx+minfdx*minfdx+maxbdy*maxbdy+minfdy*minfdy+maxbdz*maxbdz+minfdz*minfdz);
		gradminus = sqrt(maxfdx*maxfdx+minbdx*minbdx+maxfdy*maxfdy+minbdy*minbdy+maxfdz*maxfdz+minbdz*minbdz);

		
		term1 = maxkf*gradplus + minkf*gradminus;
		term2 = maxu*bdx + minu*fdx + maxv*bdy + minv*fdy + maxw*bdz + minw*fdz;
		term3 = epsilon*kI*curv;
		
		// GDM update
		//phi = phi0 + timeStep*(term1 + term2 + term3);

		//put update in temporary holder
		holder[counter] = (term1  - term2 + term3);
		if(fabs(holder[counter]) > max) max = fabs(holder[counter]);
 
		counter++;

		tubeit++;
	}

	return max;
};


//function to update outer tube points
float updateOuter(LF3Dfull &levelFunction,
		 IMTK::Volume<float> &cmap,
		 float *holder,
		 bool &rebuild,
		 double timeStep, 
		 double epsilon,
		 double F0)
{
	LF3Dfull::tubeIterator tubeit;
	IMTK::Subscript<3> current;
	int i, counter;
	float newphi, kI, term1, term2, term3;
	double cdx,cdy,cdz,fdx,fdy,fdz,bdx,bdy,bdz,curv;
	double dxx,dxy,dxz,dyy,dyz,dzz,den,kf,maxkf,minkf;
	double maxbdx,minfdx,maxbdy,minfdy,maxbdz,minfdz,maxfdx,minbdx,maxfdy,minbdy,maxfdz,minbdz;
	double gradplus, gradminus, key;
	float u, v, w, minu, maxu, minv, maxv, minw, maxw, max;
	float phi[24];
	bool onBoundary;

	tubeit = levelFunction.outerBegin();
	counter = 0;
	max = 0.0;
	while( (tubeit != levelFunction.outerEnd()) && (!rebuild) ){

		current = *tubeit;

		phi[0] = levelFunction.getVoxel(current[0], current[1], current[2]);
		phi[1] = levelFunction.getVoxel(current[0]+1, current[1], current[2]);
		phi[2] = levelFunction.getVoxel(current[0]-1, current[1], current[2]);
		phi[3] = levelFunction.getVoxel(current[0], current[1]+1, current[2]);
		phi[4] = levelFunction.getVoxel(current[0], current[1]-1, current[2]);
		phi[5] = levelFunction.getVoxel(current[0], current[1], current[2]+1);
		phi[6] = levelFunction.getVoxel(current[0], current[1], current[2]-1);
		phi[7] = levelFunction.getVoxel(current[0]+2, current[1], current[2]);
		phi[8] = levelFunction.getVoxel(current[0]-2, current[1], current[2]);
		phi[9] = levelFunction.getVoxel(current[0], current[1]+2, current[2]);
		phi[10] = levelFunction.getVoxel(current[0], current[1]-2, current[2]);
		phi[11] = levelFunction.getVoxel(current[0], current[1], current[2]+2);
		phi[12] = levelFunction.getVoxel(current[0], current[1], current[2]-2);
		phi[13] = levelFunction.getVoxel(current[0]+1, current[1]+1, current[2]);
		phi[14] = levelFunction.getVoxel(current[0]+1, current[1], current[2]+1);
		phi[15] = levelFunction.getVoxel(current[0], current[1]+1, current[2]+1);
		phi[16] = levelFunction.getVoxel(current[0]-1, current[1]+1, current[2]);
		phi[17] = levelFunction.getVoxel(current[0]+1, current[1]-1, current[2]);
		phi[18] = levelFunction.getVoxel(current[0]-1, current[1]-1, current[2]);
		phi[19] = levelFunction.getVoxel(current[0]-1, current[1], current[2]+1);
		phi[20] = levelFunction.getVoxel(current[0]+1, current[1], current[2]-1);
		phi[21] = levelFunction.getVoxel(current[0]-1, current[1], current[2]-1);
		phi[22] = levelFunction.getVoxel(current[0], current[1]-1, current[2]+1);
		phi[23] = levelFunction.getVoxel(current[0], current[1]+1, current[2]-1);
		phi[24] = levelFunction.getVoxel(current[0], current[1]-1, current[2]-1);
		
		// check if we are on the boundary
		onBoundary = false;
		if(phi[0] < 0){
			for(i=1; i < 25; i++){
				if(phi[i] <= -FAR){
					onBoundary = true;
					break;
				}
			}
		}
		if(phi[0] > 0){
			for(i=1; i < 25; i++){
				if(phi[i] >= FAR){
					onBoundary = true;
					break;
				}
			}
		}
		if(onBoundary){
			//fix current levelset value
			holder[counter] = 0.0;
			counter++;
			tubeit++;
			continue; // skip the rest
		}

		//Compute forward derivative with respect to x.
		fdx = phi[1] - phi[0];
		
		//Compute backward derivative with respect to x.
		bdx = phi[0] - phi[2];
		
		//Compute central derivative with respect to x.
		cdx = (phi[1] - phi[2])*0.5;
		
		//Compute forward derivative with respect to y.
		fdy = phi[3] - phi[0];
		
		//Compute backward derivative with respect to y.
		bdy = phi[0] - phi[4];
		
		//Compute central derivative with respect to y.
		cdy = (phi[3] - phi[4])*0.5;
		
		//Compute forward derivative with respect to z.
		fdz = phi[5] - phi[0];
		
		//Compute backward derivative with respect to z.
		bdz = phi[0] - phi[6];
		
		//Compute central derivative with respect to z.
		cdz = (phi[5] - phi[6])*0.5;
		
		//Compute partials and curvature
		dxx = 0.25*((phi[7] - phi[0]) - (phi[0] - phi[8]));
		dyy = 0.25*((phi[9] - phi[0]) - (phi[0] - phi[10]));
		dzz = 0.25*((phi[11] - phi[0]) - (phi[0] - phi[12]));
		dxy = 0.25*((phi[13] - phi[16]) - (phi[17] - phi[18]));
		dxz = 0.25*((phi[14] - phi[19]) - (phi[20] - phi[21]));
		dyz = 0.25*((phi[15] - phi[22]) - (phi[23] - phi[24]));
		
		den = cdx*cdx + cdy*cdy + cdz*cdz;
		// from Dr. Ge's origional 2D code, I do the same here
		/* Note: The real curvature should use:
		   
		   dtemp = sqrt(cdx*cdx+cdy*cdy);
		   curv = (dxx*cdy*cdy - 2*cdy*cdx*dxy + dyy*cdx*cdx) / (dtemp*dtemp*dtemp);
		   
		   However, since in the actual update function, the curvature is
		   multiplied by a term of sqrt(cdx*cdx+cdy*cdy), I simply change the
		   value of curv to the one above and omit the sqrt(...) term in
		   the update function below.
		*/
		if (den == 0) 
			curv = 0;
		else 
			curv = ((dyy+dzz)*cdx*cdx + (dxx+dzz)*cdy*cdy + (dxx+dyy)*cdz*cdz -
				2*cdx*cdy*dxy - 2*cdx*cdz*dxz -2*cdy*cdz*dyz)/ den;
	   	
		//Update the level function
		kI = cmap.get(current);
		kf = F0*kI*kI;
		
		calcDiffusionOnline(cmap, current[0], current[1], current[2], u, v, w);
		
		if(u > 0){
			maxu = u;
			minu = 0;
		}
		else{
			maxu = 0;
			minu = u;
		}
		
		if(v > 0){
			maxv = v;
			minv = 0;
		}
		else{
			maxv = 0;
			minv = v;
		}
		if(w > 0){
			maxw = w;
			minw = 0;
		}
		else{
			maxw = 0;
			minw = w;
		}
		
		if (kf > 0) {
			maxkf = kf;
			minkf = 0;
		}
		else {
			maxkf = 0;
			minkf = kf;
		}
		
		if (fdx > 0) {
			maxfdx = fdx;
			minfdx = 0;
			}
		else {
			maxfdx = 0;
			minfdx = fdx;
		}
			
		if (bdx > 0) {
			maxbdx = bdx;
			minbdx = 0;
		}
		else {
			maxbdx = 0;
			minbdx = bdx;
		}
		
		if (fdy > 0) {
			maxfdy = fdy;
			minfdy = 0;
		}
		else {
			maxfdy = 0;
			minfdy = fdy;
		}
		
		if (bdy > 0) {
			maxbdy = bdy;
			minbdy = 0;
		}
		else {
			maxbdy = 0;
			minbdy = bdy;
		}
		
		if (fdz > 0) {
			maxfdz = fdz;
			minfdz = 0;
		}
		else {
			maxfdz = 0;
			minfdz = fdz;
		}
		
		if (bdz > 0) {
			maxbdz = bdz;
			minbdz = 0;
		}
		else {
			maxbdz = 0;
			minbdz = bdz;
		}
		
		gradplus = sqrt(maxbdx*maxbdx+minfdx*minfdx+maxbdy*maxbdy+minfdy*minfdy+maxbdz*maxbdz+minfdz*minfdz);
		gradminus = sqrt(maxfdx*maxfdx+minbdx*minbdx+maxfdy*maxfdy+minbdy*minbdy+maxfdz*maxfdz+minbdz*minbdz);

		
		term1 = maxkf*gradplus + minkf*gradminus;
		term2 = maxu*bdx + minu*fdx + maxv*bdy + minv*fdy + maxw*bdz + minw*fdz;
		term3 = epsilon*kI*curv;
		
		// GDM update
		newphi = phi[0] + timeStep*(term1 - term2 + term3);

		//check for sign change
		rebuild = newphi*phi[0] < 0;
		if(rebuild){
			clog << "Sign change on outer tube detected." << endl;
		}

		//update in temporary holder
		holder[counter] = (term1 - term2 + term3);
		if(fabs(holder[counter]) > max) max = fabs(holder[counter]);

		counter++;

		tubeit++;
	}

	return max;
};


void levelSetEvolve(LF3Dfull &levelFunction,
		    IMTK::Volume<float> &cmap,
		    int numIterations, 
		    double timeStep, 
		    double epsilon,
		    double F0)
{
	IMTK::Subscript<3> bounds, current;
	LF3Dfull::tubeIterator tubeit;
	bool keepGoing, rebuild;
	int trip, dumptrip, counter, iteration;
	float *innerHolder, *outerHolder, innerMax, outerMax, maxUpdate, phi;

	IMTK::Mesh *mesh;
	char tempname[256];

	clog << "In levelSetEvolve:" << endl;
	clog << numIterations << " number iterations" << endl;
	clog << timeStep << " time step" << endl;
	clog << epsilon << " epsilon" << endl;
	clog << F0 << " F0" << endl;

	bounds = levelFunction.getBounds();

	clog << "Starting Iterations" << endl;
	iteration=0;
	keepGoing = numIterations > 0;

	innerHolder = new float[levelFunction.getNumInner()];
	outerHolder = new float[levelFunction.getNumOuter()];

	trip = 1;
	dumptrip = 0;
	while (keepGoing){
		rebuild = false;
 		if(iteration == trip*6){
			rebuild = true;
			trip++;
		}
		else{
			// compute GDM update for outer tube
			outerMax = updateOuter(levelFunction, cmap, outerHolder, rebuild, timeStep, epsilon, F0);
		}

		if(!rebuild){

			//levelFunction.dump("before.raw");

			// compute GDM update for inner tube
			innerMax = updateInner(levelFunction, cmap, innerHolder, timeStep, epsilon, F0);

			// CFL condition
			// maxUpdate = (innerMax > outerMax) ? innerMax : outerMax;
// 			cerr << maxUpdate << endl;
// 			timeStep = 0.5/maxUpdate;
// 			if(timeStep > 0.5) timeStep = 0.5;

			// update the level function with new update values
			tubeit = levelFunction.innerBegin();
			counter = 0;
			while(tubeit != levelFunction.innerEnd()){
				phi = levelFunction.get(*tubeit);
				levelFunction.set(*tubeit, phi + timeStep*innerHolder[counter]);
				counter++;
				tubeit++;
			}
			tubeit = levelFunction.outerBegin();
			counter = 0;
			while(tubeit != levelFunction.outerEnd()){
				phi = levelFunction.get(*tubeit);
				levelFunction.set(*tubeit, phi + timeStep*outerHolder[counter]);
				counter++;
				tubeit++;
			}

    			//levelFunction.dump("after.raw");

			iteration++;

		}
		// rebuild if nessesary
		else{

			clog << "Rebuilding Tube." << endl;

// 			levelFunction.dump("before_rebuild.raw");

			rebuildLevelFunction(levelFunction);

// 			levelFunction.dump("after_rebuild.raw");

			delete innerHolder;
			delete outerHolder;
			innerHolder = new float[levelFunction.getNumInner()];
			outerHolder = new float[levelFunction.getNumOuter()];
		}

// 		if( (iteration-1) == dumptrip*10){
	// 		mesh = tetradecomp(levelFunction, 2);
// 			sprintf(tempname, "levelset%i.oogl", iteration-1);
// 			ooglwrite(mesh, tempname);
// 			delete mesh;
//  			sprintf(tempname, "levelset%i.raw", iteration-1);
//  			levelFunction.dump(tempname);
// 			dumptrip++;
// 		}


		keepGoing = (iteration < numIterations); 

		clog << "Iteration " <<  iteration << " Time Step of " << timeStep << endl;

//    		cerr << "Press <return> to continue.";
//  		getc(stdin);


	}

	
};
