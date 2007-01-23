/********************************************************
File: RBTree.hh
Abstract:  A template for a Red-Black tree.
Created: 02/08/1999
Author: Chris L. Wyatt

Last Revision ($Revision: 1.5 $) 
by $Author: clwyatt $ 
on $Date: 2001/09/25 17:51:56 $
*********************************************************/

//prevent recursive includes

#ifndef _RBTree_hh
#define _RBTree_hh 1

#ifdef IRIX
#include <assert.h>
#else
#include <cassert>
#endif

#include "Data.hh"

#define BLK_COLOR	0
#define RED_COLOR	1

/** Red-Black tree class template.

A base class template implementing a red-black tree.
A copy of T is made when inserted in the tree. This
copy is deleted when the node is deleted.

@author Chris L. Wyatt <cwyatt@wfubmc.edu>
@version $Revision: 1.5 $
*/

namespace IMTK{

template <class T>
class RBTree: public Data
{
public:
	/** Red-Black tree node.
	*/
	class Node
	{
	private:
		friend class RBTree<T>;
		int color;
		Node * childl;
		Node * childr;
		Node * parent;
		
	protected:
		T data;
		double key;

	public:
		
		/// Default Constructor
		Node(){
			color = BLK_COLOR;
			childl = NULL;
			childr = NULL;
			parent = NULL;
			key = 0;
		}
		
		/**
		 * Construct full Node
		 * 
		 * @param theData 
		 * @param theColor
		 * @param theLeftChild
		 * @param theRightChild
		 * @param theParent
		 * @param theKey
		 */
		Node(T theData, int theColor, Node * theLeftChild,
			   Node * theRightChild, Node * theParent,
			   double theKey){
			
			data = theData;
			color = theColor;
			childl = theLeftChild;
			childr = theRightChild;
			parent = theParent;
			key = theKey;
		}
		
		/// Copy Constructor
		Node(const Node & N){
			
			data = N.data;
			color = N.color;
			childl = N.childl;
			childr = N.childr;
			parent = N.parent;
			key = N.key;
		}
		
		/// Destructor
		~Node(){}	

		/// Return node data
		T * getData(){
			return &data;
		}

	};
	
private:
	Node * root;
	Node * nil;
	int numNodes;

protected:

	/**
	 * function to insert node into binary tree
	 * 
	 * @param theData object to insert
	 * @param theKey value of the associated key
	 */
	Node * binTreeInsert(T theData, double theKey){

		Node * nodepnt;
		Node * location;
		Node * node;
		int found;
		
		if (root == nil){ //if empty make root
			nodepnt = new Node(theData, BLK_COLOR, nil, nil, NULL, theKey);
			root=nodepnt;
			node = nodepnt;
		}
		else{ //insert
			location = binTreeSearch(theKey, &found);

			//Error: duplicate node in RBTree
			assert(found == 0);

			nodepnt = new Node(theData, BLK_COLOR, nil, nil, location, theKey);
			if( theKey < location->key )
			  location->childl = nodepnt;
			else
			  location->childr = nodepnt;
			node = nodepnt;
		}
		
		return node;
	}

	/**
	 * function to restore red/black tree, left case
	 * 
	 * @param cur pointer to current node
	 * @param p pointer to parent node
	 * @param gp pointer to grandparent node
	 */
	void insCorrectL(Node * &cur, Node * p, Node * gp){

		Node * uncle;

		uncle = gp->childr;
		
		if ( uncle->color == RED_COLOR ){ //case #1
			p->color = BLK_COLOR;
			gp->color = RED_COLOR;
			uncle->color = BLK_COLOR;
			cur = gp;  //move up 2 levels
		}
		else{
			if ( p->childr == cur ){ //case #2
				rotateLeft(p);
				cur = cur->childl;
			}

			//fall to case #3
			p->color = BLK_COLOR;
			gp->color = RED_COLOR;
			rotateRight(gp);
		}
	}

