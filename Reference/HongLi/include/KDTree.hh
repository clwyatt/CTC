/********************************************************
File: KDTree.hh
Abstract:  A template for a KD tree.
Created: 05/12/1999
Author: Chris L. Wyatt

Last Revision ($Revision: 1.1.1.1 $) 
by $Author: hongli $ 
on $Date: 2002/12/06 21:49:28 $
*********************************************************/

//prevent recursive includes

#ifndef _KDTree_hh
#define _KDTree_hh 1

#ifdef IRIX
#include <assert.h>
#else
#include <cassert>
#endif

#include <slist>

#include "Data.hh"
#include "Vector.hh"

#define D_MAX (T)(1000000)

/** K-D tree class template.

A base class template implementing a N dimensional k-d tree.
The precision type (int, float, etc) and the dimension (1, 2, ...)
are specified as template parameters.

Implementation based on:
J. H. Friedman, J. L. Bentley, R. A. Finkel,
"An Algorithm for Finding Best Matches in Logarithmic Expected Time",
ACM Trans. on Mathmatical Software, Vol. 3, No. 3, pgs. 209-226, Sept. 1977.

The k-d tree is constructed slightly different from most trees for performance reasons.
The entire contents of the tree are passed to the buildTree function.

@author Chris L. Wyatt <cwyatt@wfubmc.edu>
@version $Revision: 1.1.1.1 $
*/

namespace IMTK {

template <class T, int N>
class KDTree: public Data
{
	class Node
	{
	private:
		friend class KDTree<T>;
		int key;
		double median;
		bool isTerminal;
		std::slist< Vector<T> > bucketItems;
		Node * childl;
		Node * childr;

	protected:

	public:

		/// Default Constructor
		Node(){
			key = 0;
			median = 0;
			isTerminal = false;
			childl = NULL;
			childr = NULL;
		}
		
		/**
		 * Construct full Node
		 * 
		 * @param theKey
		 * @param theMedian
		 * @param theIsTermial
		 * @param theBucketItems
		 * @param theLeftChild
		 * @param theRightChild
		 */
		Node(int theKey, double theMedian, bool theIsTerminal,
			   const std::slist<Vector<T> > &theBucketItems,
			   Node * theLeftChild, Node * theRightChild){
			key = theKey;
			median = theMedian;
			isTerminal = theIsTerminal;
			bucketItems = theBucketItems;
			childl = theLeftChild;
			childr = theRightChild;
		}
		
		/// Copy Constructor
		Node(const Node & N){
			key = N.key;
			median = N.median;
			isTerminal = N.isTerminal;
			bucketItems = N.bucketItems;
		}
		
		/// Destructor
		~Node(){

		}
	};
		
private:
	Node<T> * root;
	int minBucketSize;
	double Bplus[N], Bminus[N]; //bounds
	double PDQ; //min dist encountered so far when searching
	Vector<T> PDR; //point coresponding to PDQ
	int done; //flag indicating if search is complete
	
protected:
	/**
	 * Recursive function to perform quick-sort
	 * 
	 * @param input
	 * @param low
	 * @param high
	 * @param max
	 */
	void qsort(T * input, int low, int high, int max){
		int position;

		if (high > low){
			RandP(input, low, high, max, &position);

			qsort(input, low, (position-1), max);
			qsort(input, (position+1), high, max);
		}
	}

	/**
	 * function used by qsort to rearrange and place sublists
	 * 
	 * @param input
	 * @param low
	 * @param high
	 * @param max
	 * @param position
	 */
	void RandP(T * input, int low, int high, int max, int * position){
		int mr, ml; 
		T x, temp;

		//initialize
		mr= low;
		ml= high+1;
		x= input[low];

		//loop shifting mr and ml until mr > ml
		while (mr < ml){
			do{ //shift mr
				mr+=1;
			}
			while(get(input, high, mr) < x);
			do{ //shift ml
				ml-=1;
			}
			while(get(input, high, ml) > x);

			if (mr < ml){ //swap places
				temp = input[mr];
				input[mr]= input[ml];
				input[ml]= temp;
			}
		}
		*position = ml;
		input[low] = input[*position];
		input[*position] = x;
	}

	/**
	 * funtion used by RandP to get value from list including high + 1=INT_MAX
	 * 
	 * @param in
	 * @param high
	 * @param index
	 */
	T get(T * in, int high, int index){
  
		if(index <= high)
			return *(in+index);
		else
			return D_MAX;
	}


