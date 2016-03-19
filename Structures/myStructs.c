// Testing correct compilation of structures

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "queue.h"
#include "stack.h"


int main(void)
{
  QUEUE_t myQueue;
  STACK_t myStack;
  
  stack_init(&myStack);
  queue_init(&myQueue);
  
  for(int i = 0; i < 10; i++)
  {
    insertQueue(&myQueue,(void*) (rand() % 100));
    push(&myStack, (void*) (rand() % 50));
  }
  itemS_t* holdS = pop(&myStack);
  printf("Item popped: %d\n", holdS->keyValue);
  free(holdS);
  
  printf("Pushing 99...\n");
  push(&myStack, (void*) 99);
  holdS = pop(&myStack);
  printf("Item popped: %d", holdS->keyValue);
  free(holdS);
  
  
  printQueue(&myQueue);
  printStack(&myStack);
  
  itemQ_t* holdQ = removeQueue(&myQueue);
  
  printf("Item removed from queue: %d.\n", holdQ->keyValue);
  free(holdQ);
  
  printQueue(&myQueue);
  printStack(&myStack);
  
  clearQueue(&myQueue);
  clearStack(&myStack);
  
  
  return 0;
}
