/********************************************************
File: SpecFun.cc
Abstract: implementation for special functions

See header (SpecFun.hh) for documentation

Last Revision ($Revision: 1.2 $) 
by $Author: clwyatt $ 
on $Date: 2002/02/27 20:07:48 $
*********************************************************/

#include <iostream>

#include "SpecFun.hh"

using namespace std;

#ifdef IRIX
#include <assert.h>
#include <math.h>
#else
#include <cassert>
#include <cmath>
#endif


//**************************************************************
// functions common to the computation of the special functions
//**************************************************************
#define EPS 2.2204e-16
#define ITMAX 100


double gammaln(double xx){
	double x, y, tmp, ser;
	int j;
	static double cof[6] = {76.18009172947146,-86.50532032941677,
			      24.01409824083091,-1.231739572450155,
			      0.1208650973866179e-2,-0.5395239384953e-5};

	y = x = xx;
	tmp = x+5.5;
	tmp -= (x+0.5)*log(tmp);
	ser = 1.000000000190015;

	for(j=0; j <= 5; j++)
		ser += cof[j]/++y;

	return -tmp + log(2.5066282746310005*ser/x);
};

void gser(double &gamser, double a, double x){
	int n;
	double sum, del, ap, gln;

	gln = gammaln(a);

	if(x <= 0.0) {
		if(x < 0.0) clog << "WARNING: x less than 0 in SpecFun gser" << endl;
		gamser=0.0;
		return;
	}
	else{
		ap = a;
		del = sum = 1.0/a;
		for(n=1; n<=ITMAX; n++){
			++ap; 
			del *= x/ap;
			sum += del;
			if(fabs(del) < fabs(sum)*EPS){
				gamser = sum*exp(-x + a*log(x) - gln);
				return; 
			} 
		} 
		clog << "WARNING: argument a too large, ITMAX too small in SpecFun gser" << endl;
		return;
	}
};

void gcf(double &gammacf, double a, double x){
	int i;
	double an, b, c, d, del, h, gln;

	gln = gammaln(a);
	b = x + 1.0 - a;
	c = 1.0/DBL_MIN;
	d = 1.0/b;
	h = d;

	for(i=1; i <= ITMAX; i++){
		an = -i*(i-a);
		b += 2.0;
		d = an*d + b;
		if(fabs(d) < DBL_MIN) d = DBL_MIN;
		c = b + an/c;
		if(fabs(c) < DBL_MIN) c = DBL_MIN;
		d = 1.0/d;
		del = d*c;
		h *= del;
		if(fabs(del-1.0) < EPS) break;
	}

	assert(i <= ITMAX);

	gammacf = exp(-x + a*log(x) - gln)*h;
};

//**************************************************************
// special function implementations
//**************************************************************

double IMTK::erf(double x){

	return x < 0.0 ? -gammap(0.5, x*x) : gammap(0.5, x*x);
};

double IMTK::erfinv(double x){

	int iter = 0;
	double znew;
	double zold = 1;
	double s = 0.8862269255;

	if(x > +0){
		znew = sqrt(-log(1-fabs(x)));
	}
	else{
		znew = - sqrt(-log(1-fabs(x)));
	}

	while(fabs(erf(znew) - x) > EPS*fabs(x)){
		zold = znew;
		znew = zold - (erf(zold) - x)*exp(zold*zold)*s;
		if(iter++ > ITMAX){
			clog << "WARNING: iterations exceeded in SpecFun erfinv" << endl;
			break;
		}
	}

	return znew;
};

double IMTK::gammap(double a, double x){
	double gamser, gammcf;

	assert(x >= 0.0 && a > 0.0);

	if(x < (a + 1.0)){
		gser(gamser, a, x);
		return gamser;
	}
	else{
		gcf(gammcf, a, x);
		return 1.0 - gammcf;
	}

	return 0;
};

