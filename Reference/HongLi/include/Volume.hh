/********************************************************
File: Volume.hh
Abstract: A general 3 dimensional volume template. 
Created: 06/02/1998
Author: Chris L. Wyatt

Last Revision ($Revision: 1.2 $) 
by $Author: hongli $ 
on $Date: 2003/10/02 19:45:40 $
*********************************************************/

//prevent recursive includes
#ifndef _Volume_hh
#define _Volume_hh 1

#ifdef IRIX
#include <assert.h>
#include <math.h>
#else
#include <cassert>
#include <cmath>
#include <string>
#endif

#include <iostream>


// Write an Analyze format to disk 
#include "AnalyzeHdr.hh"

#include "Data.hh"
#include "NDArray.hh"
#include "Subscript.hh"

#include "bytereverse.hh"
#define GAP 10


/** 3 dimensional volume template.

A base class template implementing the storage of 3D
grid based data and basic element mathmatics. Data type is
specified when instanciated and must have defined
the appropriate overloads (+ - * / =).

AxialSlice, SagitalSlice, and CoronalSlice are nested classes.
They provide effecient access to the volume data similar to
an iterator.

The bounds, inherited from NDArray, specify volume boundaries in X, Y, Z order.

\begin{verbatim}
    z
   /
  /
 /________ y
 |
 |
 |
 |
 x
\end{verbatim}

@author Chris L. Wyatt <cwyatt@wfubmc.edu>
@version $Revision: 1.2 $
*/

namespace IMTK{

	// some common error conditions returned
 

template <class T>
class Volume: public NDArray<T,3> 
{
private:
	// coordinate spacing and origin
	float dx, dy, dz; // = to 1 if unknown
	float xorig, yorig, zorig; // = to 0 if unknown
	int typeindex; //this is for saving file

protected:
	
public:

	//forward declarations for slice objects
	class AxialSlice;
	class SagitalSlice;
	class CoronalSlice;

	//make them friendly
	friend class AxialSlice;
	friend class SagitalSlice;
	friend class CoronalSlice;
	
	void setType(int i) {typeindex = i;}
	int  getType()	{return typeindex;}


	// friend classes for efficient access to slices of 3D volume
	/// Axial Slice class
	class AxialSlice{
	private:
		Volume<T> * _parent;
		unsigned int _nrows;
		unsigned int _ncols;
		unsigned int _z;
	public:
		// not very usefull constructor;
		AxialSlice(){}

		AxialSlice(Volume<T> * theP, unsigned int theZ){
			_parent = theP;
			_ncols = _parent->dims[0];
			_nrows = _parent->dims[1];
			assert(theZ < _parent->dims[2]);
			_z = theZ;
		}
		
		///get number of rows
		inline unsigned int nrows(){ return _nrows;}

		//get number of columns
		inline unsigned int ncols(){ return _ncols;}

		//get current slice
		inline unsigned int slice(){ return _z;}

		/// get location
		inline T get(unsigned int theC, unsigned int theR){
			return _parent->getVoxel(theC, theR, _z);
// 			unsigned int index =  _z*(_parent->dims[0])*(_parent->dims[1])
// 				+ theC*(_parent->dims[0]) + theR;
// 			assert((index < _parent->n_));
// 			return *(_parent->d_ + index);
		}
		
