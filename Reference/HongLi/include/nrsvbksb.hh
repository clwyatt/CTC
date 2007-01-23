/***************************************************************************
File: nrsvbksb.hh
Abstract:  function used by invertMatrixSVD in nrinvert.hh
Created: 09/2001

(C) 2002 by Wake Forest University School of Medicine

Last Revision ($Revision: 1.1.1.1 $) 
by $Author: hongli $ on $Date: 2002/12/06 21:49:30 $
***************************************************************************/

#ifndef _SVBKSB_HH_
#define _SVBKSB_HH_

#include <stdlib.h>
#include <stdio.h>
#include "nrfree_vector.hh"
#include "nrvector.hh"
template <class T>
void svbksb(T **u,T w[],T **v,int m,int n,T b[],T x[]);


/**
* A function called only by invertMatrixSVD in invert.hh
* kernel code comes from 'Numeric Recipe' 
* @see invertMatrixSVD()
* @see svdcmp()
*/
template <class T>
void svbksb(T **u,T w[],T **v,int m,int n,T b[],T x[])
/*
Solves A.X = B for a vector X, where A is specified by the arrays
u[1..m][1..n],w[1..n],v[1..n][1..n] as returned by svdcmp. m and n
are dimensions of A, and will be equal for square matrices. b[1..m]
is the input right-hand side. x[1..n] is the output solution vector.
No input quantities are destroyed, so the routine may be called
sequentially with different b's.
*/
{
	int jj,j,i;
	T s,*tmp;


	tmp=nrvector<T>(1,n);
	for (j=1;j<=n;j++) {
		s=0.0;
		if (w[j]) {
			for (i=1;i<=m;i++) s += u[i][j]*b[i];
			s /= w[j];
		}
		tmp[j]=s;
	}
	for (j=1;j<=n;j++) {
		s=0.0;
		for (jj=1;jj<=n;jj++) s += v[j][jj]*tmp[jj];
		x[j]=s;
	}
	free_vector(tmp,1,n);
}

#endif
