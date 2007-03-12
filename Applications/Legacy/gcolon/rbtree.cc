// File: rbtree.cc
// Created: 10/17/97
// Last Modified:10/26/97 clw
//
// Implements: red/black tree operations                
//
#include "rbtree.h"

//*******************************************
//********** insert operations **************
//*******************************************

//function to search a binary tree
rbnode * BinTreeSearch(rbnode * &root, double x, int * found, rbnode * nil)
{
  rbnode * location;
  rbnode * current;

  *found = 0;
  location = nil;
  current = root;
  while( (current != nil) && (*found == 0) )
   {
     location = current;
     if ( x == current->key )
       *found = 1;
     else
      {
       if ( x < current->key )
         current = current->childl;
       else
         current = current->childr;
      }
   }

  return location;
}

//function to insert node into binary tree
rbnode * BinTreeInsert(rbnode * &root, int xx, int yy, int zz,
                       int hr, int vr, rbnode * nil)
{
  rbnode * nodepnt; 
  rbnode * location;
  rbnode * node;
  int found;
  double x;

  x = pack(xx, yy, zz, hr, vr);

  if (root == nil) //if empty make root
   {
    nodepnt = new rbnode;
    nodepnt->key = x;
    nodepnt->x = xx;
    nodepnt->y = yy;
    nodepnt->z = zz;
    nodepnt->parent = NULL;
    nodepnt->childl = nil;
    nodepnt->childr = nil;
    root=nodepnt;
    node = nodepnt;
   }
  else //insert
   {
    location = BinTreeSearch(root, x, &found, nil);
    if (found == 0)
     {
      nodepnt = new rbnode;
      nodepnt->key = x;
      nodepnt->x = xx;
      nodepnt->y = yy;
      nodepnt->z = zz;
      nodepnt->parent = location;
      nodepnt->childl = nil;
      nodepnt->childr = nil;
      if( x < location->key )
        location->childl = nodepnt;
      else
        location->childr = nodepnt;
      node = nodepnt;
     }
    else
     {
      fprintf(stderr, "Error: duplicate node in rbtree\n");
      exit(1);
     }
   }
  return node;
}
    

//function to insert node into red/black tree
void RBInsert(rbnode * &root, int xx,int yy, int zz, 
              int hr, int vr, rbnode * &node, rbnode * nil)
{
  rbnode * current;
  rbnode * p;
  rbnode * gp;

  //insert as with normal binary tree
  node = BinTreeInsert(root, xx, yy, zz, hr, vr, nil);

  node->color = RED_COLOR; //color red
  current = node;
  p = current->parent;
  if(p != NULL) //if not root set grandparent
    gp = p->parent;

  while( (current != root) && ((current->parent)->color == RED_COLOR) && (gp != NULL) )
   {
    p = current->parent;
    gp = p->parent;

    if (gp->childl == p)
      RBInsCorrectL(root, current, p, gp, nil);
    else
      RBInsCorrectR(root, current, p, gp, nil);
   }

  root->color = BLK_COLOR;
  root->parent = NULL;

}

//function to restore red/black tree, left case
void RBInsCorrectL(rbnode * &root, rbnode * &cur, rbnode * p, rbnode * gp, rbnode * nil)
{
  rbnode * uncle;
 // int unclecolor;

  uncle = gp->childr;
//  if ( uncle == nil )
//    unclecolor = BLK_COLOR;
//  else
//    unclecolor = uncle->color;

  if ( uncle->color == RED_COLOR ) //case #1
   {
    p->color = BLK_COLOR;
    gp->color = RED_COLOR;
    uncle->color = BLK_COLOR;
    cur = gp;  //move up 2 levels
   }
  else
   {
    if ( p->childr == cur ) //case #2
     {
      RotateLeft(root, p, nil);
      cur = cur->childl;
     }
                           //fall to case #3
    p->color = BLK_COLOR;
    gp->color = RED_COLOR;
    RotateRight(root, gp, nil);
   }
}

//function to restore red/black tree, right case
void RBInsCorrectR(rbnode * &root, rbnode * &cur, rbnode * p, rbnode * gp, rbnode * nil)
{
  rbnode * uncle;
  int unclecolor;

  uncle = gp->childl;
  if ( uncle == nil )
    unclecolor = BLK_COLOR;
  else
    unclecolor = uncle->color;

  if ( unclecolor == RED_COLOR ) //case #1
   {
    p->color = BLK_COLOR;
    gp->color = RED_COLOR;
    uncle->color = BLK_COLOR;
    cur = gp; //move up 2 levels
   }
  else
   {
    if ( p->childl == cur ) //case #2
     {
      RotateRight(root, p, nil);
      cur = cur->childr;
     }
                           //fall to case #3
    p->color = BLK_COLOR;
    gp->color = RED_COLOR;
    RotateLeft(root, gp, nil);
   }
}

