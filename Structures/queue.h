// Anthony Islas
// Data Structure: Queue (FIFO)
// Date Written: 8 Oct 2015
// Last Revised: 12 Oct 2015
// This header will be used for the implementation of a queue using a circular linked list 

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>


#include "item_node.h" 


#ifndef ___QUEUE___
#define ___QUEUE___

#define itemQ_t item_t


// Structure used for the entire linked list
typedef struct
{
  // Keep track of head
  itemQ_t *head;
  // Keep track of queue size
  int size;
} queue_t;


// Prototype functions
bool queue_init(queue_t* userQ);
bool isQueueEmpty(queue_t* userQ);
void printQueue(queue_t* userQ);
void insertQueue(queue_t* userQ, void* keyToInsert);
itemQ_t* removeQueue(queue_t* userQ);
void clearQueue(queue_t* userQ);

#endif
