// Anthony Islas
// Data Structure: Binary Search Tree (BST)
// Date Written: 12 Oct 2015
// Last Revised: 12 Oct 2015
// This header will be used for the implementation of a BST using binary nodes

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "item_node.h"
#include "stack.h"
#include "queue.h"
#include "btree.h"

#ifndef ___BST___
#define ___BST___

typedef struct 
{
  // Keep track of root
  node_t* root;
  
  // Know the size (in nodes) of the tree... Might be useful later (e.g. size restriction, drawing, etc.)
  int size;
} bst_t;

// Functions we will need*
void bst_init(bst_t* myBst);
void insertNode(bst_t* myBst, node_t* newNode);
node_t* find(bst_t* myBst, void* keyValue);


#endif
