/********************************************************
File: SpecFun.hh
Abstract: Computational routines for special functions.
          Most of these routines follow NRC, except for
	  erfinv, which is similar to the method used in 
	  Octave and Matlab.

Created: 12/29/1999
Author: Chris L. Wyatt

Last Revision ($Revision: 1.1.1.1 $) 
by $Author: hongli $ 
on $Date: 2002/12/06 21:49:33 $
*********************************************************/

//prevent recursive includes

#ifndef _SpecFun_hh
#define _SpecFun_hh 1

namespace IMTK{

/**
* Compute the error function
* 
* @param x the input value
* @return result
*/
double erf(double x);

/**
* Compute the inverse error function
* 
* @param x the input value
* @return result
*/
double erfinv(double x);

/**
* Compute the incomplete gamma function, P(a,x)
* 
* @param x the input value
* @return result
*/
double gammap(double a, double x);


}

#endif