	/**
	 * function to restore red/black tree, right case
	 * 
	 * @param cur pointer to current node
	 * @param p pointer to parent node
	 * @param gp pointer to grandparent node 
	 */
	void insCorrectR(Node * &cur, Node * p, Node * gp){
 
		Node * uncle;
		int unclecolor;
		
		uncle = gp->childl;
		if ( uncle == nil )
			unclecolor = BLK_COLOR;
		else
			unclecolor = uncle->color;

		if ( unclecolor == RED_COLOR ){ //case #1
			p->color = BLK_COLOR;
			gp->color = RED_COLOR;
			uncle->color = BLK_COLOR;
			cur = gp; //move up 2 levels
		}
		else{
			if ( p->childl == cur ){ //case #2
				rotateRight(p);
				cur = cur->childr;
			}
			//fall to case #3
			p->color = BLK_COLOR;
			gp->color = RED_COLOR;
			rotateLeft(gp);
		}
	}

	/**
	 * function to rotate subtree left
	 * 
	 * @param node the node about which to rotate
	 */
	void rotateLeft(Node * &node){

		Node * a;
		Node * b;

		a= node->childr;
		b = node;

		if ( root == b )
			root = a;
		else{
			if ( b == ((b->parent)->childr) ) //node is right child
				(b->parent)->childr = a;
			else                              //node is left child
				(b->parent)->childl = a;
			a->parent = b->parent;
		}
  
		//move right child's left subtree to node's right subtree
		b->childr = a->childl;
		if ( a->childl != nil )
			(a->childl)->parent = b;

		//make node the left child of the former right child
		a->childl = b;
		b->parent = a;
		node = a;
	}

	/**
	 * function to rotate subtree right
	 * 
	 * @param node the node about which to rotate
	 */
	void rotateRight(Node * &node){

		Node * a;
		Node * b;

		a= node->childl;
		b= node;

		if ( root == b ){
			root = a;
			root->parent = NULL;
		}
		else{
			if ( b == ((b->parent)->childl) ) //node is left child
				(b->parent)->childl = a;
			else                              //node is right child
				(b->parent)->childr = a;
			a->parent = b->parent; 
		}
		
		//move left child's right subtree to node's left subtree
		b->childl = a->childr;
		if ( a->childr != nil )
			(a->childr)->parent = b;
		
		//make node the right child of the former left child
		a->childr = b;
		b->parent = a;
		node = a;
	}

	/**
	 * function to return inorder successor
	 * 
	 * @param node node to return inorder successor for
	 */
	Node * inOrderSucc(Node * node){

		Node * temp;

		temp = node->childr;
		while( temp->childl != nil )
			temp = temp->childl;
		return temp;
	}

	/**
	 * function to delete node from binary tree
	 * 
	 * @param node node to delete
	 */
	int binTreeDelete(Node * &node){

		Node * successor;
		Node * parent;
		Node * splicein;
		int deletecolor;

		if( (node->childr != nil) && (node->childl != nil) ){
			successor = inOrderSucc(node);
			//copy successor info to node, but not color
			node->key = successor->key;
			node->data = successor->data;
			node = successor;
		}

		parent = node->parent;

		if( node->childl != nil )
			splicein = node->childl;
		else
			splicein = node->childr;
		
		if( parent == NULL )
			root = splicein;
		else{
			if( parent->childl == node )
				parent->childl = splicein;
			else
				parent->childr = splicein;
			splicein->parent = parent;
		}
		
		deletecolor = node->color;

		//finally, deallocate node
		delete node;

		//reset node pointer to spliced in node
		node = splicein;

		return deletecolor;

	}