		/// set location
		inline void set(unsigned int theC, unsigned int theR, T theVal){
			//_parent->setVoxel(theC, theR, _z, theVal);
			unsigned int index =  _z*(_parent->dims[0])*(_parent->dims[1])
				+ theR*(_parent->dims[0]) + theC;
			assert((index < _parent->n_));
			*(_parent->d_ + index) = theVal;
		}
		bool write(ofstream & theOutStream){
			T temptype;
			if( theOutStream.bad() ){ //bad stream
				return false;
			}

			for(unsigned int i = 0; i < _nrows; i++)
				for (unsigned int j = 0; j < _ncols; j++){
				temptype = get(j,i);
				theOutStream.write((char *) &temptype, sizeof temptype);
				if( theOutStream.bad() ) return false;
			}
			return true;
		}
		bool write(ofstream & theOutStream, float min, float max, int x, int y){
			if( theOutStream.bad() ){ //bad stream
				printf("bad file \n");
				return false;
			}
	   		int i,j, pad;              
		        char id0;        
           		char id1;
           		long filesize;
           		short res0;
           		short res1;
           		long headersize=1078;
           		long infosize;
           		long nfil;
           		long ncol;
           		short bitplanes;
           		short bits;
           		long bicompres;
           		long bisizeimage;
           		long bix;
           		long biy;
           		long biclr;
           		long biclrimp;
			unsigned char paleta[768];

	  		for(i=0;i<256;i++)  {
	       			for(j=0;j<=2;j++) {
        	   			paleta[3*i+j]=i; 
	       				}
    				}

   			/******** escrivint el header **************/
        
   			id0='B';         
   			id1='M';
		        pad = (4 - ((_ncols) % 4)) * _nrows;
   			filesize=headersize+_ncols*_nrows + pad;
   			res0=0;
   			res1=0;
			/* headersize=1078;  */
   			infosize=0x28;
   			nfil=_nrows;
   			ncol=_ncols;
   			bitplanes=1;
   			bits=8;
   			bicompres=0;
  			bisizeimage=0;
   			bix=0;
   			biy=0;
   			biclr=0;
   			biclrimp=0;
			//swap byteorder for unix
			bits= byte_reverse(bits);
			bitplanes= byte_reverse(bitplanes);
			ncol= byte_reverse(ncol);
			nfil= byte_reverse(nfil);
			infosize= byte_reverse(infosize);
			filesize= byte_reverse(filesize);
			headersize= byte_reverse(headersize);
          
   			/****** escrivim el header *********/    
   			theOutStream.put(id0);
   			theOutStream.put(id1);
   			theOutStream.write(&filesize,sizeof(long));
   			theOutStream.write(&res0,sizeof(short));
   			theOutStream.write(&res1,sizeof(short)); 
   			theOutStream.write(&headersize,sizeof(long));
  	 		theOutStream.write(&infosize,sizeof(long));
   			theOutStream.write(&ncol,sizeof(long));
   			theOutStream.write(&nfil,sizeof(long));
   			theOutStream.write(&bitplanes,sizeof(short));
   			theOutStream.write(&bits,sizeof(short)); 
   			theOutStream.write(&bicompres,sizeof(long));
   			theOutStream.write(&bisizeimage,sizeof(long));   
   			theOutStream.write(&bix,sizeof(long));
   			theOutStream.write(&biy,sizeof(long));
   			theOutStream.write(&biclr,sizeof(long));
   			theOutStream.write(&biclrimp,sizeof(long));      

		   	for(i=0;i<256;++i){
			     theOutStream.put(paleta[i*3+2]);theOutStream.put(paleta[i*3+1]);
			     theOutStream.put(paleta[i*3+0]);theOutStream.put(0);
			   }


			float value;
			unsigned char v;
			pad = 4 - _ncols%4;
			if(pad ==4 ) pad = 0;
			for(  i = _nrows-1 ; i >= 0; i--)
				{
				for (  j = 0; j < _ncols; j++){
					if( (j == x && fabs(y-i) > GAP) || (i == y && fabs(j-x) > GAP)||(j == x -1 && fabs(y-i) > GAP) || (i == y-1 && fabs(j-x) > GAP)) 
				 	{
						v = 255;
						theOutStream.write(& v , 1);
						if( theOutStream.bad() ) return false;
					}
					else
					{
						value = (float) get(j,i);
						if( value < min) value = min;
						if( value > max) value = max;
						value = floor(255 * (value - min)/(max-min));
						v = (unsigned char) value;
						theOutStream.write(& v , 1);
						if( theOutStream.bad() ) return false;
					}
					}
				v = 0;	
            			for (int jj = 1; jj <= pad; jj++)
					theOutStream.write(&v  , 1);
				}				
			return true;
		}
            
		bool write(ofstream & theOutStream, unsigned int xfrom, unsigned int xto,unsigned  int yfrom,unsigned  int yto){
			T temptype;
			if( xfrom < 0 || xto >= _ncols || yfrom < 0 || yto >= _nrows || theOutStream.bad() ){ //bad stream
				printf("bad file \n");
				return false;
			}

			for(unsigned int i = yfrom; i < yto; i++)
				for (unsigned int j = xfrom; j < xto; j++){
				temptype = get(j,i);
				theOutStream.write((char *) &temptype, sizeof temptype);
				if( theOutStream.bad() ) return false;
			}
			return true;
		}

