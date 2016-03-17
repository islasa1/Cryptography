// Anthony Islas
// Data Structure: Stack (FIFO)
// Date Written: 12 Oct 2015
// Last Revised: 12 Oct 2015
// This c file will be used for the implementation of a stack using a circular linked list 

#include "stack.h"

//Make an init function so user can easily use
bool stack_init(stack_t* userS)
{
  // Set function pointers are initialize head
  userS->head = NULL;
  userS->size = 0;
  return true;
}

//***********************************************
//
// isEmpty predicate
//
//***********************************************

bool isStackEmpty(stack_t* userS)
{
  if(userS->head == NULL) return true;
  else return false;
}



//***********************************************
//
// print queue
//
//***********************************************
void printStack(stack_t* userS)
{
// Find head
  itemS_t **head = &userS->head;
  
  // Go into loop if head does not point to null
  if((*head) == NULL)
  {
    // List is empty
    printf("Stack is empty\n");
    return;
  }
  // else
  itemS_t* trvPtr = (*head);
  printf("\nStack is: %d -> ", INT_CAST (trvPtr->keyValue));

  // Continue to traverse list
  trvPtr = trvPtr->next;
  
  while(trvPtr != NULL)
  {
    // Print
    printf("%d -> ", INT_CAST (trvPtr->keyValue));
    
    // Continue to traverse list
    trvPtr = trvPtr->next;
   
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
void push(stack_t* userS, void* keyToInsert)
{
  // Find head
  itemS_t **head = &userS->head;
  
  // Create new item
  itemS_t* temp = malloc(sizeof(itemS_t));
  temp->keyValue = keyToInsert;
  
  userS->size++;
  
  // Reassign pushback
  temp->next = *head;
  (*head) = temp;
  return;
  
}

//***********************************************
//
// removal
//
//***********************************************

itemS_t* pop(stack_t* userS)
{
  // Find head
  itemS_t **head = &userS->head;
  
  itemS_t* rmvPtr = NULL;
  
  // FIRST check if list is empty
  if((*head) == NULL)
  {
    // List is already empty
#ifdef DEBUG
    printf("Stack empty.\n");
#endif
    return NULL; 
  }
  // else
  // Remove from list
  rmvPtr = (*head);
  (*head) = (*head)->next;
  
  userS->size--;
  
  
  // return the value to main
  return rmvPtr;

}

void clearStack(stack_t* userS)
{
  // Find head
  itemS_t **head = &userS->head;
  
  itemS_t* rmvPtr = NULL;
  
  while((*head) != NULL)
  {
    // Remove from list
    rmvPtr = (*head);
    // return the memory to OS
    free(rmvPtr);
    
    (*head) = (*head)->next;
    
  }
  
  // Check if list is empty
  if((*head) == NULL)
  {
    // List is already empty
    printf("Stack empty.\n"); 
    userS->size = 0;
  }
}
