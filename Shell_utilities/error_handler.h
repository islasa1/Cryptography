#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <errno.h>

// Use these error handlers as follows:
//
// If return value can use perror(), use PERROR subset
// Else If return value is the error code, use STDERR subset
// Else the return value to check is based on developed 
//  code and ERROR subset should be used
// 
// Then If the return value is NULL for errors, use PTR subset
// Else If the return value is -1 for errors, use NUM subset
// Else make your own handler
//
// Then If failure of function call using X requires bool return,
//   use BOOL
// Else If failure of function call using X doesn't require return var,
//  use VOID


#define PERROR_PTR_BOOL(X)                     \
  if(X == NULL) {                              \
    perror("Error System call:");              \
    return false; }                            
    
#define PERROR_NUM_BOOL(X)                     \
  if(X == -1) {                                \
    perror("Error System call: ");             \
    return false; }

#define PERROR_NUM_PTR(X)                      \
  if(X == -1) {                                \
    perror("Error System call: ");             \
    return NULL; }
    
#define PERROR_PTR_VOID(X)                     \
  if(X == NULL) {                              \
    perror("Error System call: ");             \
    return; }                                  
    
#define PERROR_NUM_VOID(X)                     \
  if(X == -1) {                                \
    perror("Error System call: ");             \
    return; }                                  
    
#define STDERR_ERRNO_BOOL(X)                                    \
  if(int errorCode = X > 0) {                                   \
    printf("Error at %d, %s", __LINE__, strerror(errorCode) );  \
    return false; }                                                                                                           
    
#define STDERROR_ERRNO_VOID(X)                                  \
  if(int errorCode = X > 0) {                                   \
    printf("Error at %d, %s", __LINE__, strerror(errorCode) );  \
    return; }

// Internal Errors reported by developed code

//##### PTR subset #####
#define ERROR_PTR_BOOL(X)                     \
  if(X == NULL) {                             \
    printf("Error: Input NULL\n");            \
    return false; }                

#define ERROR_PTR_NUM(X)                      \
  if(X == NULL) {                             \
    printf("Error: Input NULL\n");            \
    return -1; }

#define ERROR_PTR_PTR(X)                      \
  if(X == NULL) {                             \
    printf("Error: Input NULL\n");            \
    return NULL; } 

#define ERROR_PTR_VOID(X)                     \
  if(X == NULL) {                             \
    printf("Error: Input NULL\n");            \
    return; } 

//##### NUM subset #####    
#define ERROR_NUM_BOOL(X)                     \
  if(X == -1) {                               \
    printf("Error: Bad Return\n");            \
    return false; }  

#define ERROR_NUM_NUM(X)                      \
  if(X == -1) {                               \
    printf("Error: Bad Return\n");            \
    return -1; }  

#define ERROR_NUM_PTR(X)                      \
  if(X == -1) {                               \
    printf("Error: Bad Return\n");            \
    return NULL; } 
    
#define ERROR_NUM_VOID(X)                     \
  if(X == -1) {                               \
    printf("Error: Bad Return\n");            \
    return; } 

//##### BOOL subset #####
#define ERROR_BOOL_PTR(X)                     \
  if(X == false) {                            \
    printf("Error: Returned False\n");        \
    return NULL; }  

#endif