		bool write(ofstream & theOutStream,unsigned  int xfrom,unsigned  int xto, unsigned int yfrom, unsigned int yto, float min, float max){
			if( xfrom < 0 || xto >= _ncols || yfrom < 0 || yto >= _nrows || theOutStream.bad() ){ //bad stream
				printf("bad file \n");
				return false;
			}
	   		int i,j;              
		        char id0;        
           		char id1;
           		long filesize;
           		short res0;
           		short res1;
           		long headersize=1078;
           		long infosize;
           		long nfil;
           		long ncol;
           		short bitplanes;
           		short bits;
           		long bicompres;
           		long bisizeimage;
           		long bix;
           		long biy;
           		long biclr;
           		long biclrimp;
			unsigned char paleta[768];

	  		for(i=0;i<256;i++)  {
	       			for(j=0;j<=2;j++) {
        	   			paleta[3*i+j]=i; 
	       				}
    				}

   			/******** escrivint el header **************/
        
   			id0='B';         
   			id1='M';
   			nfil=yto-yfrom;
   			ncol=xto-xfrom;
   			filesize=headersize+nfil*ncol;
   			res0=0;
   			res1=0;
			/* headersize=1078;  */
   			infosize=0x28;
   			bitplanes=1;
   			bits=8;
			
   			bicompres=0;
  			bisizeimage=0;
   			bix=0;
   			biy=0;
   			biclr=0;
   			biclrimp=0;
        		
			//swap byteorder for unix
			bits= byte_reverse(bits);
			bitplanes= byte_reverse(bitplanes);
			ncol= byte_reverse(ncol);
			nfil= byte_reverse(nfil);
			infosize= byte_reverse(infosize);
			filesize= byte_reverse(filesize);
			headersize= byte_reverse(headersize);
			
   			/****** escrivim el header *********/    
   			theOutStream.put(id0);
   			theOutStream.put(id1);
   			theOutStream.write(&filesize,sizeof(long));
   			theOutStream.write(&res0,sizeof(short));
   			theOutStream.write(&res1,sizeof(short)); 
   			theOutStream.write(&headersize,sizeof(long));
  	 		theOutStream.write(&infosize,sizeof(long));
   			theOutStream.write(&ncol,sizeof(long));
   			theOutStream.write(&nfil,sizeof(long));
   			theOutStream.write(&bitplanes,sizeof(short));
   			theOutStream.write(&bits,sizeof(short)); 
   			theOutStream.write(&bicompres,sizeof(long));
   			theOutStream.write(&bisizeimage,sizeof(long));   
   			theOutStream.write(&bix,sizeof(long));
   			theOutStream.write(&biy,sizeof(long));
   			theOutStream.write(&biclr,sizeof(long));
   			theOutStream.write(&biclrimp,sizeof(long));      

		   	for(i=0;i<256;++i){
			     theOutStream.put(paleta[i*3+2]);theOutStream.put(paleta[i*3+1]);
			     theOutStream.put(paleta[i*3+0]);theOutStream.put(0);
			   }

			float value;
			char v;
			for(unsigned int i = yto -1; i >= yfrom; i--)
				for (unsigned int j = xfrom; j < xto; j++){
				value = (float) get(j,i);
				if( value < min) value = min;
				if( value > max) value = max;
				value = floor(255 * (value - min)/(max-min));
				v = (char) value;
				theOutStream.write(& v , 1);
				if( theOutStream.bad() ) return false;
			}
			return true;
		}
	};

	/// Sagital Slice class
	class SagitalSlice{
	private:
		Volume<T> * _parent;
		unsigned int _nrows;
		unsigned int _ncols;
		unsigned int _y;
	public:
		// not very usefull constructor;
		SagitalSlice(){}

		SagitalSlice(Volume<T> * theP, unsigned int theY){
			_parent = theP;
			_nrows = _parent->dims[2];
			_ncols = _parent->dims[0];
			assert(theY < _parent->dims[1]);
			_y = theY;
		}
		
		///get number of rows
		inline unsigned int nrows(){ return _nrows;}

		//get number of columns
		inline unsigned int ncols(){ return _ncols;}

		//get current slice
		inline unsigned int slice(){ return _y;}

		/// get location
		inline T get(unsigned int theC, unsigned int theR){
			return _parent->getVoxel(theC, _y, theR);
// 			unsigned int index =  theC*(_parent->dims[0])*(_parent->dims[1])
// 				+ theR*(_parent->dims[1]) + _y;
// 			assert((index < _parent->n_));
// 			return *(_parent->d_ + index);
		}
		
		/// set location
		inline void set(unsigned int theC, unsigned int theR, T theVal){
			unsigned int index =  theR*(_parent->dims[0])*(_parent->dims[1])
				+ _y*(_parent->dims[1]) + theC;
			assert((index < _parent->n_));
			*(_parent->d_ + index) = theVal;
		}
		
		bool write(ofstream & theOutStream){
			T temptype;
			if( theOutStream.bad() ){ //bad stream
				return false;
			}

			for(unsigned int i = 0; i < _nrows; i++)
				for (unsigned int j = 0; j < _ncols; j++){
				temptype = get(j,i);
				theOutStream.write((char *) &temptype, sizeof temptype);
				if( theOutStream.bad() ) return false;
			}
			return true;
		}

