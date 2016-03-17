// Anthony Islas
// Data Structure: Binary Search Tree (BST)
// Date Written: 12 Oct 2015
// Last Revised: 12 Oct 2015
// This c file will be used for the implementation binary tree general functions

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>


#include "btree.h"


// The use of a queue is not necessary, but rather than printing out the list and simply losing the traversal,
// it can be stored dynamically, and possibly used later (e.g. reconstruct a binary tree from 2 queues)

//***********************************************
//
// In-order traversal
//
//***********************************************

void inOrderTrv(node_t* root, queue_t* holdNodes)
{
  // Stack based algorith for binary tree in-order traversal
  node_t* trvPtr = root;
  stack_t nodeStack;
  stack_init(&nodeStack);
  bool done = false;
  clearQueue(holdNodes);
  
  printf("In-Order Traversal:\t");
  
  
  // Go until stack is empty
  while(!done)
  {
    while(trvPtr != NULL)
    {
      if(trvPtr->left != NULL)
      {
        // Save the node for later, it is the parent to visit later
        push(&nodeStack,(void*) trvPtr);
        trvPtr = trvPtr->left;
      }
      else 
      {
        // Visit the root
#ifdef DEBUG
        printf("\tPutting node with key value %c into queue\n", (char*) trvPtr->keyValue);
#endif
        insertQueue(holdNodes, (void*) trvPtr);
        // And in-order traverse the right-subtree, if null we will
        // pop the stack and go back up one level
        trvPtr = trvPtr->right;
      }
    } // END inner while
    
    done = isStackEmpty(&nodeStack);
    // Pop the stack if stack isn't empty yet, casting first
    if(!done)
    {
      // You have to visit the popped nodes, first pop
      trvPtr = ((node_t*) (pop(&nodeStack)->keyValue));
      
      // Visit the root
#ifdef DEBUG
      printf("\tPutting node with key value %c into queue\n", (char) (intptr_t) trvPtr->keyValue);
#endif
      insertQueue(holdNodes, (void*) trvPtr);
      
      // Now move
      trvPtr = trvPtr->right;
    }
  } // END outer while
}

//***********************************************
//
// Pre-order traversal
//
//***********************************************

void preOrderTrv(node_t* root, queue_t* holdNodes)
{
  // Stack based algorithm for binary tree pre-order traversal
  node_t* trvPtr = root;
  stack_t nodeStack;
  stack_init(&nodeStack);
  bool done = false;
  clearQueue(holdNodes);
  
  printf("Pre-Order Traversal:\t");
  
  // Go until stack is empty
  while(!done)
  {
    while(trvPtr != NULL)
    {
      // Visit the root
#ifdef DEBUG
      printf("\tPutting node with key value %c into queue\n", (char) (intptr_t) trvPtr->keyValue);
#endif
      insertQueue(holdNodes, (void*) trvPtr);
      if(trvPtr->left != NULL)
      {
        push(&nodeStack, (void*) trvPtr);
        trvPtr = trvPtr->left;
      }
      else trvPtr = trvPtr->right;
    } // END inner while
    
    done = isStackEmpty(&nodeStack);
    // Pop the stack if stack isn't empty yet, casting first
    if(!done) trvPtr = ((node_t*) (pop(&nodeStack)->keyValue))->right;
  } // END outer while
}

//***********************************************
//
// Post-order traversal
//
//***********************************************

