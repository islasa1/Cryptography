// Anthony Islas
// Data Structure: Binary Search Tree (BST)
// Date Written: 12 Oct 2015
// Last Revised: 12 Oct 2015
// This c file will be used for the implementation of a BST using binary node_ts, and a stack based traversal

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "bstree.h"

// BST Specific functions
void bst_init(bst_t* myBst)
{
  // Assign default values to the bst
  myBst->root = NULL;
  myBst->size = 0;
}

//***********************************************
//
// find function
//
//***********************************************

node_t* find(bst_t* myBst, void* findValue)
{
  // Is bst empty?
  if(myBst->root == NULL)
  {
    // Nothing to return
    printf("BST is empty.\n");
    return NULL;
  }
  
  node_t* findPtr = myBst->root;
  while(findPtr != NULL && findPtr->keyValue != findValue)
  {
    if(findValue < findPtr->keyValue) findPtr = findPtr->left;
    else findPtr = findPtr->right;
  } // END Search
  
  // Now see why we fell out
  if(findPtr == NULL) 
  {
    return NULL;
  }
  else return findPtr;
  
}

//***********************************************
//
// insertion function
//
//***********************************************

void insertNode(bst_t* myBst, node_t* newNode)
{ 
  // Is this the first node_t?
  if(myBst->root == NULL)
  {
    myBst->root = newNode;
    return;
  }
  
  // Otherwise
  // Check uniqueness
  node_t* check = find(myBst, newNode->keyValue);
  if(check != NULL)
  {
    // Error
    printf("Error: Unique key already exists. could not insert.\n");
    free(newNode);
    return;
  }
  
  
  // Assign the root
  node_t* trvPtr = myBst->root;
  myBst->size++;
  
  while(trvPtr != NULL)
  {    
    // Find which path to take
    if(newNode->keyValue < trvPtr->keyValue)
    {
      // If we are going to fall out of the tree
      if(trvPtr->left == NULL)
      {
        // Assign node_t and exit
        trvPtr->left = newNode;
        return;
      }
      else trvPtr = trvPtr->left;
    }
    else
    {
      // If we are going to fall out of the tree
      if(trvPtr->right == NULL)
      {
        // Assign node_t and exit
        trvPtr->right = newNode;
        return;
      }
      else trvPtr = trvPtr->right; 
    } // END Search
  } // END WHILE
}