	/**
	 * function to restore red/black tree, left case
	 * 
	 * @param cur pointer to current node
	 * @param p pointer to parent node
	 */
	void delCorrectL(Node * &cur, Node * p){

		Node * sibling;
		Node * siblingL;
		Node * siblingR;
		Node * oldp = NULL;

		sibling = p->childr;
		siblingL = sibling->childl;
		siblingR = sibling->childr;

		//case #1, transform to case 2, 3, or 4
		if(sibling->color == RED_COLOR){
			sibling->color = BLK_COLOR;
			p->color = RED_COLOR; 
			oldp = p;
			rotateLeft(p);
			//update sibling and it's children
			sibling = oldp->childr;
			siblingL = sibling->childl;
			siblingR = sibling->childr;
			p = oldp;
		}
 
		//case 2
		if( (siblingL->color == BLK_COLOR) && (siblingR->color == BLK_COLOR) ){
			sibling->color = RED_COLOR;
			cur = p;
		}

		//case 3 and/or 4
		else{
			//case 3, transform to case 4
			if(siblingR->color == BLK_COLOR){
				sibling->color = RED_COLOR;
				siblingL->color = BLK_COLOR;
				rotateRight(sibling);
				//update sibling and it's children
				sibling = p->childr;
				siblingL = sibling->childl;
				siblingR = sibling->childr;
			}
			//case 4
			siblingR->color = BLK_COLOR;
			sibling->color = p->color;
			p->color = BLK_COLOR;
			rotateLeft(p);
			cur = root;
		}

	}

	/**
	 * function to restore red/black tree, right case
	 * 
	 * @param cur pointer to current node
	 * @param p pointer to parent node
	 */
	void delCorrectR(Node * &cur, Node * p){

		Node * sibling;
		Node * siblingL;
		Node * siblingR;
		Node * oldp = NULL;

		sibling = p->childl;
		siblingL = sibling->childl;
		siblingR = sibling->childr;

		//case #1, transform to case 2, 3, or 4
		if(sibling->color == RED_COLOR){
			sibling->color = BLK_COLOR;
			oldp = p;
			p->color = RED_COLOR; 
			rotateRight(p);
			//update sibling and it's children
			sibling = oldp->childl;
			siblingL = sibling->childl;
			siblingR = sibling->childr;
			p = oldp;
		}
		//case 2
		if( (siblingL->color == BLK_COLOR) && (siblingR->color == BLK_COLOR) ){
			sibling->color = RED_COLOR;
			cur = p;
		}
		//case 3 and/or 4
		else{
			//case 3, transform to case 4
			if(siblingL->color == BLK_COLOR){
				sibling->color = RED_COLOR;
				siblingR->color = BLK_COLOR;
				rotateLeft(sibling);
				//update sibling and it's children
				sibling = p->childl;
				siblingL = sibling->childl;
				siblingR = sibling->childr;
			}
			//case 4
			siblingL->color = BLK_COLOR;
			sibling->color = p->color;
			p->color = BLK_COLOR;
			rotateRight(p);
			cur = root;
		}
	}

	/**
	 * function to do preorder travsersal, add to list
	 *
	 * @param subRoot root of subtree for recursive traversal
	 * @param list pointer to list of type <T>
	 * @param entry integer representing current entry into list
	 */
	void preorder(Node * subRoot, T * list, int *entry){

		Node * curr_node;
		Node * parent;
		int count;

		if(subRoot != nil){
			preorder(subRoot->childl, list, entry);
			preorder(subRoot->childr, list, entry);
			*(list + *entry) = subRoot->data;

			*entry += 1;
		}
	}

	/**
	 * function to do preorder travsersal, add to list
	 *
	 * @param subRoot root of subtree for recursive traversal
	 * @param list pointer to list of BTreeNode
	 * @param entry integer representing current entry into list
	 */
	void preorder2(Node * subRoot, Node * list, int *entry){

		Node * curr_node;
		Node * parent;
		int count;

		if(subRoot != nil){
			preorder2(subRoot->childl, list, entry);
			preorder2(subRoot->childr, list, entry);
			*(list + *entry) = *subRoot;

			*entry += 1;
		}
	}

public:
	/// Default Constructor
	RBTree(){
		nil = new Node;
		nil->color = BLK_COLOR;
		root = nil;
		numNodes = 0;
	}