	/**
	 * function to estimate the spread for jth axis from list
	 * 
	 * @param theAxis the axis to use
	 * @param theSize the length of the input list
	 * @param theList the input
	 */
	double Spread(int theAxis, int theSize, Vector<T> * theList){
		double temp, min, max, spread;
		int i;

		if(theAxis == 1) min = (double)(theList->getElement(0));
		if(theAxis == 2) min = (double)(theList->getElement(1));
		if(theAxis == 3) min = (double)(theList->getElement(2));
		max = min;

		//fprintf(stderr, "The size is %i\n", theSize);

		for(i=1; i< theSize; i++){
			if(theAxis == 1) temp = (double)((theList+i)->getElement(0));
			if(theAxis == 2) temp = (double)((theList+i)->getElement(1));
			if(theAxis == 3) temp = (double)((theList+i)->getElement(2));

			if(temp < min) min = temp;
			if(temp > max) max = temp;
		}
	       
		spread = fabs(max - min);

		return spread;
	}

	/**
	 * function to compute the median of the jth axis from list
	 * (for now I use the mean since it is easier to compute)
	 * 
	 * @param theAxis the axis to use
	 * @param theSize the length of the input list
	 * @param theList the input
	 */
	double Median(int theAxis, int theSize, Vector<T> * theList){
		int i;
		T * list;
		T med1, med2;
		double med;

		list = new T[theSize];
		for(i=0; i< theSize; i++){
			if(theAxis == 1) *(list + i) = (theList+i)->getElement(0);
			if(theAxis == 2) *(list + i) = (theList+i)->getElement(1);
			if(theAxis == 3) *(list + i) = (theList+i)->getElement(2);
		}
	       
		// sort the list
		qsort(list, 0, (theSize-1), theSize);

		// choose median
		if(fmod((double)theSize, 2.0) == 0.0){ //even
			i = (int)((float)(theSize)/2);
			med1 = *(list + i);
			med2 = *(list + i + 1);
			med = ((double)(med1 + med2))/2.0;
		}
		else{
			i = (int)((float)(theSize)/2) + 1;
			med = (double)(*(list + i));
		}

		delete [] list;

		return med;
	}

	/**
	 * function to compute the left sublist from list
	 * 
	 * @param theAxis the axis to use
	 * @param theMed the seperator value
	 * @param theSize the length of the input list
	 * @param theList the input
	 * @param newSize the size of the sublist
	 */
	Vector<T> * LeftSubList(int theAxis, double theMed, int theSize, Vector<T> * theList, int * newSize){
		int i, j;
		Vector<T> * newList;

		*newSize = 0;

		for(i=0; i< theSize; i++){
			if(theAxis == 1){
				if((double)((theList+i)->getElement(0)) <= theMed) *newSize += 1;
			}
			if(theAxis == 2){
				if((double)((theList+i)->getElement(1)) <= theMed) *newSize += 1;
			}
			if(theAxis == 3){
				if((double)((theList+i)->getElement(2)) <= theMed) *newSize += 1;
			}
		}
	       
		newList = new Vector<T>[*newSize];
		j = 0;
		for(i=0; i< theSize; i++){
			if(theAxis == 1){
				if((double)((theList+i)->getElement(0)) <= theMed){
					*(newList + j) = *(theList + i);
					j += 1;
				}
			}
			if(theAxis == 2){
				if((double)((theList+i)->getElement(1)) <= theMed){
					*(newList + j) = *(theList + i);
					j += 1;
				}
			}
			if(theAxis == 3){
				if((double)((theList+i)->getElement(2)) <= theMed){
					*(newList + j) = *(theList + i);
					j += 1;
				}
			}
		}

		return newList;
	}

	/**
	 * function to compute the right sublist from list
	 * 
	 * @param theAxis the axis to use
	 * @param theMed the seperator value
	 * @param theSize the length of the input list
	 * @param theList the input
	 * @param newSize the size of the sublist
	 */
	Vector<T> * RightSubList(int theAxis, double theMed, int theSize, Vector<T> * theList, int * newSize){
		int i, j;
		Vector<T> * newList;

		*newSize = 0;

		for(i=0; i< theSize; i++){
			if(theAxis == 1){
				if((double)((theList+i)->getElement(0)) > theMed) *newSize += 1;
			}
			if(theAxis == 2){
				if((double)((theList+i)->getElement(1)) > theMed) *newSize += 1;
			}
			if(theAxis == 3){
				if((double)((theList+i)->getElement(2)) > theMed) *newSize += 1;
			}
		}
	       
		newList = new Vector<T>[*newSize];
		j = 0;
		for(i=0; i< theSize; i++){
			if(theAxis == 1){
				if((double)((theList+i)->getElement(0)) > theMed){
					*(newList + j) = *(theList + i);
					j += 1;
				}
			}
			if(theAxis == 2){
				if((double)((theList+i)->getElement(1)) > theMed){
					*(newList + j) = *(theList + i);
					j += 1;
				}
			}
			if(theAxis == 3){
				if((double)((theList+i)->getElement(2)) > theMed){
					*(newList + j) = *(theList + i);
					j += 1;
				}
			}
		}

		return newList;
	}