		bool write(ofstream & theOutStream, float min, float max, int x,int y){
			if( theOutStream.bad() ){ //bad stream
				return false;
			}
	   		int i,j, pad;              
		        char id0;        
           		char id1;
           		long filesize;
           		short res0;
           		short res1;
           		long headersize=1078;
           		long infosize;
           		long nfil;
           		long ncol;
           		short bitplanes;
           		short bits;
           		long bicompres;
           		long bisizeimage;
           		long bix;
           		long biy;
           		long biclr;
           		long biclrimp;
			unsigned char paleta[768];

	  		for(i=0;i<256;i++)  {
	       			for(j=0;j<=2;j++) {
        	   			paleta[3*i+j]=i; 
	       				}
    				}

   			/******** escrivint el header **************/
        
   			id0='B';         
   			id1='M';
		        pad = (4 - ((_ncols) % 4)) * _nrows;
   			filesize=headersize+_ncols*_nrows + pad;
   			res0=0;
   			res1=0;
			/* headersize=1078;  */
   			infosize=0x28;
   			nfil=_nrows;
   			ncol=_ncols;
   			bitplanes=1;
   			bits=8;
   			bicompres=0;
  			bisizeimage=0;
   			bix=0;
   			biy=0;
   			biclr=0;
   			biclrimp=0;
			//swap byteorder for unix
			bits= byte_reverse(bits);
			bitplanes= byte_reverse(bitplanes);
			ncol= byte_reverse(ncol);
			nfil= byte_reverse(nfil);
			infosize= byte_reverse(infosize);
			filesize= byte_reverse(filesize);
			headersize= byte_reverse(headersize);
          
   			/****** escrivim el header *********/    
   			theOutStream.put(id0);
   			theOutStream.put(id1);
   			theOutStream.write(&filesize,sizeof(long));
   			theOutStream.write(&res0,sizeof(short));
   			theOutStream.write(&res1,sizeof(short)); 
   			theOutStream.write(&headersize,sizeof(long));
  	 		theOutStream.write(&infosize,sizeof(long));
   			theOutStream.write(&ncol,sizeof(long));
   			theOutStream.write(&nfil,sizeof(long));
   			theOutStream.write(&bitplanes,sizeof(short));
   			theOutStream.write(&bits,sizeof(short)); 
   			theOutStream.write(&bicompres,sizeof(long));
   			theOutStream.write(&bisizeimage,sizeof(long));   
   			theOutStream.write(&bix,sizeof(long));
   			theOutStream.write(&biy,sizeof(long));
   			theOutStream.write(&biclr,sizeof(long));
   			theOutStream.write(&biclrimp,sizeof(long));      

		   	for(i=0;i<256;++i){
			     theOutStream.put(paleta[i*3+2]);theOutStream.put(paleta[i*3+1]);
			     theOutStream.put(paleta[i*3+0]);theOutStream.put(0);
			   }

			float value;
			char v;
			pad = 4 - _ncols%4;
			if(pad ==4 ) pad = 0;
			for(  i = _nrows-1 ; i >= 0; i--)
				{
				for (  j = 0; j < _ncols; j++){
					if( (j == x && fabs(y-i) > GAP) || (i == y && fabs(j-x) > GAP)||(j == x -1 && fabs(y-i) > GAP) || (i == y-1 && fabs(j-x) > GAP)) 
				 	{
						v = 255;
						theOutStream.write(& v , 1);
						if( theOutStream.bad() ) return false;
					}
					else
					{
						value = (float) get(j,i);
						if( value < min) value = min;
						if( value > max) value = max;
						value = floor(255 * (value - min)/(max-min));
						v = (unsigned char) value;
						theOutStream.write(& v , 1);
						if( theOutStream.bad() ) return false;
					}
					}
				v = 0;	
            			for (int jj = 1; jj <= pad; jj++)
					theOutStream.write(&v , 1);
				}	
			return true;
		}
            
		
		bool write(ofstream & theOutStream,unsigned  int xfrom, unsigned int xto,unsigned  int yfrom,unsigned  int yto){
			T temptype;
			if( xfrom < 0 || xto >= _ncols || yfrom < 0 || yto >= nrows || theOutStream.bad() ){ //bad stream
				printf("bad file \n");
				return false;
			}

			for(unsigned int i = yfrom; i < yto; i++)
				for (unsigned int j = xfrom; j < xto; j++){
				temptype = get(j,i);
				theOutStream.write((char *) &temptype, sizeof temptype);
				if( theOutStream.bad() ) return false;
			}
			return true;
		}
		bool write(ofstream & theOutStream,unsigned  int xfrom,unsigned  int xto,unsigned  int yfrom,unsigned  int yto, float min, float max){
			T temptype;
			if( xfrom < 0 || xto >= _ncols || yfrom < 0 || yto >= _nrows || theOutStream.bad() ){ //bad stream
				printf("bad file \n");
				return false;
			}
	   		int i,j;              
		        char id0;        
           		char id1;
           		long filesize;
           		short res0;
           		short res1;
           		long headersize=1078;
           		long infosize;
           		long nfil;
           		long ncol;
           		short bitplanes;
           		short bits;
           		long bicompres;
           		long bisizeimage;
           		long bix;
           		long biy;
           		long biclr;
           		long biclrimp;
			unsigned char paleta[768];

	  		for(i=0;i<256;i++)  {
	       			for(j=0;j<=2;j++) {
        	   			paleta[3*i+j]=i; 
	       				}
    				}

   			/******** escrivint el header **************/
        
   			id0='B';         
   			id1='M';
   			nfil=yto-yfrom;
   			ncol=xto-xfrom;
   			filesize=headersize+nfil*ncol;
   			res0=0;
   			res1=0;
			/* headersize=1078;  */
   			infosize=0x28;
   			bitplanes=1;
   			bits=8;
   			bicompres=0;
  			bisizeimage=0;
   			bix=0;
   			biy=0;
   			biclr=0;
   			biclrimp=0;
			//swap byteorder for unix
			bits= byte_reverse(bits);
			bitplanes= byte_reverse(bitplanes);
			ncol= byte_reverse(ncol);
			nfil= byte_reverse(nfil);
			infosize= byte_reverse(infosize);
			filesize= byte_reverse(filesize);
			headersize= byte_reverse(headersize);
          
   			/****** escrivim el header *********/    
   			theOutStream.put(id0);
   			theOutStream.put(id1);
   			theOutStream.write(&filesize,sizeof(long));
   			theOutStream.write(&res0,sizeof(short));
   			theOutStream.write(&res1,sizeof(short)); 
   			theOutStream.write(&headersize,sizeof(long));
  	 		theOutStream.write(&infosize,sizeof(long));
   			theOutStream.write(&ncol,sizeof(long));
   			theOutStream.write(&nfil,sizeof(long));
   			theOutStream.write(&bitplanes,sizeof(short));
   			theOutStream.write(&bits,sizeof(short)); 
   			theOutStream.write(&bicompres,sizeof(long));
   			theOutStream.write(&bisizeimage,sizeof(long));   
   			theOutStream.write(&bix,sizeof(long));
   			theOutStream.write(&biy,sizeof(long));
   			theOutStream.write(&biclr,sizeof(long));
   			theOutStream.write(&biclrimp,sizeof(long));      

		   	for(i=0;i<256;++i){
			     theOutStream.put(paleta[i*3+2]);theOutStream.put(paleta[i*3+1]);
			     theOutStream.put(paleta[i*3+0]);theOutStream.put(0);
			   }

			float value;
			char v;
			for(unsigned int i = yto -1; i >= yfrom; i--)
				for (unsigned int j = xfrom; j < xto; j++){
				value = (float) get(j,i);
				if( value < min) value = min;
				if( value > max) value = max;
				value = floor(255 * (value - min)/(max-min));
				v = (char) value;
				theOutStream.write(& v , 1);
				if( theOutStream.bad() ) return false;
			}
			return true;
		}
};

