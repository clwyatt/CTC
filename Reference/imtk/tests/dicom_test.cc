/********************************************************
File: dicom_test.cc
Abstract: Test file for Dicom class. 
Created: 11/16/2001
Author: Chris L. Wyatt

Last Revision ($Revision: 1.1 $) 
by $Author: clwyatt $ 
on $Date: 2001/11/16 21:46:33 $
*********************************************************/
#include <iostream>

#include "Dicom.hh"
#include "Subscript.hh"
#include "Image.hh"

using namespace std;

int main(int argc, char **argv) 
{
	IMTK::Dicom dcmObj;
	
	std::string fname = "temp.dcm";
	if(!dcmObj.open(fname)){
		clog << "Call to open failed." << endl;
		return 0;
	}
	
	float xo, yo, zo;
	if(dcmObj.getOrigin(xo, yo, zo))
		clog << "Image Origin is " << xo << ", " << yo << ", " << zo << endl;
	else
		clog << "Call to getOrigin failed." << endl;
		
	float dx, dy;
	if(dcmObj.getPxlSize(dx, dy))
		clog << "Pixel Size is " << dx << " x " << dy << endl;
	else
		clog << "Call to getPxlSize failed." << endl;
		
		
	int num;
	if(dcmObj.getSliceNum(num))
		clog << "Slice number is " << num << endl;
	else
		clog << "Call to getSliceNum failed." << endl;
		
	unsigned short xdim, ydim;
	if(dcmObj.getDims(xdim, ydim))
		clog << "Image Dimensions are " << xdim << " x " << ydim << endl;
	else{
		clog << "Call to getDims failed." << endl;
		return 0;
	}
		
	IMTK::Subscript<2> bounds;
	bounds[0] = xdim;
	bounds[1] = ydim;
	
	unsigned short *data;
	data = new unsigned short[xdim*ydim];
	if(!dcmObj.getData(data))
		clog << "Call to getData failed." << endl;
	else{
		IMTK::Image<unsigned short> im(bounds, data);
		im.dump("temp.raw");	
	}

	dcmObj.close();

	delete [] data;
}




