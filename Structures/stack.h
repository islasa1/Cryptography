// Anthony Islas
// Data Structure: Stack (LIFO)
// Date Written: 12 Oct 2015
// Last Revised: 12 Oct 2015
// This header will be used for the implementation of a queue using a circular linked list 

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "item_node.h"

#ifndef ___STACK___
#define ___STACK___

#define itemS_t item_t


// Structure used for the entire linked list
typedef struct
{
  // Keep track of head
  itemS_t *head;
  // Keep track of queue size
  int size;
} stack_t;


// Prototype functions
bool stack_init(stack_t* userS);
bool isStackEmpty(stack_t* userS);
void printStack(stack_t* userS);
void push(stack_t* userS, void* keyToInsert);
itemS_t* pop(stack_t* userS);
void clearStack(stack_t* userS);

#endif