	/// Coronal Slice class
	class CoronalSlice{
	private:
		Volume<T> * _parent;
		unsigned int _nrows;
		unsigned int _ncols;
		unsigned int _x;
	public:
		// not very usefull constructor;
		CoronalSlice(){}

		CoronalSlice(Volume<T> * theP, unsigned int theX){
			_parent = theP;
			_nrows = _parent->dims[1];
			_ncols = _parent->dims[2];
			assert(theX < _parent->dims[0]);
			_x = theX;
		}
		
		///get number of rows
		inline unsigned int nrows(){ return _nrows;}

		//get number of columns
		inline unsigned int ncols(){ return _ncols;}

		//get current slice
		inline unsigned int slice(){ return _x;}

		/// get location
		inline T get(unsigned int theC, unsigned int theR){
			return _parent->getVoxel(_x, theR, theC);
// 			unsigned int index =  theR*(_parent->dims[0])*(_parent->dims[1])
// 				+ _x*(_parent->dims[1]) + theC;
// 			assert((index < _parent->n_));
// 			return *(_parent->d_ + index);
		}
		
		/// set location
		inline void set(unsigned int theC, unsigned int theR, T theVal){
			unsigned int index =  theC*(_parent->dims[0])*(_parent->dims[1])
				+ theR*(_parent->dims[1]) + _x;
			assert((index < _parent->n_));
			*(_parent->d_ + index) = theVal;
		}
		
		bool write(ofstream & theOutStream){
			T temptype;
			if( theOutStream.bad() ){ //bad stream
				return false;
			}

			for(int i = 0; i < _nrows; i++)
				for (int j = 0; j < _ncols; j++){
				temptype = get(j,i);
				theOutStream.write((char *) &temptype, sizeof temptype);
				if( theOutStream.bad() ) return false;
			}
			return true;
		}
		bool write(ofstream & theOutStream, float min, float max, int x, int y){
			if( theOutStream.bad() ){ //bad stream
				printf("bad file \n");
				return false;
			}
	   		int i,j, pad;              
		        char id0;        
           		char id1;
           		long filesize;
           		short res0;
           		short res1;
           		long headersize=1078;
           		long infosize;
           		long nfil;
           		long ncol;
           		short bitplanes;
           		short bits;
           		long bicompres;
           		long bisizeimage;
           		long bix;
           		long biy;
           		long biclr;
           		long biclrimp;
			unsigned char paleta[768];

	  		for(i=0;i<256;i++)  {
	       			for(j=0;j<=2;j++) {
        	   			paleta[3*i+j]=i; 
	       				}
    				}

   			/******** escrivint el header **************/
   			id0='B';         
   			id1='M';
			pad = 4 - _ncols%4;
			if(pad ==4 ) pad = 0;
   			filesize=headersize+_ncols*_nrows + pad * _nrows;
   			res0=0;
   			res1=0;
			/* headersize=1078;  */
   			infosize=0x28;
   			nfil=_nrows;
   			ncol=_ncols;
   			bitplanes=1;
   			bits=8;
   			bicompres=0;
  			bisizeimage=0;
   			bix=0;
   			biy=0;
   			biclr=0;
   			biclrimp=0;
			//swap byteorder for unix
			bits= byte_reverse(bits);
			bitplanes= byte_reverse(bitplanes);
			ncol= byte_reverse(ncol);
			nfil= byte_reverse(nfil);
			infosize= byte_reverse(infosize);
			filesize= byte_reverse(filesize);
			headersize= byte_reverse(headersize);
          
   			/****** escrivim el header *********/    
   			theOutStream.put(id0);
   			theOutStream.put(id1);
   			theOutStream.write(&filesize,sizeof(long));
   			theOutStream.write(&res0,sizeof(short));
   			theOutStream.write(&res1,sizeof(short)); 
   			theOutStream.write(&headersize,sizeof(long));
  	 		theOutStream.write(&infosize,sizeof(long));
   			theOutStream.write(&ncol,sizeof(long));
   			theOutStream.write(&nfil,sizeof(long));
   			theOutStream.write(&bitplanes,sizeof(short));
   			theOutStream.write(&bits,sizeof(short)); 
   			theOutStream.write(&bicompres,sizeof(long));
   			theOutStream.write(&bisizeimage,sizeof(long));   
   			theOutStream.write(&bix,sizeof(long));
   			theOutStream.write(&biy,sizeof(long));
   			theOutStream.write(&biclr,sizeof(long));
   			theOutStream.write(&biclrimp,sizeof(long));      

		   	for(i=0;i<256;++i){
			     theOutStream.put(paleta[i*3+2]);theOutStream.put(paleta[i*3+1]);
			     theOutStream.put(paleta[i*3+0]);theOutStream.put(0);
			   }

			float value;
			char v;
			for(  i = _nrows-1 ; i >= 0; i--)
				{
				for (  j = 0; j < _ncols; j++)
					{
					if( (j == x && fabs(y-i) > GAP) || (i == y && fabs(j-x) > GAP)||(j == x -1 && fabs(y-i) > GAP) || (i == y-1 && fabs(j-x) > GAP)) 
				 		{
						v = 255;
						theOutStream.write(& v , 1);
						if( theOutStream.bad() ) return false;
						}
					else
						{
						value = (float) get(j,i);
						if( value < min) value = min;
						if( value > max) value = max;
						value = floor(255 * (value - min)/(max-min));
						v = (unsigned char) value;
						theOutStream.write(& v , 1);
						if( theOutStream.bad() ) return false;
						}
					}
				v = 0;	
            			for (int jj = 1; jj <= pad; jj++)
					theOutStream.write(&v , 1);
				}	
			return true;
		}
            