	/**
	 * recursive function to delete k-d tree nodes
	 * 
	 * @param currNode the current k-d tree node
	 */
	void KDTreeDeleteNode(Node<T> * currNode){

		if(currNode->childl != NULL)
			KDTreeDeleteNode(currNode->childl);
		if(currNode->childr != NULL)
			KDTreeDeleteNode(currNode->childr);

		delete currNode->childl;
		delete currNode->childr;
	}

public:
	/// Default Constructor
	KDTree(){
		root = NULL;
		minBucketSize = 5;
	}

	/**
	 * Construct KDTree with given minimum bucket length
	 * 
	 * @param theMinBucketSize
	 */
	KDTree(int theMinBucketSize){
		root = NULL;
		minBucketSize = theMinBucketSize;
	}

	/// Copy Constructor
	KDTree(const KDTree & KDT){
		fprintf(stderr, "ERROR: function KDTree(const RBTree & T) not implemented.");
		exit(1);
	}

	/// Destructor
	~KDTree(){
		//resursively delete starting at the root
		KDTreeDeleteNode(root);

		//delete the root
		delete root;
	}

	/**
	 * Recursive function build k-d tree from list
	 * Note: the input point list is deleted internally
	 * so make a copy first if you need it. 
	 * (This is NOT the best way, but I'll leave it for now)
	 * 
	 * @param theSize the length of the input list
	 * @param theList the input
	 */
	Node<T> * BuildTree(int theSize, Vector<T> * theList){
		double sx, sy, sz, maxSpread, med;
		int idx, theLeftSize, theRightSize;
		Node<T> * currentNode;
		Vector<T> * theLeftList, * theRightList;

		//fprintf(stderr, "Calling BuildTube with size %i\n", theSize);

		if( theSize <= minBucketSize){
			currentNode = new Node<T>(0, 0, 1, theSize, theList, NULL, NULL);
			//fprintf(stderr, "Terminal Node\n");
			//for(idx=0; idx<theSize; idx++){
			//	(theList + idx)->show();
			//}
		}
		else{
			// determine axis with the largest spread
			sx = Spread(1, theSize, theList);
			maxSpread = sx;
			idx = 1;
			sy = Spread(2, theSize, theList);
			if( sy > maxSpread ){
				maxSpread = sy;
				idx = 2;
			}
			sz = Spread(3, theSize, theList);
			if( sz > maxSpread ){
				maxSpread = sz;
				idx = 3;
			}
			// compute median of axis with largest spread
			med = Median(idx, theSize, theList);

			// form sublists using median as seperator
			theLeftList = LeftSubList(idx, med, theSize, theList, &theLeftSize);
			theRightList = RightSubList(idx, med, theSize, theList, &theRightSize);

			//de-allocate space 
			delete [] theList;

			currentNode = new Node<T>(idx, med, 0, 0, NULL, NULL, NULL);
			//fprintf(stderr, "Splitting left %i, %f\n", idx, med);
			currentNode->childl = BuildTree(theLeftSize, theLeftList);
			//fprintf(stderr, "Splitting right %i, %f\n", idx, med);
			currentNode->childr = BuildTree(theRightSize, theRightList);
		}
		
		root = currentNode;

		return currentNode;
	}

	/**
	 * function to search a k-d tree for nearest neighbor
	 * 
	 * @param queryNode the point to find nearest neighbor of
	 */
	Vector<T> KDTreeSearch(Vector<T> * queryNode){
		T coords[3];

		coords[0] = queryNode->getElement(0);
		coords[1] = queryNode->getElement(1);
		coords[2] = queryNode->getElement(2);

		Bplus[0] = 10000000;
		Bplus[1] = 10000000;
		Bplus[2] = 10000000;
		Bminus[0] = -1000000;
		Bminus[1] = -1000000;
		Bminus[2] = -1000000;

		PDQ = 10000000;
		done = 0;

		KDTreeSearchRec(root, coords);

		// closest Point stored in PDR
		return PDR;
	}