void postOrderTrv(node_t* root, queue_t* holdNodes)
{
  // Stack based algorithm for binary tree post-order traversal
  // The auxiliary structure reversedRead just holds the post order
  // traversal in reversed order, fixed by popping into a queue (holdNodes)
  node_t* trvPtr = root;
  stack_t nodeStack, reversedRead;
  stack_init(&nodeStack);
  stack_init(&reversedRead);
  bool done = false;
  clearQueue(holdNodes);
  
  printf("Post-Order Traversal:\t");
  
  // Go until stack is empty
  // This a right-justified pre-order traversal
  while(!done)
  {
    while(trvPtr != NULL)
    {
      // Visit the root
#ifdef DEBUG
      printf("\tPutting node with key value %c into queue\n", (char) (intptr_t) trvPtr->keyValue);
#endif
      push(&reversedRead, (void*) trvPtr);
      if(trvPtr->right != NULL)
      {
        push(&nodeStack, (void*) trvPtr);
        trvPtr = trvPtr->right;
      }
      else trvPtr = trvPtr->left;
    } // END inner while
    
    done = isStackEmpty(&nodeStack);
    // Pop the stack if stack isn't empty yet, casting first
    if(!done) trvPtr = ((node_t*) (pop(&nodeStack)->keyValue))->left;
  } // END outer while
  
  // Traversal is done, now correct read
  while(!isStackEmpty(&reversedRead))
  {
    // Pop returns itemS_t (item_t) which has a keyValue type (void*)
    // which is argument (2) to insertQueue, but is really just the node_t*
    insertQueue(holdNodes, (pop(&reversedRead)->keyValue));
  }
}


//***********************************************
//
// Draw the binary tree in ascii using traversal order
// (Who would torture themselves to do such a thing...
//
//***********************************************