		bool write(ofstream & theOutStream,unsigned  int xfrom, unsigned int xto,unsigned  int yfrom, unsigned int yto){
			T temptype;
			if( xfrom < 0 || xto >= _ncols || yfrom < 0 || yto >= nrows || theOutStream.bad() ){ //bad stream
				printf("bad file \n");
				return false;
			}

			for(unsigned int i = yfrom; i < yto; i++)
				for (unsigned int j = xfrom; j < xto; j++){
				temptype = get(j,i);
				theOutStream.write((char *) &temptype, sizeof temptype);
				if( theOutStream.bad() ) return false;
			}
			return true;
		}

		bool write(ofstream & theOutStream,unsigned  int xfrom,unsigned  int xto,unsigned  int yfrom,unsigned  int yto, float min, float max){
			T temptype;
			if( xfrom < 0 || xto > _ncols || yfrom < 0 || yto > _nrows || theOutStream.bad() ){ //bad stream
				printf("bad file \n");
				return false;
			}
	   		int i,j;              
		        char id0;        
           		char id1;
           		long filesize;
           		short res0;
           		short res1;
           		long headersize=1078;
           		long infosize;
           		long nfil;
           		long ncol;
           		short bitplanes;
           		short bits;
           		long bicompres;
           		long bisizeimage;
           		long bix;
           		long biy;
           		long biclr;
           		long biclrimp;
			unsigned char paleta[768];

	  		for(i=0;i<256;i++)  {
	       			for(j=0;j<=2;j++) {
        	   			paleta[3*i+j]=i; 
	       				}
    				}

   			/******** escrivint el header **************/
        
   			id0='B';         
   			id1='M';
   			nfil=yto-yfrom;
   			ncol=xto-xfrom;
   			filesize=headersize+nfil*ncol;
   			res0=0;
   			res1=0;
			/* headersize=1078;  */
   			infosize=0x28;
   			bitplanes=1;
   			bits=8;
   			bicompres=0;
  			bisizeimage=0;
   			bix=0;
   			biy=0;
   			biclr=0;
   			biclrimp=0;
			//swap byteorder for unix
			bits= byte_reverse(bits);
			bitplanes= byte_reverse(bitplanes);
			ncol= byte_reverse(ncol);
			nfil= byte_reverse(nfil);
			infosize= byte_reverse(infosize);
			filesize= byte_reverse(filesize);
			headersize= byte_reverse(headersize);
          
   			/****** escrivim el header *********/    
   			theOutStream.put(id0);
   			theOutStream.put(id1);
   			theOutStream.write(&filesize,sizeof(long));
   			theOutStream.write(&res0,sizeof(short));
   			theOutStream.write(&res1,sizeof(short)); 
   			theOutStream.write(&headersize,sizeof(long));
  	 		theOutStream.write(&infosize,sizeof(long));
   			theOutStream.write(&ncol,sizeof(long));
   			theOutStream.write(&nfil,sizeof(long));
   			theOutStream.write(&bitplanes,sizeof(short));
   			theOutStream.write(&bits,sizeof(short)); 
   			theOutStream.write(&bicompres,sizeof(long));
   			theOutStream.write(&bisizeimage,sizeof(long));   
   			theOutStream.write(&bix,sizeof(long));
   			theOutStream.write(&biy,sizeof(long));
   			theOutStream.write(&biclr,sizeof(long));
   			theOutStream.write(&biclrimp,sizeof(long));      

		   	for(i=0;i<256;++i){
			     theOutStream.put(paleta[i*3+2]);theOutStream.put(paleta[i*3+1]);
			     theOutStream.put(paleta[i*3+0]);theOutStream.put(0);
			   }

			float value;
			char v;
			for( int i = yto -1; i >= yfrom; i--)
				{
				for (int j = xfrom; j < xto; j++)
					{
					value = (float) get(j,i);
					if( value < min) value = min;
					if( value > max) value = max;
					value = floor(255 * (value - min)/(max-min));
					v = (char) value;
					theOutStream.write(& v , 1);
					if( theOutStream.bad() ) return false;
					}
				}
			return true;
		}
};

