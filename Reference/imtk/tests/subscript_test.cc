/********************************************************
File: subscript_test.cc
Abstract: Test file for subscript template. 
Created: 10/24/2000
Author: Chris L. Wyatt

Last Revision ($Revision: 1.3 $) 
by $Author: clwyatt $ 
on $Date: 2001/03/31 00:42:55 $
*********************************************************/
#include <iostream>

#include "Subscript.hh"

using namespace std;

int main(int argc, char **argv) {

	int e[4] = {1, 2, 3, 4};
	int index;
	
	IMTK::Subscript<1> *sub1;
	IMTK::Subscript<2> *sub2;
	IMTK::Subscript<3> *sub3;
	IMTK::Subscript<4> *sub4;
	IMTK::Subscript<4> subindex1;
	IMTK::Subscript<4> subindex2;
	
	sub1 = new IMTK::Subscript<1>(&e[0]);
	sub2 = new IMTK::Subscript<2>(&e[0]);
	sub3 = new IMTK::Subscript<3>(&e[0]);
	sub4 = new IMTK::Subscript<4>(&e[0]);
	
	cout << (*sub1)[0] << endl;
	cout << (*sub2)[0] << (*sub2)[1] << endl;
	cout << (*sub3)[0] << (*sub3)[1] << (*sub3)[2] << endl;
	cout << (*sub4)[0] << (*sub4)[1] << (*sub4)[2] << (*sub4)[3] << endl;
	
	(*sub1)[0] = 0;
	
	(*sub2)[0] = 0;
	(*sub2)[1] = 1;
	
	(*sub3)[0] = 0;
	(*sub3)[1] = 1;
	(*sub3)[2] = 2;
	
	(*sub4)[0] = 2;
	(*sub4)[1] = 4;
	(*sub4)[2] = 4;
	(*sub4)[3] = 5;
	
	//uncomment this to check assertion
	//unsigned int temp = (*sub1)[2];
	
	cout << (*sub1)[0] << endl;
	cout << (*sub2)[0] << (*sub2)[1] << endl;
	cout << (*sub3)[0] << (*sub3)[1] << (*sub3)[2] << endl;
	cout << (*sub4)[0] << (*sub4)[1] << (*sub4)[2] << (*sub4)[3] << endl;
	
	// test index
	subindex1[0] = 0;
	subindex1[1] = 2;
	subindex1[2] = 3;
	subindex1[3] = 4;
	
	index = subindex1.getIndex(*sub4);
	cout << subindex1[0] << " " << subindex1[1] << " " << subindex1[2] << " " << subindex1[3] << endl;
	cout << "INDEX = " << index << endl;
	subindex2.setSubscript(*sub4, index);
	cout << subindex2[0] << " " << subindex2[1] << " " << subindex2[2] << " " << subindex2[3] << endl;
	
	//test equality
	subindex2[0] = 0;
	subindex2[1] = 2;
	subindex2[2] = 3;
	subindex2[3] = 4;
	
	cout << "Comparison: " << (subindex1 == subindex2) << endl;
	
	subindex2[0] = 0;
	subindex2[1] = 0;
	subindex2[2] = 1;
	subindex2[3] = 1;
	
	cout << "Comparison: " << (subindex1 == subindex2) << endl;
	
}
