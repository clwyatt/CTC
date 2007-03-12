#ifndef _lab3rbtree_h

#define _lab3rbtree_h 1

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define BLK_COLOR	0
#define RED_COLOR	1

struct rbnode{
	double key;
        int x;
        int y;
        int z;
	int color;
	rbnode * childl;
	rbnode * childr;
	rbnode * parent;
	//struct heap_node *index2heap;
};


//*******************************************
//********** insert operations **************
//*******************************************

//function to search a binary tree
rbnode * BinTreeSearch(rbnode * &root, double x, int * found, rbnode * nil);

//function to insert node into binary tree
rbnode * BinTreeInsert(rbnode * &root, int xx, int yy, int zz,
                       int hr, int vr, rbnode * nil);

//function to insert node into red/black tree
void RBInsert(rbnode * &root, int xx, int yy, int zz, 
              int hr, int vr, rbnode * &node, rbnode * nil);

//function to restore red/black tree, left case
void RBInsCorrectL(rbnode * &root, rbnode * &cur, rbnode * p, rbnode * gp, rbnode * nil);

//function to restore red/black tree, right case
void RBInsCorrectR(rbnode * &root, rbnode * &cur, rbnode * p, rbnode * gp, rbnode * nil);

//function to rotate left
void RotateLeft(rbnode * &root, rbnode * &node, rbnode * nil);

//function to rotate right
void RotateRight(rbnode * &root, rbnode * &node, rbnode * nil);

//*******************************************
//********** delete operations **************
//*******************************************

//function to return inorder successor
rbnode * InOrderSucc(rbnode * root, rbnode * node, rbnode * nil);

//function to delete node from binary tree
int BinTreeDelete(rbnode * &root, rbnode * &node, rbnode * nil);

//function to delete node from red/black tree
void RBDelete(rbnode * &root, double x, rbnode * nil);

//function to restore red/black tree, left case
void RBDelCorrectL(rbnode * &root, rbnode * &cur, rbnode * p, rbnode * nil);

//function to restore red/black tree, right case
void RBDelCorrectR(rbnode * &root, rbnode * &cur, rbnode * p, rbnode * nil);

//*******************************************
//**********  misc operations  **************
//*******************************************

//function to do preorder travsersal, printing info
void preorder(rbnode * root, rbnode * nil);

double pack(int x, int y, int z, int hr, int vr);

#endif
