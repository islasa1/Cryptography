// Anthony Islas
// Data Structure: Base structure
// Date Written: 12 Oct 2015
// Last Revised: 12 Oct 2015
// This header will be used for structure (C type) definition for all core implementation

#ifndef ___ITEM_NODE___
#define ___ITEM_NODE___

#include <stdint.h>

// Note: These structures use void* to allow abstract use with almost any data type
// caution MUST be used when using this however, and the user must know what and when
// to type cast data for retrieval/referencing

#define INT_CAST (int) (intptr_t)
#define CHAR_CAST (char) (intptr_t)
#define VOID_PTR (void*) (intptr_t)


//
// main structure used to created linked list, note a void* in the queue to hold any type of value
//
typedef struct linkedList
{
  void* keyValue;
  struct linkedList* next;
} item_t;


// 
// main structure used to create a binary tree
//
typedef struct binaryNode
{
  void* keyValue;
  void* data;
  
  // Point to children
  struct binaryNode* left;
  struct binaryNode* right;
  
} node_t;

#endif