	/// Default Constructor
	Volume(): NDArray<T,3>(){

		dx = 1; dy = 1; dz = 1;
		xorig = 0; yorig = 0; zorig = 0;
	}

	/**
	 * Construct Volume with diminsions
	 * 
	 * @param theDims subscript list of dimensions
	 */
	Volume(Subscript<3> theDims): NDArray<T,3>(theDims){}
	
	/**
	 * Construct Volume with diminsions and data
	 * 
	 * @param theDims subscript list of dimensions
	 * @param theDataPtr pointer to array data
	 */
	Volume(Subscript<3> theDims, T * theDataPtr): NDArray<T,3>(theDims, theDataPtr){}

	/// Copy Constructor for NDArray to Volume
	Volume(const NDArray<T,3> & A): NDArray<T,3>(A){}

	/// Copy Constructor for Volume to Volume
	Volume(const Volume<T> & V): NDArray<T,3>(V){}

	/// Destructor
	~Volume(){}
	
	/// get axial slicer
	AxialSlice axial(unsigned int theZ){
		
		assert(theZ < dims[2]);

		return AxialSlice(this, theZ);
	}

	/// get sagital slicer
	SagitalSlice sagital(unsigned int theY){
		
		assert(theY < dims[1]);

		return SagitalSlice(this, theY);
	}

	/// get coroanl slicer
	CoronalSlice coronal(unsigned int theX){
		
		assert(theX < dims[0]);

		return CoronalSlice(this, theX);
	}

	/**
	 * Set value of Volume
	 * 
	 * @param theX
	 * @param theY
	 * @param theZ
	 * @param value value to set location too
	 */
	inline void setVoxel(unsigned int theX, unsigned int theY, unsigned int theZ, T value){

		Subscript<3> sub;

		sub[0] = theX;
		sub[1] = theY;
		sub[2] = theZ;

		this->set(sub, value);
	}

	/**
	 * Return value of Volume
	 * 
	 * @param theX
	 * @param theY
	 * @param theZ
	 * @return value
	 */
	inline T getVoxel(unsigned int theX, unsigned int theY, unsigned int theZ) const{

		Subscript<3> sub;

		sub[0] = theX;
		sub[1] = theY;
		sub[2] = theZ;

		return this->get(sub);
	}

	/**
	 * Get the voxel spacing/sizes
	 * 
	 * @param theDX
	 * @param theDY
	 * @param theDZ
	 */
	inline void getVoxelSpacing(float &theDX, float &theDY, float &theDZ){

		theDX = dx;
		theDY = dy;
		theDZ = dz;
	}

	/**
	 * Set the voxel spacing/sizes
	 * 
	 * @param theDX
	 * @param theDY
	 * @param theDZ
	 */
	inline void setVoxelSpacing(float theDX, float theDY, float theDZ){

		dx = theDX;
		dy = theDY;
		dz = theDZ;
	}

	/**
	 * Get the origin location of voxel (0,0,0)
	 * 
	 * @param theOrigX
	 * @param theOrigY
	 * @param theOrigZ
	 */
	inline void getOrigin(float &theOrigX, float &theOrigY, float &theOrigZ){

		theOrigX = xorig;
		theOrigY = yorig;
		theOrigZ = zorig;
	}

	/**
	 * Set the origin location of voxel (0,0,0)
	 * 
	 * @param theOrigX
	 * @param theOrigY
	 * @param theOrigZ
	 */
	inline void setOrigin(float theOrigX, float theOrigY, float theOrigZ){

		xorig = theOrigX;
		yorig = theOrigY;
		zorig = theOrigZ;
	}