//function to rotate left
void RotateLeft(rbnode * &root, rbnode * &node, rbnode * nil)
{
  rbnode * a;
  rbnode * b;

  a= node->childr;
  b = node;

  if ( root == b )
    root = a;
  else
   {
    if ( b == ((b->parent)->childr) ) //node is right child
      (b->parent)->childr = a;
    else                                    //node is left child
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

//function to rotate right
void RotateRight(rbnode * &root, rbnode * &node, rbnode * nil)
{
  rbnode * a;
  rbnode * b;

  a= node->childl;
  b= node;

  if ( root == b )
   {
    root = a;
    root->parent = NULL;
   }
  else
   {
    if ( b == ((b->parent)->childl) ) //node is left child
      (b->parent)->childl = a;
    else                                    //node is right child
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
//*******************************************
//********** delete operations **************
//*******************************************

//function to return inorder successor
rbnode * InOrderSucc(rbnode * root, rbnode * node, rbnode * nil)
{
 rbnode * temp;
 temp = node->childr;
 while( temp->childl != nil )
   temp = temp->childl;
 return temp;
}

//function to delete node from binary tree
int BinTreeDelete(rbnode * &root, rbnode * &node, rbnode * nil)
{
  rbnode * successor;
  rbnode * parent;
  rbnode * splicein;
  int deletecolor;

  if( (node->childr != nil) && (node->childl != nil) )
   {
    successor = InOrderSucc(root, node, nil);
    //copy successor info to node, but not color
    node->key = successor->key;
    node->x = successor->x;
    node->y = successor->y;
    node->z = successor->z;

    node = successor;
   }

  parent = node->parent;

  if( node->childl != nil )
    splicein = node->childl;
  else
    splicein = node->childr;

  if( parent == NULL )
    root = splicein;
  else
   {
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

//function to delete node from red/black tree
void RBDelete(rbnode * &root, double x, rbnode * nil)
{
  rbnode * current;
  rbnode * p;
  rbnode * node;
  int delcolor, found;
  //double x;

  //x = (bnode->board)->alias;
  node = BinTreeSearch(root, x, &found, nil);

  if(found == 1) //check node is in rbtree
   {
    delcolor = BinTreeDelete(root, node, nil);
    current = node;
    if( delcolor == BLK_COLOR )
     {
      while( (current != root) && (current->color != RED_COLOR) )
       {
        p = current->parent;
        if( p->childl == current )    //left case
          RBDelCorrectL(root, current, p, nil);
        else                               //right case
          RBDelCorrectR(root, current, p, nil);
       }
      current->color = BLK_COLOR;
     }
    root->color = BLK_COLOR;
    root->parent = NULL;

   }
}

//function to restore red/black tree, left case
void RBDelCorrectL(rbnode * &root, rbnode * &cur, rbnode * p, rbnode * nil)
{
  rbnode * sibling;
  rbnode * siblingL;
  rbnode * siblingR;
  rbnode * oldp = NULL;

  sibling = p->childr;
  siblingL = sibling->childl;
  siblingR = sibling->childr;

  //case #1, transform to case 2, 3, or 4
  if(sibling->color == RED_COLOR)
   {
    sibling->color = BLK_COLOR;
    p->color = RED_COLOR; 
    oldp = p;
    RotateLeft(root, p, nil);
    //update sibling and it's children
    sibling = oldp->childr;
    siblingL = sibling->childl;
    siblingR = sibling->childr;
    p = oldp;
   }
 
  //case 2
  if( (siblingL->color == BLK_COLOR) && (siblingR->color == BLK_COLOR) )
   {
    sibling->color = RED_COLOR;
    cur = p;
   }

  //case 3 and/or 4
  else
   {
    //case 3, transform to case 4
    if(siblingR->color == BLK_COLOR)
     {
      sibling->color = RED_COLOR;
      siblingL->color = BLK_COLOR;
      RotateRight(root, sibling, nil);
      //update sibling and it's children
      sibling = p->childr;
      siblingL = sibling->childl;
      siblingR = sibling->childr;
     }
    //case 4
    siblingR->color = BLK_COLOR;
    sibling->color = p->color;
    p->color = BLK_COLOR;
    RotateLeft(root, p, nil);
    cur = root;
   }

}

//function to restore red/black tree, right case
void RBDelCorrectR(rbnode * &root, rbnode * &cur, rbnode * p, rbnode * nil)
{
  rbnode * sibling;
  rbnode * siblingL;
  rbnode * siblingR;
  rbnode * oldp = NULL;

  sibling = p->childl;
  siblingL = sibling->childl;
  siblingR = sibling->childr;

  //case #1, transform to case 2, 3, or 4
  if(sibling->color == RED_COLOR)
   {
    sibling->color = BLK_COLOR;
    oldp = p;
    p->color = RED_COLOR; 
    RotateRight(root, p, nil);
    //update sibling and it's children
    sibling = oldp->childl;
    siblingL = sibling->childl;
    siblingR = sibling->childr;
    p = oldp;
   }
  //case 2
  if( (siblingL->color == BLK_COLOR) && (siblingR->color == BLK_COLOR) )
   {
    sibling->color = RED_COLOR;
    cur = p;
   }
  //case 3 and/or 4
  else
   {
    //case 3, transform to case 4
    if(siblingL->color == BLK_COLOR)
     {
      sibling->color = RED_COLOR;
      siblingR->color = BLK_COLOR;
      RotateLeft(root, sibling, nil);
      //update sibling and it's children
      sibling = p->childl;
      siblingL = sibling->childl;
      siblingR = sibling->childr;
     }
    //case 4
    siblingL->color = BLK_COLOR;
    sibling->color = p->color;
    p->color = BLK_COLOR;
    RotateRight(root, p, nil);
    cur = root;
   }
}

//*******************************************
//**********  misc operations  **************
//*******************************************

//function to do preorder travsersal, printing info
void preorder(rbnode * root, rbnode * nil)
{
  rbnode * curr_node;
  rbnode * parent;
  int count;

  if(root != nil)
   {
    preorder(root->childl, nil);
    preorder(root->childr, nil);
    if( (root->childl == nil) || (root->childr == nil) )
     {
      curr_node = root;
      parent = curr_node->parent;
      if(curr_node->color == BLK_COLOR)
        count = 1;
      else
        count = 0;

      while(parent != NULL)
       {

        curr_node = curr_node->parent;
        parent = curr_node->parent;
        if(curr_node->color == BLK_COLOR)
          count += 1;
       }

     }

   }
}

double pack(int x, int y, int z, int hr, int vr)
{
  double value;

  value = x*vr + y + z*hr*vr;

  return value;
}
