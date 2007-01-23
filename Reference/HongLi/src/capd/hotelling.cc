#include "hotelling.hh" 
#include "nrjacobi.hh"
/* I assume that the eigenvalues and eigenvectors have been 
 * allocated by calling nrvector() and matrix() functions
 * supplied by Numerical Recipes in C.
 */

 void hotelling(mvVec2f *pts, int npts, float *eigenvalues, float **eigenvectors)
{

	int i,nrot;
	double x, y, cv;
	mvVec2f avgPoint;
	float **covariance; 

	/* First we need to compute the average vector
	*/
	x = y = 0;
	for (i=0;i<npts;i++){
		x += pts[i].x;	
		y += pts[i].y;	
	}
	avgPoint.x = x/npts;
	avgPoint.y = y/npts;

	/* Now compute the normalized vectors
	*/

	for (i=0; i<npts; i++){
		pts[i].x -= avgPoint.x;
		pts[i].y -= avgPoint.y;
	}

	/* Now the covariance matrix;
	*/
	covariance = matrix<float>(1,2,1,2);
	cv = 0.0;
	for (i=0; i<npts; i++){
		cv += pts[i].x*pts[i].x;
	}
	covariance[1][1] = cv/npts;
	cv = 0.0;
	for (i=0; i<npts; i++){
		cv += pts[i].x*pts[i].y;
	}
	covariance[1][2] = cv/npts;

	covariance[2][1] = covariance[1][2];

	cv = 0.0;
	for (i=0; i<npts; i++){
		cv += pts[i].y*pts[i].y;
	}
	covariance[2][2] = cv/npts;

	/* Now compute the eigenvalues and eigenvectors
	*/

	jacobi(covariance, 2, eigenvalues, eigenvectors, &nrot);
	
	free_matrix(covariance, 1, 2, 1, 2);
}


 void hotelling(mvVec3f *pts, int npts, float *eigenvalues, float **eigenvectors)
{

	int i,nrot;
	double x, y, z, cv;
	mvVec3f avgPoint;
	float **covariance; 

	/* First we need to compute the average vector
	*/
	x = y = z = 0;
	for (i=0;i<npts;i++){
		x += pts[i].x;	
		y += pts[i].y;	
		z += pts[i].z;	
	}
	avgPoint.x = x/npts;
	avgPoint.y = y/npts;
	avgPoint.z = z/npts;

	/* Now compute the normalized vectors
	*/

	for (i=0; i<npts; i++){
		pts[i].x -= avgPoint.x;
		pts[i].y -= avgPoint.y;
		pts[i].z -= avgPoint.z;
	}

	/* Now the covariance matrix;
	*/
	covariance = matrix<float>(1,3,1,3);
	cv = 0.0;
	for (i=0; i<npts; i++){
		cv += pts[i].x*pts[i].x;
	}
	covariance[1][1] = cv/npts;

	cv = 0.0;
	for (i=0; i<npts; i++){
		cv += pts[i].x*pts[i].y;
	}
	covariance[1][2] = cv/npts;

	cv = 0.0;
	for (i=0; i<npts; i++){
		cv += pts[i].x*pts[i].z;
	}
	covariance[1][3] = cv/npts;

	cv = 0.0;
	for (i=0; i<npts; i++){
		cv += pts[i].y*pts[i].x;
	}
	covariance[2][1] = cv/npts;

	cv = 0.0;
	for (i=0; i<npts; i++){
		cv += pts[i].y*pts[i].y;
	}
	covariance[2][2] = cv/npts;

	cv = 0.0;
	for (i=0; i<npts; i++){
		cv += pts[i].y*pts[i].z;
	}
	covariance[2][3] = cv/npts;

	cv = 0.0;
	for (i=0; i<npts; i++){
		cv += pts[i].z*pts[i].x;
	}
	covariance[3][1] = cv/npts;

	cv = 0.0;
	for (i=0; i<npts; i++){
		cv += pts[i].z*pts[i].y;
	}
	covariance[3][2] = cv/npts;

	cv = 0.0;
	for (i=0; i<npts; i++){
		cv += pts[i].z*pts[i].z;
	}
	covariance[3][3] = cv/npts;

	/* Now compute the eigenvalues and eigenvectors
	*/

	jacobi(covariance, 3, eigenvalues, eigenvectors, &nrot);

	free_matrix(covariance, 1, 3, 1, 3);
}