	/// Copy Constructor
	RBTree(const RBTree & RBT){

		//RBTree(const RBTree & T) not implemented
		assert(0);
	}

	/// Destructor
	~RBTree(){

		// delete all nodes
		for(int i=0; i < numNodes; i ++)
			this->deleteNode(root->key);

		// delete nil node
		delete nil;
	}

	/**
	 * function to search a binary tree
	 * 
	 * @param theKey node to search for
	 * @param found indicates success or failure
	 */
	Node * binTreeSearch(double theKey, int * found){

		Node * location;
		Node * current;
		
		*found = 0;
		location = nil;
		current = root;
		while( (current != nil) && (*found == 0) ){
			location = current;
			if ( theKey == current->key )
				*found = 1;
			else{
				if ( theKey < current->key )
					current = current->childl;
				else
					current = current->childr;
			}
		}

		return location;
	}

	/**
	 * function to insert node into red/black tree
	 * 
	 * @param theData the object to insert
	 * @param theKey the associated key value
	 */
	void insertNode(T theData, double theKey){

		Node * current = NULL;
		Node * p = NULL;
		Node * gp = NULL;
		Node * node = NULL;

		//insert as with normal binary tree
		node = binTreeInsert(theData, theKey);

		node->color = RED_COLOR; //color red
		current = node;
		p = current->parent;
		if(p != NULL) //if not root set grandparent
			gp = p->parent;
		
		while( (current != root) && ((current->parent)->color == RED_COLOR) && (gp != NULL) ){
			p = current->parent;
			gp = p->parent;
			
			if (gp->childl == p)
				insCorrectL(current, p, gp);
			else
				insCorrectR(current, p, gp);
		}

		root->color = BLK_COLOR;
		root->parent = NULL;

		numNodes += 1;
	}

	/**
	 * function to delete node from red/black tree
	 * 
	 * @param theKey the key value of the node to delete
	 */
	void deleteNode(double theKey){

		Node * current;
		Node * p;
		Node * node;
		int delcolor, found;

		node = binTreeSearch(theKey, &found);

		if(found == 1){ //check node is in rbtree
			delcolor = binTreeDelete(node);
			current = node;
			if( delcolor == BLK_COLOR ){
				while( (current != root) && (current->color != RED_COLOR) ){
					p = current->parent;
					if( p->childl == current )    //left case
						delCorrectL(current, p);
					else                          //right case
						delCorrectR(current, p);
				}
				current->color = BLK_COLOR;
			}
			root->color = BLK_COLOR;
			root->parent = NULL;
		}

		numNodes -= 1;
	}

	/**
	 * return the data at the root of the tree
	 *
	 */
	T getRoot(){
		return root->data;
	}

	/**
	 * return a pointer to the data of type <T> with given key
	 *
	 * @param theKey the key to search for
	 */
	T * getNode(double theKey){

		int found;
		Node *pntr;

		pntr = binTreeSearch(theKey, &found);

		// Error: no such node in RBTree
		assert(found > 0);

		return &(pntr->data);
	}

	/**
	 * return int indicating if tree is empty
	 *
	 */
	int isEmpty(){
		int e;

		if(root == nil) e = 1;
		else e = 0;

		return e;
	}

	/**
	 * get array of nodes in the tree
	 *
	 */
	T * getList(int *size){

		T * list;
		int entry = 0;

		*size = numNodes;
		list = new T[*size];

		preorder(root, list, &entry);

		return list;
	}



	/**
	 * create a copy of the tree
	 *
	 */
	RBTree<T> * clone(){

		RBTree<T> * copy;
		Node * list;
		int entry, i;

		entry = 0;

		list = new Node[numNodes];

		preorder2(root, list, &entry);

		copy = new RBTree<T>;
		for(i=0; i < numNodes; i++)
			copy->insertNode((list+i)->data, (list+i)->key);

		delete [] list;

		return copy;

	}
};
}

#endif
