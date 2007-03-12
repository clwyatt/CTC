#include "invertMatrix.h"

/***************************************** invertMatrix.cc ****************************/

/* Numeric Recipe, page 45.
*/


//extern float *vector();
//extern float **matrix();
//extern int *ivector();

/*
void invertMatrixLU(float **a, float **y, int n)
{
	float **u, *col, d;
	int i,j;
	int *indx;

	col = vector(1,n);
	indx = ivector(1,n);
	u = matrix(1,n,1,n);

	for(i=1;i<=n;i++){
		for(j=1;j<=n;j++){
			u[i][j] = a[i][j];
		}
	}

	ludcmp(u, n, indx, &d);
	for (j=1; j<=n; j++){
		for (i=1; i<=n; i++) col[i] = 0.0;
		col[j] = 1.0;
		lubksb(u, n, indx, col);
		for (i=1; i<=n; i++) y[i][j] = col[i];
	}

	free_vector(col,1,n);
	free_ivector(indx,1,n);
	free_matrix(u,1,n,1,n);
}
*/


#define TOL 1.0e-5

void invertMatrixSVD(float **a, float **y, int n)
{
	int j,i;
	float **u, *w, **v, wmax,thresh,*x,*col;
	
	u = matrix(1,n,1,n);
	v = matrix(1,n,1,n);
	w = vector(1,n);
	col = vector(1,n);
	x = vector(1,n);

	//fprintf(stderr,"Matrix ---------------------------------------\n");
	for(i=1;i<=n;i++){
		for(j=1;j<=n;j++){
			u[i][j] = a[i][j];
			//fprintf(stderr,"%.3f ", u[i][j]);
		}
		//fprintf(stderr,"\n");
	}
	svdcmp(u,n,n,w,v);
	wmax=0.0;
	for (j=1;j<=n;j++)
		if (w[j] > wmax) wmax=w[j];
	thresh=TOL*wmax;

	//fprintf(stderr,"singular values -------------------------------\n");
	//for (j=1;j<=n;j++)
		//fprintf(stderr,"%.3f ", w[j]);
	//fprintf(stderr,"\n");
	for (j=1;j<=n;j++)
		if (w[j] < thresh){
			w[j]=0.0;
		}
	//fprintf(stderr,"New singular values (thresh = %f) -----------\n", thresh);
	//for (j=1;j<=n;j++)
		//fprintf(stderr,"%.3f ", w[j]);
	//fprintf(stderr,"\n");

	for (j=1; j<=n; j++){
		for (i=1; i<=n; i++) col[i] = 0.0;
		col[j] = 1.0;
		svbksb(u,w,v,n,n,col,x);
		for (i=1; i<=n; i++) y[i][j] = x[i];
	}

	free_matrix(u,1,n,1,n);
	free_matrix(v,1,n,1,n);
	free_fvector(w,1,n);
	free_fvector(col,1,n);
	free_fvector(x,1,n);
}