	/**
	 * Recursive function to search a k-d tree for nearest neighbor
	 * 
	 * @param subRoot the subroot of the search tree
	 * @param qcoords the point to find nearest neighbor of
	 */
	void KDTreeSearchRec(Node<T> * subRoot, T qcoords[3]){
		int d, i;
		double p, temp;

		if(done == 0){
			if(subRoot->isTerminal == 1){
				for(i=0; i < subRoot->bucketSize; i++){
					temp = dist(qcoords, subRoot->bucketItems + i);
 					if(temp < PDQ){
						PDQ = temp;
						PDR = *(subRoot->bucketItems + i);
					}
				}
				if(BallWithinBounds(qcoords)) done = 1;
			}
			else{
				d = subRoot->key;
				p = subRoot->median;
				if(qcoords[d-1] <= p){
					temp = Bplus[d-1];
					Bplus[d-1] = p;
					//fprintf(stderr, "Searching1 left %i, %f, [%f,%f]\n", d, p, Bminus[d-1],Bplus[d-1]);
					KDTreeSearchRec(subRoot->childl, qcoords);
					Bplus[d-1] = temp;
				}
				else{
					temp = Bminus[d-1];
					Bminus[d-1] = p;
					//fprintf(stderr, "Searching1 right %i, %f, [%f,%f]\n", d, p, Bminus[d-1],Bplus[d-1]);
					KDTreeSearchRec(subRoot->childr, qcoords);
					Bminus[d-1] = temp;
				}
				
				if(qcoords[d-1] <= p){
					temp = Bminus[d-1];
					Bminus[d-1] = p;
					if(BoundsOverlapBall(qcoords)){
						//fprintf(stderr, "Searching2 right %i, %f, [%f,%f]\n", d, p, Bminus[d-1],Bplus[d-1]);
						KDTreeSearchRec(subRoot->childr, qcoords);
					}
					Bminus[d-1] = temp;
				}
				else{
					temp = Bplus[d-1];
					Bplus[d-1] = p;
					if(BoundsOverlapBall(qcoords)){
						//fprintf(stderr, "Searching2 left %i, %f, [%f,%f]\n", d, p, Bminus[d-1],Bplus[d-1]);
						KDTreeSearchRec(subRoot->childl, qcoords);
					}
					Bplus[d-1] = temp;
				}

				if(BallWithinBounds(qcoords)) done = 1;
			}
		}
	}

	/**
	 * function to determine if distance from query record to key is
	 * less than radius of ball
	 * 
	 * @param qcoords the point to find nearest neighbor of
	 */
	int BallWithinBounds(T coords[3]){
		int d, result;

		result = 1; //true

		for(d=0; d < 3; d++){
			if( (cdist(d, qcoords[d], Bminus[d]) <= PDQ) ||
			    (cdist(d, qcoords[d], Bplus[d]) <= PDQ) ){
				result = 0;
				break;
			}
		}
	       
		return result;
	}


	/**
	 * function to determine if bounds overlap ball
	 * 
	 * @param qcoords the point to find nearest neighbor of
	 */
	int BoundsOverlapBall(T qcoords[3]){
		int d, result;
		double sum, temp;

		sum = 0;
		result = 0;

		for(d=0; d < 3; d++){
			if(qcoords[d] <= Bminus[d]){
				temp = cdist(d, qcoords[d], Bminus[d]);
				sum = sum + temp*temp;
				if(dissim(sum) < PDQ){
					result = 1;
					break;
				}
			}
			else if(qcoords[d] >= Bplus[d]){
				temp = cdist(d, qcoords[d], Bplus[d]);
				sum = sum + temp*temp;
				if(dissim(sum) < PDQ){
					result = 1;
					break;
				}
			}
// 			else{
// 				result = 1;
// 				break;
// 			}
		}

		return result;
	}

	/**
	 * function to compute coordinate distances
	 * 
	 * @param i index
	 * @param X coordinate
	 * @param B bound
	 */
	double cdist(int i, T X, double B){
		double temp;

		temp = fabs((double(X) - B)); //*(double(X) - B));

		return temp;
	}

	/**
	 * function to compute dissimularity measure
	 * 
	 * @param arg argument of dissem function
	 */
	double dissim(double arg){

		return sqrt(arg);
	}

	/**
	 * function to compute Euclidean dist
	 * 
	 * @param point1
	 * @param point2
	 */
	double dist(T point1[3], Vector<T> * point2){
		double x, y, z, dx, dy, dz;

		x = (double)point2->getElement(0);
		y = (double)point2->getElement(1);
		z = (double)point2->getElement(2);

		dx = x - (double)(point1[0]);
		dy = y - (double)(point1[1]);
		dz = z - (double)(point1[2]);

		return sqrt(dx*dx + dy*dy + dz*dz);
	}
};
}

#endif
