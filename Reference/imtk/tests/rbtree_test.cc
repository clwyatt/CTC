/********************************************************
File: rbtree_test.cc
Abstract: Test file for Red-Black Tree template. 
Created: 11/15/2000
Author: Chris L. Wyatt

Last Revision ($Revision: 1.3 $) 
by $Author: clwyatt $ 
on $Date: 2001/03/31 00:42:55 $
*********************************************************/
#include <iostream>

#include "RBTree.hh"

using namespace std;

int main(int argv, char **argc)
{
	IMTK::RBTree<int> *RBT;
	int data = 100;
	int *ip;
	double key = 100;

	RBT = new IMTK::RBTree<int>();

	for(int i=0; i < 100; i++){
		data = i;
		key = double(i);
		RBT->insertNode(data, key);
	}
	
	ip = RBT->getNode(90);

	cout << "Node with key 90 is " << *ip << endl;

	// test more.......
}
   