// TODO: be able to reconstruct tree from traversal orders
void treeDrawUnique(queue_t* inOrder, queue_t* otherOrder, bool whichOrder)
{

#ifdef DEBUG
  printf("Char array inOrder size: %d\nChar array otherOrder size: %d\n", inOrder->size + 1, otherOrder->size + 1);
#endif

  // Add one to have null terminator
  char inOrderList[inOrder->size + 1], otherOrderList[otherOrder->size + 1], usedNodes[inOrder->size + 1];
  int holdSize;

  // First preliminary checks to make sure it is possible
  if(inOrder->size == otherOrder->size)
  {
    if(otherOrder->size == 0)
    {
      //Tree is empty
      printf("\n*** DRAWING BINARY TREE USING PREORDER ***\n");
      printf("\t(Empty Tree)\n");
      return;
    }
    else
    {
      // continiue checks
      // Move queue into char arrays
      holdSize = inOrder->size;
      
      // reinitialize the char arrays
      for(int charIndex = 0; charIndex <= inOrder->size; charIndex++)
      { 
        inOrderList[charIndex] = '\0';
        otherOrderList[charIndex] = '\0';
        usedNodes[charIndex] = '\0';
      }
  
      int index = 0;
      node_t* current = NULL;
      
      // Since they are the same size, we can use the same loop
      while(!isQueueEmpty(inOrder)) 
      {
  
        
        // Remove from front
        current = (node_t*) (removeQueue(inOrder)->keyValue);
        // Put node key value into array
        inOrderList[index] = (char) (intptr_t) (current->keyValue);
        
        // Repeat
        current = (node_t*) (removeQueue(otherOrder)->keyValue);
        otherOrderList[index] = (char) (intptr_t) (current->keyValue);
        // Move index
        index++;
      }
      
      // Now that we have the char arrays, do the next check
      for(int iterator = 0; iterator <= inOrder->size; iterator++)
      {
        if(strchr(otherOrderList, inOrderList[iterator]) == NULL)
        {
          // Missing a key value
          printf("Error: Key value (%c) not found in other order", inOrderList[iterator]);
          return;
        } 
        
      } // END for
    } // End else
    
    
  } // End initial if
  // already wrong
  else 
  {
    printf("Error: Missing elements from one of the orders");
    return;
  }
  
  
  
  // Set it so if true, use inOrder and preOrder
  // False uses postOrder
  if(whichOrder)
  {
    printf("\n*** DRAWING BINARY TREE USING PREORDER ***\n");
    // Need 2 queues, one for current level, and one for next level
    // use pointers to simplify the exchange of data from one to another
    queue_t* currentLvl = malloc(sizeof(queue_t));
    queue_t* nextLvl = malloc(sizeof(queue_t));
    queue_init(currentLvl);
    queue_init(nextLvl);
    
    // Pre-order, first element is the root
    
    insertQueue(currentLvl, (void*) (intptr_t) otherOrderList[0]);
    usedNodes[0] = otherOrderList[0];
    int preOrderCnt;
    int usedNodeCount = 1;
    
    // Go until we reach a lvl where there are no nodes
    
    while(!isQueueEmpty(currentLvl) || !isQueueEmpty(nextLvl))
    {
    // Go through the current level
    char lastChar = 0;
    preOrderCnt = 1;
    
    
    while(!isQueueEmpty(currentLvl))
    {
      int charToPrintIndex;
      bool leftDone = false, rightDone = false; 
      char charToPrint = (char) (intptr_t) (removeQueue(currentLvl)->keyValue);
      
      char* charPtr = NULL;
      
      
      // Find subtrees to evaluate in next level
      for(int index = preOrderCnt; index < holdSize; index++)
      {
        charPtr = strchr(inOrderList, otherOrderList[index]);
        // Check for left subtrees
        // Must be:
        // No subtree exist yet
        // Before the current node in order
        // Not already used
        // Cannot pass over any used nodes
        if(!leftDone && strchr(inOrderList, charToPrint) > charPtr && strchr(usedNodes, *charPtr) == NULL)
        {
          // Assuming it can pass the validation test we will assign tree
          bool valid = true;
          // Found a valid candidate, check to make sure it does not cross over any used nodes
          // Offset so it does not check for the node currently being used (obviously a used node)
          for(int iterator = strchr(inOrderList, charToPrint) - inOrderList - 1;\
              iterator > strchr(inOrderList, *charPtr) - inOrderList;\
              iterator--)
          {
            // Iterate through all indexes from the current node to the candidate left subtree
            if(strchr(usedNodes, inOrderList[iterator]) != NULL)    valid = false;
          }
          
          if(valid)
          {
            // Found a left subtree to the node just removed from current level
            // Cannot have another left subtree
            leftDone = true;
            
            // Put left subtree into queue for next level
            insertQueue(nextLvl, (void*) (intptr_t) (*charPtr));
            
            // Put "node" under used nodes
            usedNodes[usedNodeCount] = *charPtr;
            usedNodeCount++;
          }
        } // END left subtree check
        
        // Check for right subtrees
        // Must be:
        // No subtree exist yet
        // After the current node in order
        // Not already used
        // Cannot pass over any used nodes
        if(!rightDone && strchr(inOrderList, charToPrint) < charPtr && strchr(usedNodes, *charPtr) == NULL)
        {
          // Assuming it can pass the validation test we will assign tree
          bool valid = true;
          // Found a valid candidate, check to make sure it does not cross over any used nodes
          // Offset so it does not check for the node currently being used (obviously a used node)
          for(int iterator = strchr(inOrderList, charToPrint) - inOrderList + 1;\
              iterator < strchr(inOrderList, *charPtr) - inOrderList;\
              iterator++)
          {
            // Iterate through all indexes from the current node to the candidate right subtree
            if(strchr(usedNodes, inOrderList[iterator]) != NULL)  valid = false;         
          }
          if(valid)
          {
            // Found a left subtree to the node just removed from current level
            // Cannot have another left subtree
            rightDone = true;

            // Put left subtree into queue for next level
            insertQueue(nextLvl, (void*) (intptr_t) (*charPtr));
            
            // Put "node" under used nodes
            usedNodes[usedNodeCount] = *charPtr;
            usedNodeCount++;
          }
        } // END right subtree check
      } // END for
      
      for(int spaces = 0;\
        spaces < (lastChar == 0 ? strchr(inOrderList, charToPrint) - inOrderList : strchr(inOrderList, charToPrint) - strchr(inOrderList, lastChar) - 1);\
        spaces++)
      {
        // Put into console however many spaces between the last character input on that level and the character to input according to inOrder
        printf("  ");
      }
      
      // Now print the character
      printf("%c  ", charToPrint);
      lastChar = charToPrint;
      
      // continiue with the current level while not empty, moving preorder count
      preOrderCnt++;
    } // END while
    // Current level is finished, move onto next level
    
    currentLvl = nextLvl;
    nextLvl = malloc(sizeof(queue_t));;  
    
    // New level
    printf("\n");
      
    } // End entire tree
    
  }
  else
  {
    printf("\n*** DRAWING BINARY TREE USING PREORDER ***\n");
    
    // Sorry don't know yet
    printf("Error: Format not supported yet\n");
  
  }


}


//***********************************************
//
// Draw the binary tree in ascii using a root node
// Also gruesomely painful
//
//***********************************************

