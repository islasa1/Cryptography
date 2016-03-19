// Anthony Islas
// Data Structure: Queue (LIFO)
// Date Written: 8 Oct 2015
// Last Revised: 12 Oct 2015
// This c file will be used for the implementation of a queue using a circular linked list 

#include "queue.h"

//Make an init function so user can easily use
bool queue_init(QUEUE_t* userQ)
{
  // Set function pointers are initialize head
  userQ->head = NULL;
  userQ->size = 0;
  return true;
}

//***********************************************
//
// isEmpty predicate
//
//***********************************************

bool isQueueEmpty(QUEUE_t* userQ)
{
  if(userQ->head == NULL) return true;
  else return false;
}

//***********************************************
//
// print queue
//
//***********************************************
void printQueue(QUEUE_t* userQ)
{
// Find head
  itemQ_t **head = &userQ->head;
  
  // Go into loop if head does not point to null
  if((*head) == NULL)
  {
    // List is empty
    printf("Queue is empty\n");
    return;
  }
  // else
  itemQ_t* trvPtr = (*head)->next;
  printf(" %d", CHAR_CAST (trvPtr->keyValue));

  
  while(trvPtr != (*head))
  {
    // Continue to traverse list
    trvPtr = trvPtr->next;
    printf(" %d", CHAR_CAST (trvPtr->keyValue));
   
  }
  printf("\n");
  return;
}

// *NOTE: itemList** (pointer to pointer) is used in insertion and deletion function
//        because the functions exist outside of the main loop, and for special cases of moving 
//        the head of the list we need the actual pointer, not a copy.

//***********************************************
//
// insertion function
//
//***********************************************
void insertQueue(QUEUE_t* userQ, void* keyToInsert)
{
  // Find head
  itemQ_t **head = &userQ->head;
  
  // Create new item
  itemQ_t* temp = malloc(sizeof(itemQ_t));
  temp->keyValue = keyToInsert;
  
  userQ->size++;

  // FIRST check the first element
  if((*head) == NULL)
  {
    // Take care of special case for first item
    temp->next = temp;
    (*head) = temp;
    return;
  }
  else
  {
    // Insert value before
    temp->next = (*head)->next;
    (*head)->next = temp;
    (*head) = temp;
    return;
  }
  
}

//***********************************************
//
// removal
//
//***********************************************

itemQ_t* removeQueue(QUEUE_t* userQ)
{
  // Find head
  itemQ_t **head = &userQ->head;
  
  itemQ_t* rmvPtr = NULL;
  
  // FIRST check if list is empty
  if((*head) == NULL)
  {
    // List is already empty
#ifdef DEBUG
    printf("Queue empty");
#endif
    return NULL; 
  }
  // else
  // Remove from list
  rmvPtr = (*head)->next;
  if((*head)->next == (*head)) (*head) = NULL;
  else (*head)->next = (*head)->next->next;
  
  userQ->size--;
  
  
  // return the value to main
  return rmvPtr;

}

void clearQueue(QUEUE_t* userQ)
{
  // Find head
  itemQ_t **head = &userQ->head;
  
  itemQ_t* rmvPtr = NULL;
  
  while((*head) != NULL)
  {
    // else
    // Remove from list
    rmvPtr = (*head)->next;
    if((*head)->next == (*head)) (*head) = NULL;
    else (*head)->next = (*head)->next->next;
    
    // return the memory to OS
    free(rmvPtr);
  }
  
  // Check if list is empty
  if((*head) == NULL)
  {
    // List is already empty
#ifdef DEBUG
    printf("Queue empty.\n"); 
#endif
    userQ->size = 0;
  }
}
