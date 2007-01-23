/***************************************************************************
File: nrinvert.hh
Abstract:  Calculate inverse of a given matrix
Created: 09/2001

(C) 2002 by Wake Forest University School of Medicine

Last Revision ($Revision: 1.1.1.1 $) 
by $Author: hongli $ on $Date: 2002/12/06 21:49:32 $
***************************************************************************/

#ifndef _INVERT_HH_
#define _INVERT_HH_

#include <stdlib.h>
#include <math.h>
#include "nrsvdcmp.hh"
#include "nrsvbksb.hh"
#include "nrfree_matrix.hh"
#include "nrfree_vector.hh"
#include "nrmatrix.hh"

#define TOL_ 1.0e-10

template <class T>
void invertMatrixSVD(T **a, T **y, int n, T & cond);


/**
* An function template to calculate inverse matrix
*   the kernal cades comes from 'Numeric Recipe'. SVD
*   is used for the inverse calculation.
* @param a input matrix
* @param y the inverse matrix of input a
* @param n size of the input matrix
* @param cond condition number of the matrix.
*/
template <class T>
void invertMatrixSVD(T **a, T **y, int n, T &cond)
{
	int j,i;
	T **u, *w, **v, wmax,wmin, thresh,*x,*col;
	
	u = matrix<T>(1,n,1,n);
	v = matrix<T>(1,n,1,n);
	w = nrvector<T>(1,n);
	col = nrvector<T>(1,n);
	x = nrvector<T>(1,n);

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
	wmin=100000.0;
	for (j=1;j<=n;j++)
		{
		 if (w[j] > wmax) wmax=w[j];
		 if (w[j] < wmin) wmin=w[j];
		 }
	if (wmin == 0) cond = 100000000.0;
	else	cond = wmax/wmin;
	 	 
	thresh=TOL_*wmax;

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
	free_vector(w,1,n);
	free_vector(col,1,n);
	free_vector(x,1,n);
}

#endif
