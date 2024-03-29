/***************************************************************************
File: nrjacobi.hh
Abstract: Template for Jacobi matrix calculation. the kernal comes from
          Numeric Recipe. It is modified to be an template to calculate
	  various type input.
Created: 03/2002

(C) 2002 by Wake Forest University School of Medicine

Last Revision ($Revision: 1.1.1.1 $) 
by $Author: hongli $ on $Date: 2002/12/06 21:49:32 $
***************************************************************************/

#ifndef _NRJACOBI_HH_ 
#define _NRJACOBI_HH_ 

#include <stdlib.h>
#include <stdio.h>
#include "nrfree_vector.hh"
#include "nrvector.hh"
#include <math.h>
#define NRANSI

#define ROTATE(a,i,j,k,l) g=a[i][j];h=a[k][l];a[i][j]=g-s*(h+g*tau);\
        a[k][l]=h+s*(g-h*tau);

template <class T>
void jacobi(T **a, int n, T d[], T **v, int *nrot);


/**
*	An function template for calculate Jacobin 
*	the original program comes from 'Numeric Recipe'
*	@param a input matrix
*	@param n the size of the matrix
*	@param d
*	@param v
*	@param nrot
*/
template <class T>
void jacobi(T **a, int n, T d[], T **v, int *nrot)
{
        int j,iq,ip,i;
        T tresh,theta,tau,t,sm,s,h,g,c,*b,*z;

        b=nrvector<T>(1,n);
        z=nrvector<T>(1,n);
        for (ip=1;ip<=n;ip++) {
                for (iq=1;iq<=n;iq++) v[ip][iq]=0.0;
                v[ip][ip]=1.0;
        }
        for (ip=1;ip<=n;ip++) {
                b[ip]=d[ip]=a[ip][ip];
                z[ip]=0.0;
        }
        *nrot=0;
        for (i=1;i<=50;i++) {
                sm=0.0;
                for (ip=1;ip<=n-1;ip++) {
                        for (iq=ip+1;iq<=n;iq++)
                                sm += fabs(a[ip][iq]);
                }
                if (sm == 0.0) {
                        free_vector(z,1,n);
                        free_vector(b,1,n);
                        return;
                }
                if (i < 4)
                        tresh=0.2*sm/(n*n);
                else
                        tresh=0.0;
                for (ip=1;ip<=n-1;ip++) {
                        for (iq=ip+1;iq<=n;iq++) {
                                g=100.0*fabs(a[ip][iq]);
                                if (i > 4 && (float)(fabs(d[ip])+g) == (float)fabs(d[ip])
                                        && (float)(fabs(d[iq])+g) == (float)fabs(d[iq]))
                                        a[ip][iq]=0.0;
                                else if (fabs(a[ip][iq]) > tresh) {
                                        h=d[iq]-d[ip];
                                        if ((float)(fabs(h)+g) == (float)fabs(h))
                                                t=(a[ip][iq])/h;
                                        else {
                                                theta=0.5*h/(a[ip][iq]);
                                                t=1.0/(fabs(theta)+sqrt(1.0+theta*theta));
                                                if (theta < 0.0) t = -t;
                                        }
                                        c=1.0/sqrt(1+t*t);
                                        s=t*c;
                                        tau=s/(1.0+c);
                                        h=t*a[ip][iq];
                                        z[ip] -= h;
                                        z[iq] += h;
                                        d[ip] -= h;
                                        d[iq] += h;
                                        a[ip][iq]=0.0;
                                        for (j=1;j<=ip-1;j++) {
                                                ROTATE(a,j,ip,j,iq)
                                        }
                                        for (j=ip+1;j<=iq-1;j++) {
                                                ROTATE(a,ip,j,j,iq)
                                        }
                                        for (j=iq+1;j<=n;j++) {
                                                ROTATE(a,ip,j,iq,j)
                                        }
                                        for (j=1;j<=n;j++) {
                                                ROTATE(v,j,ip,j,iq)
                                        }
                                        ++(*nrot);
                                }
                        }
                }
                for (ip=1;ip<=n;ip++) {
                        b[ip] += z[ip];
                        d[ip]=b[ip];
                        z[ip]=0.0;
                }
        }
        nrerror("Too many iterations in routine jacobi");
}
#undef ROTATE
#undef NRANSI
/* (C) Copr. 1986-92 Numerical Recipes Software 9)&K9K. */
#endif  // #ifndef JACOBI_HH_ 

