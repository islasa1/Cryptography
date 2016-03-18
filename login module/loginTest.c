#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "login.h"

// Test driver to login
int main(void)
{
	char userInput[LINE_LEN];
	bool in = false;
	while(!in)
	{
		printf("Enter option [l login, n new user]: ");
		readCommand(userInput);
		in = loginProtocol(userInput[0]);
	}
	char username[MAX_NAME];
	loginGetUsername(username);
	printf("Logged in as: %s\n", username);
}