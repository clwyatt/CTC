/********************************************************
File: kdtree_test.cc
Abstract: Test file for K-D Tree template. 
Created: 05/12/1999
Author: Chris L. Wyatt

Last Revision ($Revision: 1.1.1.1 $) 
by $Author: hongli $ 
on $Date: 2002/12/06 21:49:27 $
*********************************************************/
#include <iostream>

#include "KDTree.hh"


using namespace std;

int main(int argv, char **argc)
{
	int i, length;
	input<Vector<float> > input;
	IMTK::Vector<float> result, *query;
	IMTK::KDTree<float> * testTree;
	float inc, x, y;

	testTree = new IMTK::KDTree<float>(20);

	query = new IMTK::Vector<float>(3);

	length = 1000;
	inc = 1.570796327/(float)length;

	input = new (IMTK::Vector<float>(3))[length];

	for(i=0; i < length; i++){
		x = 32 + 8*cos(inc*i);
		y = 32 + 8*sin(inc*i);
		(input + i)->setElement(0, x);
		(input + i)->setElement(1, y);
//		fprintf(stderr, "input ");
//		(input+i)->show();
	}

	fprintf(stderr, "Building k-d Tree\n");
	testTree->BuildTree(length, input);

	//inc = 8.0/(float)length;

	fprintf(stderr, "Searching k-d Tree\n");

		x = 32 + 4*cos(inc+13);
		y = 32 + 8*sin(inc*13);
		query->setPoint(x, y, 32);
		fprintf(stderr, "query ");
		query->show();
		result = testTree->KDTreeSearch(query);

		result.show();

//	for(i=0; i < length; i++){
//		x = 32 + 4*cos(inc+i);
//		y = 32 + 8*sin(inc*i);
//		query->setPoint(x, y, 32);
//		result = testTree->KDTreeSearch(query);
//		fprintf(stderr, "query ");
//		query->show();
//		fprintf(stderr, "result ");
//		result.show();
//	}

	delete testTree;
}