	/**
	 * write the volume data to the given file output stream
	 * 
	 * @param theOutStream
	 */
	bool write(ofstream & theOutStream){

		if( theOutStream.bad() ){ //bad stream
			printf("bad file \n");
			return false;
		}

		// temporary holders
		unsigned long tempul;
		float tempf1, tempf2, tempf3;
		T temptype;

		tempul = dims[0];
		theOutStream.write((char *) &tempul, sizeof tempul);
		if( theOutStream.bad() ) return false;
		tempul = dims[1];
		theOutStream.write((char *) &tempul, sizeof tempul);
		if( theOutStream.bad() ) return false;
		tempul = dims[2];
		theOutStream.write((char *) &tempul, sizeof tempul);
		if( theOutStream.bad() ) return false;

		tempul = sizeof temptype;
		theOutStream.write((char *) &tempul, sizeof tempul);
		if( theOutStream.bad() ) return false;

		getOrigin(tempf1, tempf2, tempf3);
		theOutStream.write((char *) &tempf1, sizeof tempf1);
		if( theOutStream.bad() ) return false;
		theOutStream.write((char *) &tempf2, sizeof tempf2);
		if( theOutStream.bad() ) return false;
		theOutStream.write((char *) &tempf3, sizeof tempf3);
		if( theOutStream.bad() ) return false;

		getVoxelSpacing(tempf1, tempf2, tempf3);
		theOutStream.write((char *) &tempf1, sizeof tempf1);
		if( theOutStream.bad() ) return false;
		theOutStream.write((char *) &tempf2, sizeof tempf2);
		if( theOutStream.bad() ) return false;
		theOutStream.write((char *) &tempf3, sizeof tempf3);
		if( theOutStream.bad() ) return false;

		for(int i = 0; i < size(); i++){
			temptype = (*this)[i];
			theOutStream.write((char *) &temptype, sizeof temptype);
			if( theOutStream.bad() ) return false;
		}

		return true;
	}

	/**
	 * read the volume data from the given file input stream
	 * 
	 * @param theInStream
	 */
	bool read(ifstream & theInStream){

		if( theInStream.bad() ){ //bad stream
			return false;
		}

		// temporary holders
		unsigned long tempul;
		float tempf1, tempf2, tempf3;
		T temptype;

		theInStream.read((char *) &tempul, sizeof tempul);
		if( theInStream.bad() ) return false;
		dims[0] = tempul;
		theInStream.read((char *) &tempul, sizeof tempul);
		if( theInStream.bad() ) return false;
		dims[1] = tempul;
		theInStream.read((char *) &tempul, sizeof tempul);
		if( theInStream.bad() ) return false;
		dims[2] = tempul;

		theInStream.read((char *) &tempul, sizeof tempul);
		if( theInStream.bad() ) return false;
		if( tempul != sizeof(temptype) ) return false;

		setBounds(dims);
		clog << "Volume size: " <<  dims[0] << " x " << dims[1] << " x " << dims[2];

		theInStream.read((char *) &tempf1, sizeof tempf1);
		if( theInStream.bad() ) return false;
		theInStream.read((char *) &tempf2, sizeof tempf2);
		if( theInStream.bad() ) return false;
		theInStream.read((char *) &tempf3, sizeof tempf3);
		if( theInStream.bad() ) return false;
		setOrigin(tempf1, tempf2, tempf3);
		clog << "     origon: " << tempf1 << ", " << tempf2 << ", " << tempf3 << endl;

		theInStream.read((char *) &tempf1, sizeof tempf1);
		if( theInStream.bad() ) return false;
		theInStream.read((char *) &tempf2, sizeof tempf2);
		if( theInStream.bad() ) return false;
		theInStream.read((char *) &tempf3, sizeof tempf3);
		if( theInStream.bad() ) return false;
		setVoxelSpacing(tempf1, tempf2, tempf3);
		clog << "       (mm): " << dims[0]*tempf1 << "mm x " << dims[1]*tempf2 << "mm x " << dims[2]*tempf3 << "mm";

		for(int i = 0; i < size(); i++){
			theInStream.read((char *) &temptype, sizeof temptype);
			if( theInStream.bad() ) return false;
			(*this)[i] = temptype;
		}

		return true;
	}
	

void writeAnalyze( const std::string & dbName,int &error)
  {
  ofstream outfile;
  std::string fileName = dbName + ".hdr";
  outfile.open( fileName.c_str() );
  if(!outfile){
	cerr << "Error opening Analyze(?) file " << fileName.c_str() << endl;
	error = 1;
	return;
   }

  struct dsr * ds;
  ds = new dsr;
  ds->hk.fill(  );
  ds->dime.fill( dims[0], dims[1], dims[2], dx,dy,dz, getType() );
  ds->hist.fill( getType() );

  // Write the struct ds (size exactly 348 bytes) to disk.
  outfile.write( &(*ds), sizeof( struct dsr ));
  outfile.close();
  
  fileName = dbName + ".img";
  outfile.open( fileName.c_str() );
  if(!outfile){
	cerr << "Error opening Analyze(?) file " << fileName.c_str() << endl;
	error = 2;
	return;
	}
  
  T temp; 
  unsigned int i, j, k;
  for(k=0; k < dims[2]; k++)
	for(j=0; j < dims[1]; j++)
		for(i=0; i < dims[0]; i++){
			temp = getVoxel(i, j, k);
			outfile.write((char *)(&temp), sizeof(temp));
		}

	// clean up
   outfile.close(); 
}
	

};
}

#endif