// Pad function just adds numSpaces spaces to line
void pad(int numSpaces)
{
  for(int count = 0; count < numSpaces; count++)
  {
    printf(" ");
  }
}


void treeDrawGeneric(node_t* root)
{

  printf("\n*** DRAWING BINARY TREE GENERICALLY ***\n");

  node_t* trvPtr = root;
  bool done = false;
  int countLvl = 0;
  
  // Need 2 queues, one for current level, and one for next level
  // use pointers to simplify the exchange of data from one to another
  queue_t* currentLvl = malloc(sizeof(queue_t));
  queue_t* nextLvl = malloc(sizeof(queue_t));
  queue_t* levels = malloc(sizeof(queue_t));
  queue_init(currentLvl);
  queue_init(nextLvl);
  queue_init(levels);
  
  // Insert into the current lvl queue to start the traversals
  insertQueue(currentLvl, (void*) (intptr_t) trvPtr);
  while(!done)
  {
    // Assume we are done
    done = true;
    
    // Prep the level
    int maxNodes = pow(2,countLvl);
    int nodeIndex = 0;
    char* nodeString = malloc(maxNodes + 1);
    
    // Reinit the char array
    for(int charIndex = 0; charIndex <= maxNodes; charIndex++)
    {
      nodeString[charIndex] = '\0';
    }
    
    while(!isQueueEmpty(currentLvl))
    {
      // Convert to a node_t*
      itemQ_t* holdNode = (removeQueue(currentLvl));
      trvPtr = (node_t*) (intptr_t) (holdNode->keyValue);
      nodeString[nodeIndex] = (char) (intptr_t) (trvPtr->keyValue);
      
      //printf("Node: %d for Level: %d\n\tChar: %c\n", nodeIndex, countLvl, nodeString[nodeIndex]);
      
      if(trvPtr->left == NULL)
      {
        // Create a blank node
        node_t* newNode = malloc(sizeof(node_t));
        newNode->keyValue = (void*) (intptr_t) ' ';
        newNode->data = (void*) (intptr_t) 0;
        
        insertQueue(nextLvl, newNode);
        
      }
      else
      {
        insertQueue(nextLvl, trvPtr->left);
        // If we have an actual node left in the next level
        if( ((char) (intptr_t) (trvPtr->left->keyValue)) != ' ') done = false;
      }
      
      if(trvPtr->right == NULL)
      {
        // Create a blank node
        node_t* newNode = malloc(sizeof(node_t));
        newNode->keyValue = (void*) (intptr_t) ' ';
        newNode->data = (void*) (intptr_t) 0;
        insertQueue(nextLvl, newNode);
      }
      else
      {
        insertQueue(nextLvl, trvPtr->right);
        // If we have an actual node left in the next level
        if( ((char) (intptr_t) (trvPtr->right->keyValue)) != ' ') done = false;
      }
      
      // Prevent memory leaks?
      free(holdNode);
      nodeIndex++;  
      
      
    } // End current level traversal
    
    
    // Prep for next level
    insertQueue(levels,(void*) (intptr_t) nodeString);
    
    
    currentLvl = nextLvl;
    nextLvl = malloc(sizeof(queue_t));
    countLvl++; 
  
  } // End all level traversals
  
  // Print levels with padding
  for(int curLvl = 0; curLvl < countLvl; curLvl++)
  {
    // Current level
    // Prepad the level with 2^(max levels - 1 - current level) - 1
    // The -1 of max levels is to fix the offset created by last iteration of
    // the while loop, the -current level fixes it for which ever we are at
    // and -1 at end allows the next char to print in that place next
    pad((pow(2, countLvl - 1 - curLvl) - 1));
    
    // Grab the current level's string
    itemQ_t* level = (removeQueue(levels));
    char* nodesToPrint = (char*) (intptr_t) (level->keyValue);
    
    for(int curNode = 0; curNode < pow(2,curLvl); curNode++)
    {
      // Current node
      // Print the node
      printf("%c", nodesToPrint[curNode]);
      pad((pow(2, countLvl - curLvl) - 1));
    }
    
    free(nodesToPrint);
    free(level);
    // Next level
    printf("\n");
  } // End print all levels

  // End print binary tree 
}
