// Anthony Islas
// Data Structure: Binary Search Tree (BST)
// Date Written: 12 Oct 2015
// Last Revised: 12 Oct 2015
// This header will be used for the implementation binary tree general functions

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "item_node.h"
#include "stack.h"
#include "queue.h"

#ifndef ___BTREE___
#define ___BTREE___

// *These functions are unique to the class tree, which then can be used by 
// other data structures of the same type (i.e. AVL)
void inOrderTrv(node_t* root, queue_t* holdNodes);
void preOrderTrv(node_t* root, queue_t* holdNodes);
void postOrderTrv(node_t* root, queue_t* holdNodes);

// This will draw the binary tree based on the order traversals given to it
void treeDrawUnique(queue_t* inOrder, queue_t* otherOrder, bool whichOrder);
void treeDrawGeneric(node_t* root);


#endif