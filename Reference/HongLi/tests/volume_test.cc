/********************************************************
File: volume_test.cc
Abstract: Test file for Volume template. 
Created: 11/04/2000
Author: Chris L. Wyatt

Last Revision ($Revision: 1.1.1.1 $) 
by $Author: hongli $ 
on $Date: 2002/12/06 21:49:27 $
*********************************************************/
#include <iostream>

#include "Volume.hh"

using namespace std;

int main(int argc, char **argv) {

	unsigned int r, c, count;
	IMTK::Volume<double> array1;
	IMTK::Subscript<3> sub1;
	IMTK::Subscript<3> bounds;
	
	bounds[0] = 5;
	bounds[1] = 10;
	bounds[2] = 15;

	array1.setBounds(bounds);
	IMTK::NDArray<double,3>::iterator it = array1.begin();
	count = 0;
	while(it != array1.end()){
		*it = count;
		it++;
		count++;
	}

	cout << "Value at (3,3,3) = " << array1.getVoxel(3, 3, 3) << endl;

	sub1[0] = 3;
	sub1[1] = 3;
	sub1[2] = 3;

	cout << "Value at (3,3,3) = " << array1.get(sub1) << endl;
 
	IMTK::Volume<double>::AxialSlice as = array1.axial(3);

	for(r=0; r < as.nrows(); r++)
		for(c=0; c < as.ncols(); c++){
			cout << "Axial (" << r << "," << c << ") = " << as.get(r,c) << endl;
		}

	IMTK::Volume<double>::SagitalSlice ss = array1.sagital(3);

	for(r=0; r < ss.nrows(); r++)
		for(c=0; c < ss.ncols(); c++){
			cout << "Sagital (" << r << "," << c << ") = " << ss.get(r,c) << endl;
		}

	IMTK::Volume<double>::CoronalSlice cs = array1.coronal(3);

	for(r=0; r < cs.nrows(); r++)
		for(c=0; c < cs.ncols(); c++){
			cout << "Coronal (" << r << "," << c << ") = " << cs.get(r,c) << endl;
		}
}




