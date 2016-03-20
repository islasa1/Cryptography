// login.h - Header for login module
// Provide structure layout for storing user data for login, password recovery
// 
// Anthony Islas
// Developed for use in CS 415 HCI originally, modified for use in
// SE420 SQA. Took out recovery security questions. Not an issue right now..
// Date Written: 6 Sept 2015
// Last Revised: 16 March 2016
//
#ifndef LOGIN_H
#define LOGIN_H

#include <stdio.h>
#include <stdbool.h>

#define MAX_USERS  10
#define MAX_NAME   9
#define MAX_PASSWD 14
#define MIN_INPUT  6
#define	LINE_LEN	 512
#define DEF_CHAR   "NULL"
#define _XOPEN_SOURCE
#define PASSPHRASE "passwords.txt"

#ifndef ASCII_A
#define ASCII_A 65
#endif

// Macro init

#define INIT_USER(USER)                			 \
  { USER.user_id = -1; 			                 \
    strcpy(USER.user_name, DEF_CHAR);        \
    strcpy(USER.user_passwd, DEF_CHAR);      \
  }


typedef struct userStruct
{
	int  user_id;
	char user_name[MAX_NAME];
	char user_passwd[MAX_PASSWD];
} users_t;

// Prototypes
void readInput(char *);
void readPasswd(char *, const char *, char *);
bool login(users_t*);
bool newAccount(users_t*);
bool loginProtocol(char);
void loginGetUsername(char curName[MAX_NAME]);
int loginGetCurUser(void);
void loginSetCurUser(int);

#endif